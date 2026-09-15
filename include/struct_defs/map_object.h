#ifndef MPR_MAP_OBJECT_H
#define MPR_MAP_OBJECT_H

#include <nitro/fx/fx.h>

typedef struct {
	u32   status;
	u32   unk_04;
	u8    localID;
	u8    movementType;
	s8    movementRangeX;
	s8    movementRangeZ;
	s8    initialDir;
	s8    facingDir;
	s8    movingDir;
	u8    padding_0F;
	u16   mapID;
	u16   graphicsID;
	u16   trainerType;
	u16   flag;
	u16   script;
	s16   unk_1A;
	s16   unk_1C;
	s16   unk_1E;
	s16   xInitial;
	s16   yInitial;
	s16   zInitial;
	s16   x;
	s16   y;
	s16   z;
	fx32  unk_2C;
	u8    unk_30[16];
	u8    unk_40[16];
} MapObjectSave;

#endif /* MPR_MAP_OBJECT_H */
