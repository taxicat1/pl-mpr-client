#ifndef MPR_SPRITE_UTIL_H
#define MPR_SPRITE_UTIL_H

#include <nitro/gx.h>

#include "sprite.h"
#include "sprite_resource.h"

#define RESOURCE_NONE -1

typedef struct {
	u32  charResourceID;
	u32  plttResourceID;
	u32  cellResourceID;
	u32  animResourceID;
	u32  mcellResourceID;
	u32  manimResourceID;
	u32  vramTransfer;
	u32  priority;
} ResdatTableEntry;

typedef struct {
	NNSG2dRendererInstance  renderer;
	NNSG2dRenderSurface     mainScreen;
	NNSG2dRenderSurface     subScreen;
} G2dRenderer;

void SpriteResourcesHeader_Init(
	SpriteResourcesHeader* resourceHeader,
	int charResourceID,
	int plttResourceID,
	int cellResourceID,
	int animResourceID,
	int mcellResourceID,
	int manimResourceID,
	BOOL vramTransfer,
	int priority,
	SpriteResourceCollection* charResources,
	SpriteResourceCollection* plttResources,
	SpriteResourceCollection* cellResources,
	SpriteResourceCollection* animResources,
	SpriteResourceCollection* mcellResources,
	SpriteResourceCollection* manimResources);
void SpriteResourcesHeader_Clear(SpriteResourcesHeader* resourceHeader);
SpriteResourcesHeaderList* SpriteResourcesHeaderList_NewFromResdat(
	const ResdatTableEntry* resdatEntries,
	HeapID heapID,
	SpriteResourceCollection* charResources,
	SpriteResourceCollection* plttResources,
	SpriteResourceCollection* cellResources,
	SpriteResourceCollection* animResources,
	SpriteResourceCollection* mcellResources,
	SpriteResourceCollection* manimResources);
void SpriteResourcesHeaderList_Free(SpriteResourcesHeaderList* headerList);
SpriteList* SpriteList_InitRendering(int maxElements, G2dRenderer* g2dRenderer, HeapID heapID);
void SetSubScreenViewRect(G2dRenderer* g2dRenderer, fx32 x, fx32 y);

#endif /* MPR_SPRITE_UTIL_H */
