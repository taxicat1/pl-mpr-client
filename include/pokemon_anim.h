#ifndef MPR_POKEMON_ANIM_H
#define MPR_POKEMON_ANIM_H

#include "pokemon_sprite.h"
#include "sys_task_manager.h"

#define MAX_ANIM_TRANSFORMS      4
#define MAX_TRANSFORM_DATA_VARS  8
#define MAX_POKEMON_ANIM_VARS    8

typedef struct TransformData TransformData;
typedef struct PokemonAnim PokemonAnim;

typedef void (*TransformFunc)(TransformData*, PokemonAnim*);

typedef struct {
	u16  animation;
	u16  startDelay;
	u8   flipSprite;
} PokemonAnimTemplate;

struct TransformData {
	BOOL  active;
	int   vars[MAX_TRANSFORM_DATA_VARS];
	int*  transformMemberPtr;
	int*  animMemberPtr;
	u8    calcType;
	u8    startDelay;
	int   originalValue;
	int   dummy_34;
	int   dummy_38;
	int   offsetX;
	int   offsetY;
	int   scaleX;
	int   scaleY;
	int   rotationZ;
	
	TransformFunc func;
};

struct PokemonAnim {
	PokemonSprite*  sprite;
	SysTask*        task;
	void*           scriptData;
	u32*            scriptPtr;
	BOOL            active;
	int             animNum;
	int             waitFrame;
	int             endAnim;
	BOOL            completed;
	int             vars[MAX_POKEMON_ANIM_VARS];
	int             commandCount;
	int             loopMax;
	int             loopCounter;
	u32*            loopStart;
	int             startDelay;
	int             originalX;
	int             originalY;
	int             translateX;
	int             translateY;
	int             offsetX;
	int             offsetY;
	int             scaleX;
	int             scaleY;
	int             rotationZ;
	TransformData   transforms[MAX_ANIM_TRANSFORMS];
	u8              flipSprite;
	u8              waitForTransform;
	u8              yNormalization;
	u8              fadeActive;
};

typedef struct {
	PokemonAnim*  anims;
	HeapID        heapID;
	u8            flipSprite;
	u8            animCount;
} PokemonAnimManager;

typedef enum {
	POKEANIMSPEED_FAST,
	POKEANIMSPEED_MEDIUM,
	POKEANIMSPEED_SLOW,
	POKEANIMSPEED_MAX
} PokeAnimSpeed;

PokemonAnimManager* PokemonAnimManager_New(HeapID heapID, int animCount, u8 flipSprite);
void PokemonAnimManager_Free(PokemonAnimManager* monAnimMan);
u8 PokemonAnim_GetSpeedFromNature(const u8 nature);
void PokemonAnimManager_InitAnim(PokemonAnimManager* monAnimMan, PokemonSprite* monSprite, const PokemonAnimTemplate* animTemplate, u8 index);
void PokemonAnimManager_DeleteAnim(PokemonAnimManager* monAnimMan, u8 index);

#endif /* MPR_POKEMON_ANIM_H */
