#include <nitro.h>
#include <string.h>
#include <nnsys/g2d.h>

#include "sprite_system.h"

#include "assert.h"
#include "heap.h"
#include "cell_transfer.h"
#include "char_transfer.h"
#include "narc.h"
#include "palette.h"
#include "pltt_transfer.h"
#include "render_oam.h"
#include "sprite.h"
#include "sprite_resource.h"
#include "sprite_transfer.h"
#include "sprite_util.h"
#include "system.h"


static Sprite* CreateSpriteFromResourceHeader(SpriteSystem* spriteSys, SpriteManager* spriteMan, int resourceHeaderID, s16 x, s16 y, s16 z, u16 animIdx, int priority, int plttIdx, NNS_G2D_VRAM_TYPE vramType, int param10, int param11, int param12, int param13);


SpriteSystem* SpriteSystem_Alloc(HeapID heapID) {
	SpriteSystem* spriteSys = Heap_Alloc(heapID, sizeof(SpriteSystem));
	if (spriteSys == NULL) {
		return NULL;
	}
	
	spriteSys->heapID = heapID;
	spriteSys->spriteManagerCount = 0;
	spriteSys->inUse = TRUE;
	
	return spriteSys;
}


SpriteManager* SpriteManager_New(SpriteSystem* spriteSys) {
	GF_ASSERT(spriteSys != NULL);
	
	SpriteManager* spriteMan = Heap_Alloc(spriteSys->heapID, sizeof(SpriteManager));
	if (spriteMan == NULL) {
		return NULL;
	}
	
	spriteSys->spriteManagerCount++;
	for (int i = 0; i < SPRITE_RESOURCE_MAX; i++) {
		spriteMan->ownedResources[i] = NULL;
	}
	
	return spriteMan;
}


BOOL SpriteSystem_Init(SpriteSystem* spriteSys, const RenderOamTemplate* oamTemplate, const CharTransferTemplateWithModes* transferTemplate, int plttCapacity) {
	GF_ASSERT(spriteSys != NULL);
	if (spriteSys == NULL) {
		return FALSE;
	}
	
	CharTransferTemplate charTransferTemplate;
	charTransferTemplate.maxTasks = transferTemplate->maxTasks;
	charTransferTemplate.sizeMain = transferTemplate->sizeMain;
	charTransferTemplate.sizeSub = transferTemplate->sizeSub;
	charTransferTemplate.heapID = spriteSys->heapID;
	CharTransfer_InitWithVramModes(&charTransferTemplate, transferTemplate->modeMain, transferTemplate->modeSub);
	PlttTransfer_Init(plttCapacity, spriteSys->heapID);
	NNS_G2dInitOamManagerModule();
	
	if (spriteSys->inUse == TRUE) {
		RenderOam_Init(oamTemplate->mainOamStart, oamTemplate->mainOamCount, oamTemplate->mainAffineOamStart, oamTemplate->mainAffineOamCount, oamTemplate->subOamStart, oamTemplate->subOamCount, oamTemplate->subAffineOamStart, oamTemplate->subAffineOamCount, spriteSys->heapID);
	}
	
	spriteSys->cellTransferStates = CellTransfer_New(32, spriteSys->heapID);
	CharTransfer_ClearBuffers();
	PlttTransfer_Clear();
	
	return TRUE;
}


BOOL SpriteSystem_InitSprites(SpriteSystem* spriteSys, SpriteManager* spriteMan, int maxSprites) {
	if (spriteSys == NULL || spriteMan == NULL) {
		return FALSE;
	}
	
	spriteMan->sprites = SpriteList_InitRendering(maxSprites, &spriteSys->renderer, spriteSys->heapID);
	return TRUE;
}


void SpriteSystem_DrawSprites(SpriteManager* spriteMan) {
	GF_ASSERT(spriteMan != NULL);
	SpriteList_Update(spriteMan->sprites);
}


void SpriteSystem_TransferOam(void) {
	RenderOam_Transfer();
}


void SpriteManager_DeleteAllSprites(SpriteManager* spriteMan) {
	SpriteList_Delete(spriteMan->sprites);
}


void SpriteManager_FreeResourceHeaders(SpriteManager* spriteMan) {
	if (spriteMan->resourceHeaders == NULL) {
		return;
	}
	
	SpriteResourcesHeaderList_Free(spriteMan->resourceHeaders);
}


