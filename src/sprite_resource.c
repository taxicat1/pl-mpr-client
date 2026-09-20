#include <nitro.h>
#include <string.h>
#include <nnsys/g2d/g2d_Image.h>

#include "sprite_resource.h"

#include "constants/heap.h"
#include "assert.h"
#include "graphics.h"
#include "heap.h"
#include "narc.h"
#include "resource_collection.h"

#define SPRITE_VRAM_TYPE_DEFAULT  NNS_G2D_VRAM_TYPE_3DMAIN
#define NARC_INDEX_NONE           0xFFFFFFFE

typedef struct {
	NNSG2dCharacterData* tileData;
	NNS_G2D_VRAM_TYPE vramType;
} TileResourceData;

typedef struct {
	NNSG2dPaletteData* paletteData;
	NNS_G2D_VRAM_TYPE vramType;
	int paletteIndex;
} PaletteResourceData;

typedef struct {
	NNSG2dCellDataBank* spriteBank;
} SpriteResourceData;

typedef struct {
	NNSG2dAnimBankData* animBank;
} SpriteAnimResourceData;

typedef struct {
	NNSG2dMultiCellDataBank* multiSpriteBank;
} MultiSpriteResourceData;

typedef struct {
	NNSG2dAnimBankData* multiSpriteAnimBank;
} MultiSpriteAnimResourceData;

typedef struct {
	SpriteResourceType type;
	union {
		SpriteResourceTableEntryFile  fileEntries[1];
		SpriteResourceTableEntryNARC  narcEntries[1];
	};
} SpriteResourceTableBinary;

static SpriteResource* SpriteResourceCollection_AllocResource(SpriteResourceCollection* spriteResources);
static void SpriteResourceCollection_InitResFromFile(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, const char* filename, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, SpriteResourceType type, HeapID heapID);
static void SpriteResourceCollection_InitRes(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, SpriteResourceType type, HeapID heapID, BOOL allocAtEnd);
static void SpriteResource_UnpackData(SpriteResource* spriteRes, SpriteResourceType type, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID);
static TileResourceData* SpriteUtil_UnpackTileResource(void* rawData, NNS_G2D_VRAM_TYPE vramType, HeapID heapID);
static PaletteResourceData* SpriteUtil_UnpackPaletteResource(void* rawData, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID);
static SpriteResourceData* SpriteUtil_UnpackSpriteResource(void* rawData, HeapID heapID);
static SpriteAnimResourceData* SpriteUtil_UnpackSpriteAnimResource(void* rawData, HeapID heapID);
static MultiSpriteResourceData* SpriteUtil_UnpackMultiSpriteResource(void* rawData, HeapID heapID);
static MultiSpriteAnimResourceData* SpriteUtil_UnpackMultiSpriteAnimResource(void* rawData, HeapID heapID);
static void* SpriteResource_GetData(const SpriteResource* spriteRes);
static void SpriteResource_FreeData(SpriteResource* spriteRes);
static int SpriteResourceTableEntryNARC_GetEntryCount(const SpriteResourceTableEntryNARC* entries);


SpriteResourceCollection* SpriteResourceCollection_New(int capacity, SpriteResourceType type, HeapID heapID) {
	SpriteResourceCollection* spriteResources = Heap_Alloc(heapID, sizeof(SpriteResourceCollection));
	spriteResources->collection = ResourceCollection_New(capacity, heapID);
	spriteResources->resources = Heap_Alloc(heapID, sizeof(SpriteResource) * capacity);
	
	memset(spriteResources->resources, 0, sizeof(SpriteResource) * capacity);
	
	spriteResources->capacity = capacity;
	spriteResources->count = 0;
	spriteResources->type = type;
	
	return spriteResources;
}


void SpriteResourceCollection_Delete(SpriteResourceCollection* spriteResources) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(spriteResources->collection);
	GF_ASSERT(spriteResources->resources);
	
	SpriteResourceCollection_Clear(spriteResources);
	
	ResourceCollection_Delete(spriteResources->collection);
	spriteResources->collection = NULL;
	
	Heap_Free(spriteResources->resources);
	spriteResources->resources = NULL;
	
	Heap_Free(spriteResources);
	spriteResources = NULL;
}


