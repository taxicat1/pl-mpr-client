#ifndef MPR_CHAR_TRANSFER_H
#define MPR_CHAR_TRANSFER_H

#include <nitro/gx.h>
#include <nnsys/g2d.h>

#include "constants/heap.h"

typedef struct {
	int     maxTasks;
	int     sizeMain;
	int     sizeSub;
	HeapID  heapID;
} CharTransferTemplate;

typedef struct {
	int                maxTasks;
	int                sizeMain;
	int                sizeSub;
	GXOBJVRamModeChar  modeMain;
	GXOBJVRamModeChar  modeSub;
} CharTransferTemplateWithModes;

typedef struct {
	NNSG2dCharacterData*  data;
	NNS_G2D_VRAM_TYPE     vramType;
	u32                   resourceID;
	BOOL                  atEnd;
} CharTransferTaskTemplate;

typedef struct {
	u32  size;
	u32  offset;
	u32  vramType;
} CharTransferAllocation;

void CharTransfer_Init(const CharTransferTemplate* transferTemplate);
void CharTransfer_InitWithVramModes(const CharTransferTemplate* transferTemplate, GXOBJVRamModeChar modeMain, GXOBJVRamModeChar modeSub);
void CharTransfer_Free(void);
void CharTransfer_SetOffsetSub(u32 offset);
void CharTransfer_ClearBuffers(void);
BOOL CharTransfer_Request(const CharTransferTaskTemplate* transferTaskTemplate);
BOOL CharTransfer_HasTask(int resourceID);
void CharTransfer_ReplaceCharData(int resourceID, NNSG2dCharacterData* data);
void CharTransfer_ResetTask(int resourceID);
void CharTransfer_ResetAllTasks(void);
NNSG2dImageProxy* CharTransfer_GetImageProxy(int resourceID);
NNSG2dImageProxy* CharTransfer_ResizeTaskRange(int resourceID, u32 size);
NNSG2dImageProxy* CharTransfer_CopyTask(const NNSG2dImageProxy* imageProxy);
void CharTransfer_DeleteTask(const NNSG2dImageProxy* imageProxy);
BOOL CharTransfer_AllocRange(int size, BOOL atEnd, NNS_G2D_VRAM_TYPE vramType, CharTransferAllocation* allocation);
void CharTransfer_ClearRange(CharTransferAllocation* allocation);
int CharTransfer_GetBlockSize(GXOBJVRamModeChar vramMode);

#endif /* MPR_CHAR_TRANSFER_H */
