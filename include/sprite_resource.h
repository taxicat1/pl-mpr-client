#ifndef MPR_SPRITE_RESOURCE_H
#define MPR_SPRITE_RESOURCE_H

#include <nnsys.h>

#include "constants/heap.h"

#include "narc.h"
#include "resource_collection.h"

typedef enum {
	SPRITE_RESOURCE_CHAR = 0,
	SPRITE_RESOURCE_PLTT,
	SPRITE_RESOURCE_CELL,
	SPRITE_RESOURCE_ANIM,
	SPRITE_RESOURCE_MULTI_CELL,
	SPRITE_RESOURCE_MULTI_ANIM,
	SPRITE_RESOURCE_MAX
} SpriteResourceType;

#define MAX_SPRITE_RESOURCE_GEN4 (SPRITE_RESOURCE_ANIM + 1) // MULTI_CELL and MULTI_ANIM are not used in the default rom

typedef struct {
	Resource*           rawResource;
	SpriteResourceType  type;
	void*               data;
} SpriteResource;

typedef struct SpriteResourceCollection {
	ResourceCollection* collection;
	SpriteResource* resources;
	int capacity;
	int count;
	SpriteResourceType type;
} SpriteResourceCollection;

typedef struct {
	int                id;
	char               filename[64];
	NNS_G2D_VRAM_TYPE  vramType;
	int                paletteIndex;
} SpriteResourceTableEntryFile;

typedef struct {
	NarcID             narcID;
	int                memberIndex;
	BOOL               compressed;
	int                id;
	NNS_G2D_VRAM_TYPE  vramType;
	int                paletteIndex;
} SpriteResourceTableEntryNARC;

typedef struct {
	void*               entries;
	int                 count;
	SpriteResourceType  type;
	u8                  loadFromNARC; // Whether the entries are SpriteResourceTableEntryNARC or SpriteResourceTableEntryFile
} SpriteResourceTable;

// Fixed size, non-owning list of SpriteResource pointers
typedef struct {
	SpriteResource** resources;
	int capacity;
	int count;
} SpriteResourceList;

SpriteResourceCollection* SpriteResourceCollection_New(int capacity, SpriteResourceType type, HeapID heapID);
void SpriteResourceCollection_Delete(SpriteResourceCollection* spriteResources);
SpriteResource* SpriteResourceCollection_AddFromTable(SpriteResourceCollection* spriteResources, const SpriteResourceTable* table, int index, HeapID heapID);
SpriteResource* SpriteResourceCollection_AddTiles(SpriteResourceCollection* spriteResources, NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, HeapID heapID);
SpriteResource* SpriteResourceCollection_AddPalette(SpriteResourceCollection* spriteResources, NarcID narcID, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID);
SpriteResource* SpriteResourceCollection_Add(SpriteResourceCollection* spriteResources, NarcID narcID, int memberIdx, BOOL compressed, int id, SpriteResourceType type, HeapID heapID);
void SpriteResourceCollection_ModifyTiles(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NarcID narcID, int memberIdx, BOOL compressed, HeapID heapID);
void SpriteResourceCollection_ModifyPalette(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NarcID narcID, int memberIdx, BOOL compressed, HeapID heapID);
SpriteResource* SpriteResourceCollection_AddTilesFrom(SpriteResourceCollection* spriteResources, NARC* narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, HeapID heapID);
SpriteResource* SpriteResourceCollection_AddTilesFromEx(SpriteResourceCollection* spriteResources, NARC* narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, HeapID heapID, BOOL allocAtEnd);
SpriteResource* SpriteResourceCollection_AddPaletteFrom(SpriteResourceCollection* spriteResources, NARC* narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID);
SpriteResource* SpriteResourceCollection_AddPaletteFromEx(SpriteResourceCollection* spriteResources, NARC* narc, int memberIdx, BOOL compressed, int id, NNS_G2D_VRAM_TYPE vramType, int paletteIdx, HeapID heapID, BOOL allocAtEnd);
SpriteResource* SpriteResourceCollection_AddFrom(SpriteResourceCollection* spriteResources, NARC* narc, int memberIdx, BOOL compressed, int id, SpriteResourceType type, HeapID heapID);
void SpriteResourceCollection_ModifyTilesFrom(SpriteResourceCollection* spriteResources, SpriteResource* spriteRes, NARC* narc, int memberIdx, BOOL compressed, HeapID heapID);
int SpriteResourceCollection_Extend(SpriteResourceCollection* spriteResources, const SpriteResourceTable* table, SpriteResourceList* outList, HeapID heapID);
void SpriteResourceCollection_ExtendEx(SpriteResourceCollection* spriteResources, const SpriteResourceTable* table, int first, int count, SpriteResourceList* outList, HeapID heapID);
SpriteResourceList* SpriteResourceList_New(int capacity, HeapID heapID);
void SpriteResourceList_Delete(SpriteResourceList* list);
BOOL SpriteResourceCollection_IsIDUnused(const SpriteResourceCollection* spriteResources, int id);
void SpriteResourceCollection_Remove(SpriteResourceCollection* spriteResources, SpriteResource* resource);
void SpriteResourceCollection_Clear(SpriteResourceCollection* spriteResources);
SpriteResource* SpriteResourceCollection_Find(const SpriteResourceCollection* spriteResources, int id);
int SpriteResource_GetID(const SpriteResource* spriteRes);
NNSG2dCharacterData* SpriteResource_GetTileData(const SpriteResource* spriteRes);
NNSG2dPaletteData* SpriteResource_GetPaletteFade(const SpriteResource* spriteRes);
NNSG2dCellDataBank* SpriteResource_GetSpriteData(const SpriteResource* spriteRes);
NNSG2dCellAnimBankData* SpriteResource_GetSpriteAnimData(const SpriteResource* spriteRes);
NNSG2dMultiCellDataBank* SpriteResource_GetMultiSpriteData(const SpriteResource* spriteRes);
NNSG2dMultiCellAnimBankData* SpriteResource_GetMultiSpriteAnimData(const SpriteResource* spriteRes);
SpriteResourceType SpriteResource_GetType(const SpriteResource* spriteRes);
NNS_G2D_VRAM_TYPE SpriteResource_GetVRAMType(const SpriteResource* spriteRes);
int SpriteResource_GetPaletteIndex(const SpriteResource* spriteRes);
int SpriteResourceTable_GetSize(void);
SpriteResourceTable* SpriteResourceTable_GetArrayElement(SpriteResourceTable* table, int index);
void SpriteResourceTable_LoadFromBinary(const void* data, SpriteResourceTable* table, HeapID heapID);
void SpriteResourceTable_Clear(SpriteResourceTable* table);
int SpriteResourceTable_GetCount(const SpriteResourceTable* table);

#endif /* MPR_SPRITE_RESOURCE_H */