SpriteResource* SpriteResourceCollection_AddFromTable(SpriteResourceCollection* spriteResources, const SpriteResourceTable* table, int index, HeapID heapID) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(table);
	GF_ASSERT(table->count > index);
	GF_ASSERT(spriteResources->type == table->type);
	
	SpriteResource* spriteRes = SpriteResourceCollection_AllocResource(spriteResources);
	GF_ASSERT(spriteRes);
	
	if (table->loadFromNARC == FALSE) {
		SpriteResourceTableEntryFile* fileEntry = (SpriteResourceTableEntryFile*)table->entries + index;
		GF_ASSERT(SpriteResourceCollection_IsIDUnused(spriteResources, fileEntry->id) == TRUE);
		SpriteResourceCollection_InitResFromFile(
			spriteResources,
			spriteRes,
			fileEntry->filename,
			fileEntry->id,
			fileEntry->vramType,
			fileEntry->paletteIndex,
			table->type,
			heapID);
	} else {
		SpriteResourceTableEntryNARC* narcEntry = (SpriteResourceTableEntryNARC*)table->entries + index;
		GF_ASSERT(SpriteResourceCollection_IsIDUnused(spriteResources, narcEntry->id) == TRUE);
		SpriteResourceCollection_InitRes(
			spriteResources,
			spriteRes,
			narcEntry->narcID,
			narcEntry->memberIndex,
			narcEntry->compressed,
			narcEntry->id,
			narcEntry->vramType,
			narcEntry->paletteIndex,
			table->type,
			heapID,
			FALSE);
	}
	
	spriteResources->count++;
	return spriteRes;
}


SpriteResource* SpriteResourceCollection_AddTiles(SpriteResourceCollection* spriteResources, NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, HeapID heapID) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(spriteResources->type == SPRITE_RESOURCE_CHAR);
	
	SpriteResource* spriteRes = SpriteResourceCollection_AllocResource(spriteResources);
	GF_ASSERT(spriteRes);
	
	SpriteResourceCollection_InitRes(
		spriteResources,
		spriteRes,
		narcID,
		memberIdx,
		compressed,
		id,
		vramType,
		0,
		SPRITE_RESOURCE_CHAR,
		heapID,
		FALSE);
	
	spriteResources->count++;
	return spriteRes;
}


SpriteResource* SpriteResourceCollection_AddPalette(SpriteResourceCollection* spriteResources, NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(spriteResources->type == SPRITE_RESOURCE_PLTT);
	
	SpriteResource* spriteRes = SpriteResourceCollection_AllocResource(spriteResources);
	GF_ASSERT(spriteRes);
	
	SpriteResourceCollection_InitRes(
		spriteResources,
		spriteRes,
		narcID,
		memberIdx,
		compressed,
		id,
		vramType,
		paletteIdx,
		SPRITE_RESOURCE_PLTT,
		heapID,
		FALSE);
	
	spriteResources->count++;
	return spriteRes;
}


SpriteResource* SpriteResourceCollection_Add(SpriteResourceCollection* spriteResources, NarcID narcID, int memberIdx, BOOL compressed, int id, SpriteResourceType type, HeapID heapID) {
	GF_ASSERT(spriteResources);
	
	SpriteResource* spriteRes = SpriteResourceCollection_AllocResource(spriteResources);
	GF_ASSERT(spriteRes);
	
	SpriteResourceCollection_InitRes(spriteResources, spriteRes, narcID, memberIdx, compressed, id, 0, 0, type, heapID, FALSE);
	
	spriteResources->count++;
	return spriteRes;
}


void SpriteResourceCollection_ModifyTiles(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NarcID narcID, int memberIdx, BOOL compressed, HeapID heapID) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(spriteResources->type == SPRITE_RESOURCE_CHAR);
	GF_ASSERT(spriteRes);
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_CHAR);
	
	int id = SpriteResource_GetID(spriteRes);
	NNS_G2D_VRAM_TYPE vramType = SpriteResource_GetVRAMType(spriteRes);
	
	SpriteResourceCollection_Remove(spriteResources, spriteRes);
	SpriteResourceCollection_InitRes(spriteResources, spriteRes, narcID, memberIdx, compressed, id, vramType, 0, SPRITE_RESOURCE_CHAR, heapID, FALSE);
}


