#ifndef MPR_CAMERA_H
#define MPR_CAMERA_H

#include <nitro/fx/fx.h>

#include "constants/heap.h"

#define CAMERA_DELAY_NONE  0
#define CAMERA_DELAY_X     1
#define CAMERA_DELAY_Y     2
#define CAMERA_DELAY_Z     4

#define CAMERA_DEFAULT_NEAR_CLIP  (FX32_ONE * 150)
#define CAMERA_DEFAULT_FAR_CLIP   (FX32_ONE * 900)

enum CameraProjection {
    CAMERA_PROJECTION_PERSPECTIVE = 0,
    CAMERA_PROJECTION_ORTHOGRAPHIC,
    CAMERA_PROJECTION_COUNT
};

typedef struct CameraAngle {
    u16  x;
    u16  y;
    u16  z;
    u16  padding_06;
} CameraAngle;

typedef struct CameraLookAt {
    VecFx32  position;
    VecFx32  target;
    VecFx32  up;
} CameraLookAt;

typedef struct CameraPerspectiveProjection {
    fx32  sinFovY;
    fx32  cosFovY;
    fx32  aspectRatio;
    fx32  nearClip;
    fx32  farClip;
} CameraPerspectiveProjection;

typedef struct CameraPositionHistory {
    int       historySize;
    int       currentHistoryIndex;
    int       nextHistoryIndex;
    int       delay;
    BOOL      delayReached;
    BOOL      delayX;
    BOOL      delayY;
    BOOL      delayZ;
    VecFx32*  positions;
} CameraPositionHistory;

typedef struct Camera {
	CameraPerspectiveProjection  perspective;
	CameraLookAt                 lookAt;
	fx32                         distance;
	CameraAngle                  angle;
	u8                           projection;
	u16                          fovY;
	VecFx32                      prevTargetPos;
	const VecFx32*               targetPos;
	BOOL                         trackTargetX;
	BOOL                         trackTargetY;
	BOOL                         trackTargetZ;
	CameraPositionHistory*       history;
} Camera;

extern GXBufferMode gBufferMode;

Camera* Camera_Alloc(const HeapID heapID);
void Camera_Delete(Camera* camera);
void Camera_SetAsActive(Camera* camera);
void Camera_ComputeViewMatrix(void);
void Camera_ReleaseTarget(Camera* camera);
void Camera_SetClipping(const fx32 nearClip, const fx32 farClip, Camera* camera);
void Camera_InitWithPosition(const VecFx32* position, const fx32 distance, const CameraAngle* angle, const u16 fovY, const u8 projection, Camera* camera);
void Camera_ComputeProjectionMatrix(const u8 projection, Camera* camera);
void Camera_Move(const VecFx32* delta, Camera* camera);

#endif /* MPR_CAMERA_H */