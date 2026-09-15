#ifndef MPR_POKEMON_SPRITE_H
#define MPR_POKEMON_SPRITE_H

#include <nnsys.h>

#include "constants/heap.h"
#include "constants/narc.h"
#include "constants/pokemon_sprite_attributes.h"
#include "struct_defs/sprite_animation_frame.h"

#define MAX_MON_SPRITES 4

typedef struct PokemonSprite PokemonSprite;
typedef struct PokemonSpriteTransforms PokemonSpriteTransforms;

typedef void(PokemonSpriteCallback)(PokemonSprite*, PokemonSpriteTransforms*);

typedef struct {
	u16  narcID;       //< ID of the sprite archive
	u16  character;    //< File index to pull from the archive for the character data (tiles)
	u16  palette;      //< File index to pull from the archive for the palette data
	u16  spindaSpots;  //< Simple flag denoting whether the rendered sprite should be pseudo-randomized with splotches (only for Spinda's front-sprite).
	u8   dummy;        //< Dummy value; never used or set to anything other than 0.
	u32  personality;  //< Cached personality value for Pokemon front-sprites. Specifically used for Spinda spots.
} PokemonSpriteTemplate;

struct PokemonSpriteTransforms {
	s16  xCenter;
	s16  yCenter;
	int  zCenter;
	s16  xOffset;
	s16  yOffset;
	int  zOffset;
	s16  scaleX; // Negative values flip the sprite.
	s16  scaleY;
	u16  rotationX;
	u16  rotationY;
	u16  rotationZ;
	u16  padding_1A;
	s16  xPivot;
	s16  yPivot;
	u8   drawXOffset; // Offset from the sprite origin to begin drawing in partial mode.
	u8   drawYOffset;
	u8   drawWidth; // Determines how much of the sprite actually gets drawn in partial mode.
	u8   drawHeight;
	u8   fadeInitAlpha;
	u8   fadeTargetAlpha;
	u8   fadeDelayCounter;
	u8   fadeDelayLength;
	u32  fadeTargetColor;
	u32  diffuseR : 5;
	u32  diffuseG : 5;
	u32  diffuseB : 5;
	u32  ambientR : 5;
	u32  ambientG : 5;
	u32  ambientB : 5;
	u32  padding_2C_30 : 2;
	u32  hide : 1;
	u32  partialDraw : 1; // When enabled, only draws a part of the sprite. Disables shadows and affine scaling when in use.
	u32  alpha : 5;
	u32  padding_30_07 : 2;
	u32  flipH : 1;
	u32  flipV : 1;
	u32  hide2 : 1; // It's unclear why there are 2 members that do the same thing.
	u32  fadeActive : 1;
	u32  mosaicIntensity : 4;
	u32  padding_30_17 : 15;
};

typedef struct {
	u16  plttSlot      : 2;
	u16  shouldFollowX : 1;
	u16  shouldFollowY : 1;
	u16  isAffine      : 1;
	u16  size          : 2;
	u16                : 9;
	s8   height;
	u8   padding_03;
	s16  x;
	s16  y;
	s16  xOffset;
	s16  yOffset;
} PokemonSpriteShadow;

struct PokemonSprite {
	u32  active         : 1;
	u32  polygonID      : 6;
	u32  needReloadChar : 1;
	u32  needReloadPltt : 1;
	u32                 : 23;
	
	PokemonSpriteTemplate    spriteTemplate;
	PokemonSpriteTemplate    spriteTemplateBackup;
	PokemonSpriteTransforms  transforms;
	
	u8  animActive;
	u8  currAnimFrame;
	u8  animFrameDelay;
	u8  currSpriteFrame;
	u8  animLoopTimers[MAX_ANIMATION_FRAMES];
	u8  padding_66[2];
	
	PokemonSpriteCallback*  callback;
	PokemonSpriteShadow     shadow;
	PokemonSpriteShadow     shadowBackup;
	SpriteAnimFramePt       animFrames[MAX_ANIMATION_FRAMES];
};

typedef struct {
	PokemonSprite sprites[MAX_MON_SPRITES];
	
	NNSG2dImageProxy         imageProxy;
	NNSG2dImagePaletteProxy  plttProxy;
	
	HeapID  heapID;
	
	u32   charBaseAddr;
	u32   charSize;
	u32   plttBaseAddr;
	u32   plttSize;
	u8*   charRawData;
	u16*  plttRawData;
	u16*  plttRawDataUnfaded;
	
	NNSG2dCharacterData  charData;
	NNSG2dPaletteData    plttData;
	
	u8   dummy330;
	u8   needLoadChar;
	u8   needLoadPltt;
	u8   excludeIdentity;
	u32  hideShadows; // Curiously, this field is treated like a bitmask, but it only ever uses a value of 0 or 1
} PokemonSpriteManager;

// Used to run PokemonSprite animations in a task independent of the PokemonSpriteManager
typedef struct {
	u8  active;
	u8  currSpriteFrame;
	u8  currAnimFrame;
	u8  frameDelay;
	u8  loopTimers[MAX_ANIMATION_FRAMES];
	
	u8 padding_0E[2];
	
	const SpriteAnimFramePt* animFrames;
} PokemonSpriteTaskAnim;

void* PokemonSpriteManager_New(HeapID heapID);
void PokemonSpriteManager_DrawSprites(PokemonSpriteManager* monSpriteMan);
void PokemonSpriteManager_Free(PokemonSpriteManager* monSpriteMan);
void PokemonSprite_InitAnim(PokemonSprite* monSprite, int dummy);
PokemonSprite* PokemonSpriteManager_CreateSprite(PokemonSpriteManager* monSpriteMan, PokemonSpriteTemplate* spriteTemplate, int x, int y, int z, int polygonID, SpriteAnimFramePt* animFrames, PokemonSpriteCallback* callback);
PokemonSprite* PokemonSpriteManager_CreateSpriteAtIndex(PokemonSpriteManager* monSpriteMan, PokemonSpriteTemplate* spriteTemplate, int x, int y, int z, int polygonID, int index, SpriteAnimFramePt* animFrames, PokemonSpriteCallback* callback);
void PokemonSprite_Delete(PokemonSprite* monSprite);
void PokemonSprite_SetAttribute(PokemonSprite* monSprite, PokemonSpriteAttribute attribute, int value);
int PokemonSprite_GetAttribute(PokemonSprite* monSprite, PokemonSpriteAttribute attribute);
void PokemonSprite_AddAttribute(PokemonSprite* monSprite, PokemonSpriteAttribute attribute, int delta);
void PokemonSprite_StartFade(PokemonSprite* monSprite, int initAlpha, int targetAlpha, int delay, int color);
BOOL PokemonSprite_IsFadeActive(PokemonSprite* monSprite);
void PokemonSpriteManager_UpdateCharAndPltt(PokemonSpriteManager* monSpriteMan);
void PokemonSprite_DrawSpindaSpots(u8* rawCharData, u32 personality, BOOL isAnimated);
void PokemonSprite_DecryptPt(u8* rawCharData);
void PokemonSprite_DecryptDP(u8* rawCharData);
void PokemonSprite_Decrypt(u8* rawCharData, NarcID narcID);

#endif /* MPR_POKEMON_SPRITE_H */