void SpriteResourceCollection_ModifyPalette(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NarcID narcID, int memberIdx, BOOL compressed, HeapID heapID) {

	GF_ASSERT(spriteResources);
	GF_ASSERT(spriteResources->type == SPRITE_RESOURCE_PLTT);
	GF_ASSERT(spriteRes);
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_PLTT);
	
	int id = SpriteResource_GetID(spriteRes);
	NNS_G2D_VRAM_TYPE vramType = SpriteResource_GetVRAMType(spriteRes);
	int paletteIdx = SpriteResource_GetPaletteIndex(spriteRes);
	
	SpriteResourceCollection_Remove(spriteResources, spriteRes);
	SpriteResourceCollection_InitRes(
		spriteResources,
		spriteRes,
		narcID,
		memberIdx,
		compressed,
		id,
		vramType,
		paletteIdx,
		SPRITE_RESOURCE_PLTT,
		heapID,
		FALSE);
}


int SpriteResourceCollection_Extend(SpriteResourceCollection* spriteResources, const SpriteResourceTable* table, SpriteResourceList* outList, HeapID heapID) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(table);
	
	SpriteResourceCollection_ExtendEx(spriteResources, table, 0, table->count, outList, heapID);
	return table->count;
}


void SpriteResourceCollection_ExtendEx(SpriteResourceCollection* spriteResources, const SpriteResourceTable* table, int first, int count, SpriteResourceList* outList, HeapID heapID) {
	for (int i = first; i < first + count; i++) {
		SpriteResource* spriteRes = SpriteResourceCollection_AddFromTable(spriteResources, table, i, heapID);
		
		if (outList != NULL && outList->capacity > outList->count) {
			outList->resources[outList->count] = spriteRes;
			outList->count++;
		}
	}
}


SpriteResourceList* SpriteResourceList_New(int capacity, HeapID heapID) {
	SpriteResourceList* list = Heap_Alloc(heapID, sizeof(SpriteResourceList));
	
	list->resources = Heap_Alloc(heapID, sizeof(SpriteResource*) * capacity);
	list->capacity = capacity;
	list->count = 0;
	
	return list;
}


void SpriteResourceList_Delete(SpriteResourceList* list) {
	Heap_Free(list->resources);
	Heap_Free(list);
	list = NULL;
}


BOOL SpriteResourceCollection_IsIDUnused(const SpriteResourceCollection* spriteResources, int id) {
	GF_ASSERT(spriteResources);
	return ResourceCollection_IsIDUnused(spriteResources->collection, id);
}


void SpriteResourceCollection_Remove(SpriteResourceCollection* spriteResources, SpriteResource* resource) {
	GF_ASSERT(spriteResources);
	GF_ASSERT(spriteResources->resources);
	
	SpriteResource_FreeData(resource);
	ResourceCollection_Remove(spriteResources->collection, resource->rawResource);
	
	resource->rawResource = NULL;
	spriteResources->count--;
}


void SpriteResourceCollection_Clear(SpriteResourceCollection* spriteResources) {
	for (int i = 0; i < spriteResources->capacity; i++) {
		if (spriteResources->resources[i].rawResource != NULL) {
			SpriteResourceCollection_Remove(spriteResources, spriteResources->resources + i);
		}
	}
}


SpriteResource* SpriteResourceCollection_Find(const SpriteResourceCollection* spriteResources, int id) {
	GF_ASSERT(spriteResources);
	
	for (int i = 0; i < spriteResources->capacity; i++) {
		if (spriteResources->resources[i].rawResource) {
			// Explicit assignment is needed to match
			int resID = Resource_GetID(spriteResources->resources[i].rawResource);
			if (resID == id) {
				return spriteResources->resources + i;
			}
		}
	}
	
	return NULL;
}


int SpriteResource_GetID(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes);
	return Resource_GetID(spriteRes->rawResource);
}


NNSG2dCharacterData* SpriteResource_GetTileData(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_CHAR);
	
	TileResourceData* tileData = SpriteResource_GetData(spriteRes);
	return tileData->tileData;
}


NNSG2dPaletteData* SpriteResource_GetPaletteFade(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_PLTT);
	
	PaletteResourceData* paletteData = SpriteResource_GetData(spriteRes);
	return paletteData->paletteData;
}


NNSG2dCellDataBank* SpriteResource_GetSpriteData(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_CELL);
	
	SpriteResourceData* spriteData = SpriteResource_GetData(spriteRes);
	return spriteData->spriteBank;
}


