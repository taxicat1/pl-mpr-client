#ifndef MPR_STRUCT_DEFS_SPECIES_SPRITE_DATA_H
#define MPR_STRUCT_DEFS_SPECIES_SPRITE_DATA_H

#include "struct_defs/sprite_animation_frame.h"

#define MAX_FACES  2

typedef struct {
	u8  cryDelay;    //!< How long to wait before playing the cry.
	u8  animation;   //!< Which pre-programmed animation sequence to use.
	u8  startDelay;  //!< How long to wait before starting the animation sequence.
	
	SpriteAnimFramePt frames[MAX_ANIMATION_FRAMES];  //!< Frame-data for each frame of the animation sequence.
} SpeciesSpriteAnim;

typedef struct {
	SpeciesSpriteAnim faceAnims[MAX_FACES];  //!< Data for each display-face of the Pokemon's sprite.
		
	s8  yOffset;        //!< Additional vertical offset of the Pokemon's front-face sprite.
	s8  xOffsetShadow;  //!< Horizontal offset for the shadow sprite beneath the Pokemon's sprite.
	u8  shadowSize;     //!< Size of the shadow sprite beneath the Pokemon's sprite.
} SpeciesSpriteData;

#endif /* MPR_STRUCT_DEFS_SPECIES_SPRITE_DATA_H */