void SpriteManager_FreeResources(SpriteManager* spriteMan) {
	for (int i = 0; i < spriteMan->numResourceTypes; i++) {
		SpriteResourceTable* resTable = SpriteResourceTable_GetArrayElement(spriteMan->resourcePaths, i);
		SpriteResourceTable_Clear(resTable);
	}
	
	Heap_Free(spriteMan->resourcePaths);
	SpriteTransfer_ResetCharTransferList(spriteMan->unownedResources[SPRITE_RESOURCE_CHAR]);
	SpriteTransfer_ResetPlttTransferList(spriteMan->unownedResources[SPRITE_RESOURCE_PLTT]);
	
	for (int i = 0; i < spriteMan->numResourceTypes; i++) {
		SpriteResourceList_Delete(spriteMan->unownedResources[i]);
		SpriteResourceCollection_Delete(spriteMan->ownedResources[i]);
	}
}


void SpriteSystem_FreeVramTransfers(SpriteSystem* spriteSys) {
	CellTransfer_Free(spriteSys->cellTransferStates);
	CharTransfer_Free();
	PlttTransfer_Free();
	
	if (spriteSys->inUse == TRUE) {
		RenderOam_Free();
	}
}


void SpriteSystem_FreeSpriteManager(SpriteSystem* spriteSys, SpriteManager* spriteMan) {
	spriteSys->spriteManagerCount--;
	Heap_Free(spriteMan);
}


void SpriteSystem_DestroySpriteManager(SpriteSystem* spriteSys, SpriteManager* spriteMan) {
	SpriteManager_DeleteAllSprites(spriteMan);
	SpriteManager_FreeResourceHeaders(spriteMan);
	SpriteManager_FreeResources(spriteMan);
	SpriteSystem_FreeSpriteManager(spriteSys, spriteMan);
}


void SpriteSystem_Free(SpriteSystem* spriteSys) {
	GF_ASSERT(spriteSys->spriteManagerCount == 0);
	SpriteSystem_FreeVramTransfers(spriteSys);
	Heap_Free(spriteSys);
}


BOOL SpriteSystem_LoadResourceDataFromFilepaths(SpriteSystem* spriteSys, SpriteManager* spriteMan, const SpriteResourceDataPaths* paths) {
	int numResourceTypes = SPRITE_RESOURCE_MAX;
	
	if (spriteSys == NULL || spriteMan == NULL) {
		return FALSE;
	}
	
	if (paths->asStruct.mcellResources == NULL) {
		numResourceTypes = SPRITE_RESOURCE_MAX - 2;
	}
	
	spriteMan->numResourceTypes = numResourceTypes;
	int resourceCount = SpriteResourceTable_GetSize();
	spriteMan->resourcePaths = Heap_Alloc(spriteSys->heapID, resourceCount * numResourceTypes);
	
	for (int i = 0; i < numResourceTypes; i++) {
		SpriteResourceTable* resourceTable = SpriteResourceTable_GetArrayElement(spriteMan->resourcePaths, i);
		void* buf = ReadFileToHeap(spriteSys->heapID, paths->asArray[i]);
		SpriteResourceTable_LoadFromBinary(buf, resourceTable, spriteSys->heapID);
		Heap_Free(buf);
	}
	
	for (int i = 0; i < numResourceTypes; i++) {
		SpriteResourceTable* resourceTable = SpriteResourceTable_GetArrayElement(spriteMan->resourcePaths, i);
		resourceCount = SpriteResourceTable_GetCount(resourceTable);
		spriteMan->ownedResources[i] = SpriteResourceCollection_New(resourceCount, i, spriteSys->heapID);
	}
	
	for (int i = 0; i < numResourceTypes; i++) {
		SpriteResourceTable* resourceTable = SpriteResourceTable_GetArrayElement(spriteMan->resourcePaths, i);
		resourceCount = SpriteResourceTable_GetCount(resourceTable);
		spriteMan->unownedResources[i] = SpriteResourceList_New(resourceCount, spriteSys->heapID);
		spriteMan->loadedResourceCount[i] = SpriteResourceCollection_Extend(spriteMan->ownedResources[i], resourceTable, spriteMan->unownedResources[i], spriteSys->heapID);
	}
	
	SpriteTransfer_RequestCharList(spriteMan->unownedResources[SPRITE_RESOURCE_CHAR]);
	SpriteTransfer_RequestPlttWholeRangeList(spriteMan->unownedResources[SPRITE_RESOURCE_PLTT]);
	
	void* buf = ReadFileToHeap(spriteSys->heapID, paths->asStruct.spriteTable);
	spriteMan->resourceHeaders = SpriteResourcesHeaderList_NewFromResdat(buf,
		spriteSys->heapID,
		spriteMan->ownedResources[SPRITE_RESOURCE_CHAR],
		spriteMan->ownedResources[SPRITE_RESOURCE_PLTT],
		spriteMan->ownedResources[SPRITE_RESOURCE_CELL],
		spriteMan->ownedResources[SPRITE_RESOURCE_ANIM],
		spriteMan->ownedResources[SPRITE_RESOURCE_MULTI_CELL],
		spriteMan->ownedResources[SPRITE_RESOURCE_MULTI_ANIM]);
	
	Heap_Free(buf);
	return TRUE;
}