NNSG2dCellAnimBankData* SpriteResource_GetSpriteAnimData(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_ANIM);
	
	SpriteAnimResourceData* spriteAnimData = SpriteResource_GetData(spriteRes);
	return spriteAnimData->animBank;
}


NNSG2dMultiCellDataBank* SpriteResource_GetMultiSpriteData(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_MULTI_CELL);
	
	MultiSpriteResourceData* multiSpriteData = SpriteResource_GetData(spriteRes);
	return multiSpriteData->multiSpriteBank;
}


NNSG2dMultiCellAnimBankData* SpriteResource_GetMultiSpriteAnimData(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes->type == SPRITE_RESOURCE_MULTI_ANIM);
	
	MultiSpriteAnimResourceData* multiSpriteAnimData = SpriteResource_GetData(spriteRes);
	return multiSpriteAnimData->multiSpriteAnimBank;
}


SpriteResourceType SpriteResource_GetType(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes);
	return spriteRes->type;
}


NNS_G2D_VRAM_TYPE SpriteResource_GetVRAMType(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes);
	
	if (spriteRes->type == SPRITE_RESOURCE_CHAR) {
		TileResourceData* tileData = SpriteResource_GetData(spriteRes);
		return tileData->vramType;
	}
	
	if (spriteRes->type == SPRITE_RESOURCE_PLTT) {
		PaletteResourceData* paletteData = SpriteResource_GetData(spriteRes);
		return paletteData->vramType;
	}
	
	return SPRITE_VRAM_TYPE_DEFAULT;
}


int SpriteResource_GetPaletteIndex(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes);
	
	if (spriteRes->type == SPRITE_RESOURCE_PLTT) {
		PaletteResourceData* paletteData = SpriteResource_GetData(spriteRes);
		return paletteData->paletteIndex;
	}
	
	return 0;
}


int SpriteResourceTable_GetSize(void) {
	return sizeof(SpriteResourceTable);
}


SpriteResourceTable* SpriteResourceTable_GetArrayElement(SpriteResourceTable* table, int index) {
	return table + index;
}


void SpriteResourceTable_LoadFromBinary(const void* data, SpriteResourceTable* table, HeapID heapID) {
	GF_ASSERT(table);
	
	const SpriteResourceTableBinary* tableBin = data;
	table->type = tableBin->type;
	table->loadFromNARC = TRUE;
	table->count = SpriteResourceTableEntryNARC_GetEntryCount(tableBin->narcEntries);
	
	if (table->count > 0) {
		table->entries = Heap_Alloc(heapID, sizeof(SpriteResourceTableEntryNARC) * table->count);
	} else {
		table->entries = NULL;
	}
	
	if (table->entries) {
		memcpy(table->entries, tableBin->narcEntries, sizeof(SpriteResourceTableEntryNARC) * table->count);
	}
}


void SpriteResourceTable_Clear(SpriteResourceTable* table) {
	if (table->entries) {
		Heap_Free(table->entries);
	}
	
	table->entries = NULL;
	table->count = 0;
}


int SpriteResourceTable_GetCount(const SpriteResourceTable* table) {
	GF_ASSERT(table);
	return table->count;
}


static SpriteResource* SpriteResourceCollection_AllocResource(SpriteResourceCollection* spriteResources) {
	for (int i = 0; i < spriteResources->capacity; i++) {
		if (spriteResources->resources[i].rawResource == NULL) {
			return spriteResources->resources + i;
		}
	}
	
	return NULL;
}


static void SpriteResource_UnpackData(SpriteResource* spriteRes, SpriteResourceType type, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID) {
	void* rawData = Resource_GetData(spriteRes->rawResource);
	
	switch (type) {
		case SPRITE_RESOURCE_CHAR:
			spriteRes->data = SpriteUtil_UnpackTileResource(rawData, vramType, heapID);
			break;
		case SPRITE_RESOURCE_PLTT:
			spriteRes->data = SpriteUtil_UnpackPaletteResource(rawData, vramType, paletteIdx, heapID);
			break;
		case SPRITE_RESOURCE_CELL:
			spriteRes->data = SpriteUtil_UnpackSpriteResource(rawData, heapID);
			break;
		case SPRITE_RESOURCE_ANIM:
			spriteRes->data = SpriteUtil_UnpackSpriteAnimResource(rawData, heapID);
			break;
		case SPRITE_RESOURCE_MULTI_CELL:
			spriteRes->data = SpriteUtil_UnpackMultiSpriteResource(rawData, heapID);
			break;
		case SPRITE_RESOURCE_MULTI_ANIM:
			spriteRes->data = SpriteUtil_UnpackMultiSpriteAnimResource(rawData, heapID);
			break;
	}
}


