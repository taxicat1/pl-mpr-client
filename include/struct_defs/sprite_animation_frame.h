#ifndef MPR_STRUCT_DEFS_SPRITE_ANIMATION_DATA_H
#define MPR_STRUCT_DEFS_SPRITE_ANIMATION_DATA_H

#define MAX_ANIMATION_FRAMES 10

/**
 * @brief By-frame data definition for animating a sprite.
 */
typedef struct {
	s8  spriteFrame;  //!< Which frame of the sprite to use for this frame of the animation.
	u8  frameDelay;   //!< How long this animation frame should last.
} SpriteAnimFrameDP;

typedef struct {
	s8  spriteFrame;  //!< Which frame of the sprite to use for this frame of the animation.
	u8  frameDelay;   //!< How long this animation frame should last.
	s8  xOffset;      //!< (New with Pt) Horizontal shift to apply to the sprite for this animation frame.
	s8  yOffset;      //!< (New with Pt) Vertical shift to apply to the sprite for this animation frame.
} SpriteAnimFramePt;

// Used for NARC format reading (pokeanm.narc / pl_pokeanim.narc)
typedef struct {
	SpriteAnimFrameDP  animFront;
	SpriteAnimFrameDP  animBack[3];
	SpriteAnimFrameDP  animMain[MAX_ANIMATION_FRAMES];
} SpriteAnimationTableDP;

typedef struct {
	SpriteAnimFramePt  animFront;
	SpriteAnimFramePt  animBack[3];
	SpriteAnimFramePt  animMain[MAX_ANIMATION_FRAMES];
} SpriteAnimationTablePt;

#endif /* MPR_STRUCT_DEFS_SPRITE_ANIMATION_DATA_H */
