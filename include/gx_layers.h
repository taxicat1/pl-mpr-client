#ifndef MPR_GX_LAYERS_H
#define MPR_GX_LAYERS_H

#include <nitro/gx.h>

typedef struct {
	GXVRamBG             unk_00;
	GXVRamBGExtPltt      unk_04;
	GXVRamSubBG          unk_08;
	GXVRamSubBGExtPltt   unk_0C;
	GXVRamOBJ            unk_10;
	GXVRamOBJExtPltt     unk_14;
	GXVRamSubOBJ         unk_18;
	GXVRamSubOBJExtPltt  unk_1C;
	GXVRamTex            unk_20;
	GXVRamTexPltt        unk_24;
} GXBanks;

void GXLayers_SetBanks(const GXBanks* banks);
void GXLayers_DisableEngineALayers(void);
void GXLayers_EngineAToggleLayers(u8 layerMask, u8 enable);
void GXLayers_DisableEngineBLayers(void);
void GXLayers_EngineBToggleLayers(u8 layerMask, u8 enable);
void GXLayers_TurnBothDispOn(void);
void GXLayers_SwapDisplay(void);

#endif /* MPR_GX_LAYERS_H */
