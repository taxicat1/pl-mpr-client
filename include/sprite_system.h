#ifndef MPR_SPRITE_SYSTEM_H
#define MPR_SPRITE_SYSTEM_H

#include <nitro/gx.h>

#include "constants/heap.h"

#include "char_transfer.h"
#include "narc.h"
#include "palette.h"
#include "render_oam.h"
#include "sprite.h"
#include "sprite_resource.h"
#include "sprite_util.h"

#define SPRITE_RESOURCE_NONE  (-1)

typedef struct {
	HeapID                    heapID;
	int                       spriteManagerCount;
	BOOL                      inUse;
	NNSG2dCellTransferState*  cellTransferStates;
	G2dRenderer               renderer;
} SpriteSystem;

typedef struct {
	SpriteList*                 sprites;
	SpriteResourcesHeaderList*  resourceHeaders;
	SpriteResourceTable*        resourcePaths;
	SpriteResourceCollection*   ownedResources[SPRITE_RESOURCE_MAX];
	SpriteResourceList*         unownedResources[SPRITE_RESOURCE_MAX];
	int                         loadedResourceCount[SPRITE_RESOURCE_MAX];
	int                         numResourceTypes;
} SpriteManager;

typedef struct {
	int                resourceHeaderID;
	s16                x;
	s16                y;
	s16                z;
	u16                animIdx;
	int                priority;
	int                plttIdx;
	NNS_G2D_VRAM_TYPE  vramType;
	
	int  dummy18;
	int  dummy1C;
	int  dummy20;
	int  dummy24;
} SpriteTemplateFromResourceHeader;

typedef union {
	const char* asArray[SPRITE_RESOURCE_MAX + 1];
	struct {
		const char*  charResources;
		const char*  plttResources;
		const char*  cellResources;
		const char*  animResources;
		const char*  mcellResources;
		const char*  manimResources;
		const char*  spriteTable;
	} asStruct;
} SpriteResourceDataPaths;

SpriteSystem* SpriteSystem_Alloc(HeapID heapID);
SpriteManager* SpriteManager_New(SpriteSystem* spriteSys);
BOOL SpriteSystem_Init(SpriteSystem* spriteSys, const RenderOamTemplate* oamTemplate, const CharTransferTemplateWithModes* transferTemplate, int plttCapacity);
BOOL SpriteSystem_InitSprites(SpriteSystem* spriteSys, SpriteManager* spriteMan, int maxSprites);
void SpriteSystem_DrawSprites(SpriteManager* spriteMan);
void SpriteSystem_TransferOam(void);
void SpriteManager_DeleteAllSprites(SpriteManager* spriteMan);
void SpriteManager_FreeResourceHeaders(SpriteManager* spriteMan);
void SpriteManager_FreeResources(SpriteManager* spriteMan);
void SpriteSystem_FreeVramTransfers(SpriteSystem* spriteSys);
void SpriteSystem_FreeSpriteManager(SpriteSystem* spriteSys, SpriteManager* spriteMan);
void SpriteSystem_DestroySpriteManager(SpriteSystem* spriteSys, SpriteManager* spriteMan);
void SpriteSystem_Free(SpriteSystem* spriteSys);
BOOL SpriteSystem_LoadResourceDataFromFilepaths(SpriteSystem* spriteSys, SpriteManager* spriteMan, const SpriteResourceDataPaths* paths);
Sprite* SpriteSystem_NewSpriteFromResourceHeader(SpriteSystem* spriteSys, SpriteManager* spriteMan, const SpriteTemplateFromResourceHeader* spriteTemplate);
void Sprite_SetDrawFlag2(Sprite* sprite, BOOL draw);
void Sprite_SetExplicitPalette2(Sprite* sprite, int palette);
void Sprite_SetPositionXY(Sprite* sprite, s16 x, s16 y);
void Sprite_GetPositionXY(Sprite* sprite, s16* outX, s16* outY);
void SpriteSystem_ReplaceCharResObj(SpriteSystem* spriteSys, SpriteManager* spriteMan, NarcID narcID, int memberIdx, BOOL compressed, int resourceID);
void SpriteSystem_ReplacePlttResObj(SpriteSystem* spriteSys, SpriteManager* spriteMan, NarcID narcID, int memberIdx, BOOL compressed, int resourceID);

#endif /* MPR_SPRITE_SYSTEM_H */