Sprite* SpriteSystem_NewSpriteFromResourceHeader(SpriteSystem* spriteSys, SpriteManager* spriteMan, const SpriteTemplateFromResourceHeader* template) {
	return CreateSpriteFromResourceHeader(spriteSys,
		spriteMan,
		template->resourceHeaderID,
		template->x,
		template->y,
		template->x,
		template->animIdx,
		template->priority,
		template->plttIdx,
		template->vramType,
		template->dummy18,
		template->dummy1C,
		template->dummy20,
		template->dummy24);
}


static Sprite* CreateSpriteFromResourceHeader(SpriteSystem* spriteSys, SpriteManager* spriteMan, int resourceHeaderID, s16 x, s16 y, s16 z, u16 animIdx, int priority, int plttIdx, NNS_G2D_VRAM_TYPE vramType, int param10, int param11, int param12, int param13) {
	AffineSpriteListTemplate template;
	
	template.list = spriteMan->sprites;
	template.resourceData = &spriteMan->resourceHeaders->headers[resourceHeaderID];
	template.position.x = FX32_CONST(x);
	template.position.y = FX32_CONST(y);
	template.position.z = FX32_CONST(z);
	
	if (vramType == NNS_G2D_VRAM_TYPE_2DSUB) {
		template.position.y += (192 << FX32_SHIFT);
	}
	
	template.affineScale.x = FX32_ONE;
	template.affineScale.y = FX32_ONE;
	template.affineScale.z = FX32_ONE;
	template.affineZRotation = 0;
	template.priority = priority;
	template.vramType = vramType;
	template.heapID = spriteSys->heapID;
	
	Sprite* sprite = SpriteList_AddAffine(&template);
	if (sprite != NULL) {
		Sprite_SetAnim(sprite, animIdx);
		Sprite_SetExplicitPalette(sprite, plttIdx);
	}
	
	return sprite;
}


void Sprite_SetDrawFlag2(Sprite* sprite, BOOL draw) {
	Sprite_SetDrawFlag(sprite, draw);
}


void Sprite_SetExplicitPalette2(Sprite* sprite, int palette) {
	Sprite_SetExplicitPalette(sprite, palette);
}


void Sprite_SetPositionXY(Sprite* sprite, s16 x, s16 y) {
	VecFx32 position;
	position.x = x * FX32_ONE;
	position.y = y * FX32_ONE;
	if (Sprite_GetVRamType(sprite) == NNS_G2D_VRAM_TYPE_2DSUB) {
		position.y += (192 << FX32_SHIFT);
	}
	position.z = 0;
	
	Sprite_SetPosition(sprite, &position);
}


void Sprite_GetPositionXY(Sprite* sprite, s16* outX, s16* outY) {
	const VecFx32* position = Sprite_GetPosition(sprite);
	*outX = position->x / FX32_ONE;
	if (Sprite_GetVRamType(sprite) == NNS_G2D_VRAM_TYPE_2DSUB) {
		*outY = (position->y - (192 << FX32_SHIFT)) / FX32_ONE;
	} else {
		*outY = position->y / FX32_ONE;
	}
}


void SpriteSystem_ReplaceCharResObj(SpriteSystem* spriteSys, SpriteManager* spriteMan, NarcID narcID, int memberIdx, BOOL compressed, int resourceID) {
	SpriteResource* resource = SpriteResourceCollection_Find(spriteMan->ownedResources[SPRITE_RESOURCE_CHAR], resourceID);
	SpriteResourceCollection_ModifyTiles(spriteMan->ownedResources[SPRITE_RESOURCE_CHAR],
		resource,
		narcID,
		memberIdx,
		compressed,
		spriteSys->heapID);
	SpriteTransfer_RetransferCharData(resource);
}


void SpriteSystem_ReplacePlttResObj(SpriteSystem* spriteSys, SpriteManager* spriteMan, NarcID narcID, int memberIdx, BOOL compressed, int resourceID) {
	SpriteResource* resource = SpriteResourceCollection_Find(spriteMan->ownedResources[SPRITE_RESOURCE_PLTT], resourceID);
	SpriteResourceCollection_ModifyPalette(spriteMan->ownedResources[SPRITE_RESOURCE_PLTT],
		resource,
		narcID,
		memberIdx,
		compressed,
		spriteSys->heapID);
	SpriteTransfer_ReplacePlttData(resource);
}
