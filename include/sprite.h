#ifndef MPR_SPRITE_H
#define MPR_SPRITE_H

#include <nitro/fx/fx.h>
#include <nitro/gx.h>
#include <nnsys.h>

#include "constants/graphics.h"
#include "constants/heap.h"

typedef enum {
	CELL_ANIM_TYPE_NONE = 0,
	CELL_ANIM_TYPE_CELL,
	CELL_ANIM_TYPE_MULTI_CELL,
	CELL_ANIM_TYPE_VRAM_CELL
} CellAnimType;

typedef enum {
	AFFINE_OVERWRITE_MODE_NONE = 0,
	AFFINE_OVERWRITE_MODE_NORMAL,
	AFFINE_OVERWRITE_MODE_DOUBLE // Extends the cell sprite's drawable area by 2x, use when the cell sprite is scaled up
} AffineOverwriteMode;

typedef struct SpriteList SpriteList;

typedef struct {
	const NNSG2dCellDataBank*      cellBank;
	const NNSG2dCellAnimBankData*  animBank;
	NNSG2dCellAnimation            anim;
} CellAnimationData;

typedef struct {
	NNSG2dCellDataBank*            cellBank;
	const NNSG2dCellAnimBankData*  animBank;
	NNSG2dCellAnimation            anim;
	u32                            transferHandle;
} VRamCellAnimationData;

typedef struct {
	const NNSG2dCellDataBank*           individualCellBank;
	const NNSG2dCellAnimBankData*       individualAnimBank;
	NNSG2dMultiCellAnimation            anim;
	const NNSG2dMultiCellDataBank*      cellBank;
	const NNSG2dMultiCellAnimBankData*  animBank;
	NNSG2dNode*                         nodes;
	NNSG2dCellAnimation*                cellAnims;
} MultiCellAnimationData;

typedef struct Sprite {
	VecFx32      position;
	VecFx32      affineTranslation;
	VecFx32      affineScale;
	u16          affineZRotation;
	u8           affineOverwriteMode;
	u8           flip;
	u8           overwriteFlags; // Specifies which of the 'explicit' fields are used. Overwrites data provided by the OAM.
	u8           explicitPalette; // An explicit palette index.
	u8           explicitPaletteOffset; // An explicit palette index offset added onto the index specified by the OAM.
	BOOL         explicitMosaic;
	GXOamMode    explicitOamMode;
	u8           draw;
	u8           animate;
	fx32         animSpeed;
	SpriteList*  list; // The collection this sprite belongs to

	// This field is supposed to be a union between CellAnimationData, VRamCellAnimationData, and MultiCellAnimationData
	// but it's actually too small to hold the largest of these types. This should really be u32 animData[31].
	u32                      animData[SPRITE_ANIM_SIZE];
	NNSG2dImageProxy         imageProxy;
	NNSG2dImagePaletteProxy  paletteProxy;
	u32                      type;
	u16                      activeAnimID;
	u8                       explicitPriority;
	u16                      priority;
	NNS_G2D_VRAM_TYPE        vramType;
	struct Sprite*           prev;
	struct Sprite*           next;
} Sprite;

struct SpriteList {
	Sprite*                  sprites;
	int                      capacity;
	Sprite**                 freeSprites; // Stack of currently unused sprites
	int                      freeSpriteHead;
	Sprite                   sentinelData;
	NNSG2dRendererInstance*  renderer;
	void*                    rawAnimData;
	NNSG2dCellAnimBankData*  defaultAnimBank;
	BOOL active;
};

typedef struct {
	int                      maxElements;
	NNSG2dRendererInstance*  renderer;
	HeapID                   heapID;
} SpriteListParams;

