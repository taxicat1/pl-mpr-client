#ifndef MPR_MPR_CLIENT_MPR_MENU_H
#define MPR_MPR_CLIENT_MPR_MENU_H

#include <nitro/types.h>
#include <nnsys.h>

#include "heap.h"
#include "string_gf.h"
#include "overlay_manager.h"
#include "bg_window.h"

extern const ApplicationManagerTemplate gMPRMenuAppTemplate;

void MPRMenuWindow_PrintString(BgConfig* bgConfig, Window* window, String* string);
void MPRMenuWindow_DrawScrollArrow(Window* window, int idx);
void MPRMenu_LoadNetworkIconPalette(NNS_G2D_VRAM_TYPE vramType, u32 offset, HeapID heapID);
void MPRMenu_LoadNetworkIconGraphic(NNS_G2D_VRAM_TYPE vramType, HeapID heapID);
void MPRMenu_SetNetworkIconDefault(void);
void MPRMenu_SetNetworkIcon(u16 x, u16 y, u32 unused);
void MPRMenu_ClearNetworkIcon(void);

#endif /* MPR_MPR_CLIENT_MPR_MENU_H */
