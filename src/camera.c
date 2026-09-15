#include <nitro.h>
#include <nnsys.h>
#include <string.h>

#include "camera.h"

#include "heap.h"

#define CAMERA_DEFAULT_ASPECT_RATIO  (FX32_ONE * 4 / 3)

GXBufferMode gBufferMode = GX_BUFFERMODE_W;
static Camera* sActiveCamera = NULL;

static void Camera_AdjustTargetFromAngle(Camera* camera);
static void Camera_Init(const u16 fovY, Camera* camera);
static void Camera_AdjustDeltaPos(Camera const* camera, VecFx32* deltaPos);
static void Camera_UpdateHistory(Camera const* camera, const VecFx32* inPos, VecFx32* outPos);


static void Camera_AdjustTargetFromAngle(Camera* camera) {
	u16 angleX = -camera->angle.x;
	camera->lookAt.target.x = -FX_Mul(FX_Mul(FX_SinIdx(camera->angle.y), camera->distance), FX_CosIdx(camera->angle.x));
	camera->lookAt.target.z = -FX_Mul(FX_Mul(FX_CosIdx(camera->angle.y), camera->distance), FX_CosIdx(camera->angle.x));
	camera->lookAt.target.y = -FX_Mul(FX_SinIdx(angleX), camera->distance);
	
	VEC_Add(&camera->lookAt.target, &camera->lookAt.position, &camera->lookAt.target);
}


static void Camera_Init(const u16 fovY, Camera* camera) {
	camera->fovY = fovY;
	camera->perspective.sinFovY = FX_SinIdx(fovY);
	camera->perspective.cosFovY = FX_CosIdx(fovY);
	camera->perspective.aspectRatio = CAMERA_DEFAULT_ASPECT_RATIO;
	camera->perspective.nearClip = CAMERA_DEFAULT_NEAR_CLIP;
	camera->perspective.farClip = CAMERA_DEFAULT_FAR_CLIP;
	camera->lookAt.up.x = 0;
	camera->lookAt.up.y = FX32_ONE;
	camera->lookAt.up.z = 0;
	camera->targetPos = NULL;
	camera->trackTargetX = FALSE;
	camera->trackTargetY = FALSE;
	camera->trackTargetZ = FALSE;
	camera->history = NULL;
}


static void Camera_AdjustDeltaPos(Camera const* camera, VecFx32* deltaPos) {
	if (camera->trackTargetX == FALSE) {
		deltaPos->x = 0;
	}
	
	if (camera->trackTargetY == FALSE) {
		deltaPos->y = 0;
	}
	
	if (camera->trackTargetZ == FALSE) {
		deltaPos->z = 0;
	}
}


static void Camera_UpdateHistory(Camera const* camera, const VecFx32* inPos, VecFx32* outPos) {
	if (camera->history == NULL) {
		*outPos = *inPos;
		return;
	}
	
	int* curHistoryIndex = &camera->history->currentHistoryIndex;
	int* nextHistoryIndex = &camera->history->nextHistoryIndex;
	
	if (!camera->history->delayReached) {
		*outPos = *inPos;
		
		if (*curHistoryIndex == camera->history->delay) {
			camera->history->delayReached = TRUE;
		}
	} else {
		*outPos = camera->history->positions[*curHistoryIndex];
	}
	
	*curHistoryIndex = (*curHistoryIndex + 1) % camera->history->historySize;
	
	camera->history->positions[*nextHistoryIndex] = *inPos;
	*nextHistoryIndex = (*nextHistoryIndex + 1) % camera->history->historySize;
	
	if (!camera->history->delayX) {
		outPos->x = inPos->x;
	}
	
	if (!camera->history->delayY) {
		outPos->y = inPos->y;
	}
	
	if (!camera->history->delayZ) {
		outPos->z = inPos->z;
	}
}


Camera* Camera_Alloc(const HeapID heapID) {
	return Heap_Alloc(heapID, sizeof(Camera));
}


void Camera_Delete(Camera* camera) {
	Heap_Free(camera);
}


void Camera_SetAsActive(Camera* camera) {
	sActiveCamera = camera;
}


void Camera_ComputeViewMatrix(void) {
	if (sActiveCamera == NULL) {
		return;
	}
	
	if (sActiveCamera->targetPos != NULL) {
		VecFx32 targetPosDelta;
		VecFx32 resultPos;
		
		VEC_Subtract(sActiveCamera->targetPos, &sActiveCamera->prevTargetPos, &targetPosDelta);
		
		Camera_AdjustDeltaPos(sActiveCamera, &targetPosDelta);
		Camera_UpdateHistory(sActiveCamera, &targetPosDelta, &resultPos);
		Camera_Move(&resultPos, sActiveCamera);
		
		sActiveCamera->prevTargetPos = *sActiveCamera->targetPos;
	}
	
	NNS_G3dGlbLookAt(&sActiveCamera->lookAt.position, &sActiveCamera->lookAt.up, &sActiveCamera->lookAt.target);
}


void Camera_ReleaseTarget(Camera* camera) {
	camera->targetPos = NULL;
	camera->trackTargetX = FALSE;
	camera->trackTargetY = FALSE;
	camera->trackTargetZ = FALSE;
}


void Camera_SetClipping(const fx32 nearClip, const fx32 farClip, Camera* camera) {
	camera->perspective.nearClip = nearClip;
	camera->perspective.farClip = farClip;
	
	Camera_ComputeProjectionMatrix(camera->projection, camera);
}


void Camera_InitWithPosition(const VecFx32* position, const fx32 distance, const CameraAngle* angle, const u16 fovY, const u8 projection, Camera* camera) {
	Camera_Init(fovY, camera);
	
	camera->lookAt.position = *position;
	camera->distance        = distance;
	camera->angle           = *angle;
	
	Camera_AdjustTargetFromAngle(camera);
	Camera_ComputeProjectionMatrix(projection, camera);
}


void Camera_ComputeProjectionMatrix(const u8 projection, Camera* camera) {
	if (projection == CAMERA_PROJECTION_PERSPECTIVE) {
		NNS_G3dGlbPerspective(
			camera->perspective.sinFovY,
			camera->perspective.cosFovY,
			camera->perspective.aspectRatio,
			camera->perspective.nearClip,
			camera->perspective.farClip);
		
		camera->projection = CAMERA_PROJECTION_PERSPECTIVE;
		gBufferMode = GX_BUFFERMODE_Z;
	} else {
		fx32 top = FX_Mul(FX_Div(camera->perspective.sinFovY, camera->perspective.cosFovY), camera->distance);
		fx32 right = FX_Mul(top, camera->perspective.aspectRatio);
		
		NNS_G3dGlbOrtho(top, -top, -right, right, camera->perspective.nearClip, camera->perspective.farClip);
		
		camera->projection = CAMERA_PROJECTION_ORTHOGRAPHIC;
		gBufferMode = GX_BUFFERMODE_Z;
	}
}


void Camera_Move(const VecFx32* delta, Camera* camera) {
	VEC_Add(&camera->lookAt.position, delta, &camera->lookAt.position);
	VEC_Add(&camera->lookAt.target, delta, &camera->lookAt.target);
}