static TileResourceData* SpriteUtil_UnpackTileResource(void* rawData, NNS_G2D_VRAM_TYPE vramType, HeapID heapID) {
	TileResourceData* tileData = Heap_Alloc(heapID, sizeof(TileResourceData));
	NNS_G2dGetUnpackedCharacterData(rawData, &tileData->tileData);
	tileData->vramType = vramType;
	
	return tileData;
}


static PaletteResourceData* SpriteUtil_UnpackPaletteResource(void* rawData, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID) {
	PaletteResourceData* paletteData = Heap_Alloc(heapID, sizeof(PaletteResourceData));
	NNS_G2dGetUnpackedPaletteData(rawData, &paletteData->paletteData);
	
	paletteData->vramType = vramType;
	paletteData->paletteIndex = paletteIdx;
	
	return paletteData;
}


static SpriteResourceData* SpriteUtil_UnpackSpriteResource(void* rawData, HeapID heapID) {
	SpriteResourceData* spriteResData = Heap_Alloc(heapID, sizeof(SpriteResourceData));
	NNS_G2dGetUnpackedCellBank(rawData, &spriteResData->spriteBank);
	
	return spriteResData;
}


static SpriteAnimResourceData* SpriteUtil_UnpackSpriteAnimResource(void* rawData, HeapID heapID) {
	SpriteAnimResourceData* spriteAnimData = Heap_Alloc(heapID, sizeof(SpriteAnimResourceData));
	NNS_G2dGetUnpackedAnimBank(rawData, &spriteAnimData->animBank);
	
	return spriteAnimData;
}


static MultiSpriteResourceData* SpriteUtil_UnpackMultiSpriteResource(void* rawData, HeapID heapID) {
	MultiSpriteResourceData* multiSpriteData = Heap_Alloc(heapID, sizeof(MultiSpriteResourceData));
	NNS_G2dGetUnpackedMultiCellBank(rawData, &multiSpriteData->multiSpriteBank);
	
	return multiSpriteData;
}


static MultiSpriteAnimResourceData* SpriteUtil_UnpackMultiSpriteAnimResource(void* rawData, HeapID heapID) {
	MultiSpriteAnimResourceData* multiSpriteAnimData = Heap_Alloc(heapID, sizeof(MultiSpriteAnimResourceData));
	NNS_G2dGetUnpackedMCAnimBank(rawData, &multiSpriteAnimData->multiSpriteAnimBank);
	
	return multiSpriteAnimData;
}


static void SpriteResource_FreeData(SpriteResource* spriteRes) {
	if (spriteRes->data) {
		Heap_Free(spriteRes->data);
	}
	
	spriteRes->data = NULL;
}


static void* SpriteResource_GetData(const SpriteResource* spriteRes) {
	GF_ASSERT(spriteRes);
	GF_ASSERT(spriteRes->data);
	
	return spriteRes->data;
}


static void SpriteResourceCollection_InitResFromFile(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, const char* filename, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, SpriteResourceType type, HeapID heapID) {
	spriteRes->rawResource = ResourceCollection_AddFromFile(spriteResources->collection, filename, id, heapID);
	spriteRes->type = type;
	
	SpriteResource_UnpackData(spriteRes, type, vramType, paletteIdx, heapID);
}


static void SpriteResourceCollection_InitRes(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, SpriteResourceType type, HeapID heapID, BOOL allocAtEnd) {
	void* data = LoadMemberFromNARC(narcID, memberIdx, compressed, heapID, allocAtEnd);
	
	spriteRes->rawResource = ResourceCollection_Add(spriteResources->collection, data, id);
	spriteRes->type = type;
	
	SpriteResource_UnpackData(spriteRes, type, vramType, paletteIdx, heapID);
}


static int SpriteResourceTableEntryNARC_GetEntryCount(const SpriteResourceTableEntryNARC* entries) {
	int i = 0;
	while (entries[i].narcID != NARC_INDEX_NONE) {
		i++;
	}
	
	return i;
}