typedef struct {
	const NNSG2dImageProxy*             imageProxy;
	const NNSG2dCharacterData*          charData;
	const NNSG2dImagePaletteProxy*      paletteProxy;
	NNSG2dCellDataBank*                 cellBank;
	const NNSG2dCellAnimBankData*       cellAnimBank;
	const NNSG2dMultiCellDataBank*      multiCellBank;
	const NNSG2dMultiCellAnimBankData*  multiCellAnimBank;
	BOOL                                isVRamTransfer;
	u8                                  priority;
	u8                                  padding_21[3];
} SpriteResourcesHeader;

typedef struct {
	SpriteResourcesHeader*  headers;
	int                     length;
} SpriteResourcesHeaderList;

typedef struct {
	Sprite*                     sprite;
	SpriteResourcesHeader*      resourceHeader;
	SpriteResourcesHeaderList*  resourceHeaderList;
	BOOL                        vramTransfer;
} ManagedSprite;

typedef struct {
	SpriteList*                   list;
	const SpriteResourcesHeader*  resourceData;
	VecFx32                       position;
	u32                           priority;
	NNS_G2D_VRAM_TYPE             vramType;
	HeapID                        heapID;
} SpriteListTemplate;

typedef struct {
	SpriteList*                   list;
	const SpriteResourcesHeader*  resourceData;
	VecFx32                       position;
	VecFx32                       affineScale;
	u16                           affineZRotation;
	u32                           priority;
	NNS_G2D_VRAM_TYPE             vramType;
	HeapID                        heapID;
} AffineSpriteListTemplate;

SpriteList* SpriteList_New(const SpriteListParams* params);
BOOL SpriteList_Delete(SpriteList* list);
BOOL SpriteList_DeleteAll(SpriteList* list);
void SpriteList_Update(const SpriteList* list);
void Sprite_Reset(Sprite* sprite);
Sprite* SpriteList_AddAffine(const AffineSpriteListTemplate* spriteTemplate);
Sprite* SpriteList_Add(const SpriteListTemplate* spriteTemplate);
void Sprite_Delete(Sprite* sprite);
void Sprite_SetPosition(Sprite* sprite, const VecFx32* position);
void Sprite_SetAffineScale(Sprite* sprite, const VecFx32* scale);
void Sprite_SetAffineZRotation(Sprite* sprite, u16 angle);
void Sprite_SetDrawFlag(Sprite* sprite, BOOL draw);
void Sprite_SetAnimateFlag(Sprite* sprite, BOOL animate);
void Sprite_SetAnimSpeed(Sprite* sprite, fx32 speed);
void Sprite_SetAffineOverwriteMode(Sprite* sprite, AffineOverwriteMode mode);
void Sprite_SetFlipMode(Sprite* sprite, u32 mode);
const VecFx32* Sprite_GetPosition(const Sprite* sprite);
BOOL Sprite_GetDrawFlag(const Sprite* sprite);
void Sprite_SetAnim(Sprite* sprite, u32 animID);
u32 Sprite_GetActiveAnim(const Sprite* sprite);
void Sprite_UpdateAnim(Sprite* sprite, fx32 frames);
void Sprite_SetAnimFrame(Sprite* sprite, u16 frame);
u16 Sprite_GetAnimFrame(const Sprite* sprite);
void Sprite_SetExplicitPriority(Sprite* sprite, u8 priority);
u8 Sprite_GetExplicitPriority(const Sprite* sprite);
void Sprite_SetExplicitPalette(Sprite* sprite, u32 palette);
void Sprite_SetPriority(Sprite* sprite, u32 priority);
u32 Sprite_GetPriority(const Sprite* sprite);
void Sprite_SetImageProxy(Sprite* sprite, const NNSG2dImageProxy* imageProxy);
NNS_G2D_VRAM_TYPE Sprite_GetVRamType(const Sprite* sprite);
BOOL Sprite_IsAnimated(Sprite* sprite);
void Sprite_SetExplicitOAMMode(Sprite* sprite, GXOamMode mode);
void Utility_Clear2DMainOAM(HeapID heapID);
void Utility_Clear2DSubOAM(HeapID heapID);

#endif /* MPR_SPRITE_H */
