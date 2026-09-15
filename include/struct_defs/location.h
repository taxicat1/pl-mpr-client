#ifndef MPR_LOCATION_H
#define MPR_LOCATION_H

#define WARP_ID_NONE -1

typedef enum {
	FACE_NONE = -1,
	FACE_UP = 0,
	FACE_DOWN,
	FACE_LEFT,
	FACE_RIGHT
} FaceDirection;

typedef struct {
	int  mapHeaderID;
	int  warpId;
	int  x;
	int  z;
	int  faceDirection;
} Location;

#endif /* MPR_LOCATION_H */
