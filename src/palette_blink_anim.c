#include <nitro.h>
#include <string.h>

#include "palette_blink_anim.h"

#include "constants/graphics.h"

#include "bg_window.h"
#include "hardware_palette.h"
#include "palette.h"
#include "sys_task_extensions.h"
#include "sys_task_manager.h"

static void PaletteAnimator_0200CF14(SysTask* param0, void* param1);
static u8 PaletteAnimator_0200CF8C(PaletteAnimator* param0);
static void PaletteAnimator_0200CFE8(PaletteAnimator* param0, u16* param1);
static void PaletteAnimator_0200D000(PaletteAnimator* param0, u16* param1);
static void PaletteAnimator_0200D018(PaletteAnimator* param0, u16* param1);
static void PaletteAnimator_0200D034(PaletteAnimator* param0, u16* param1);


PaletteAnimator* PaletteAnimator_CreateTask(PaletteData* palette, u16 param1, u16 param2, HeapID heapID) {
	PaletteAnimator* v0;
	SysTask* v1;
	u16* v2;
	
	v1 = SysTask_StartAndAllocateParam(PaletteAnimator_0200CF14, sizeof(PaletteAnimator), 0, heapID);
	v0 = (PaletteAnimator*)SysTask_GetParam(v1);
	
	if (palette != NULL) {
		if (param1 == 0) {
			v2 = PaletteData_GetUnfadedBuffer(palette, 0);
			v0->unk_04 = PaletteAnimator_0200D018;
		} else {
			v2 = PaletteData_GetUnfadedBuffer(palette, 1);
			v0->unk_04 = PaletteAnimator_0200D034;
		}
	} else {
		if (param1 == 0) {
			v2 = (u16*)GetHardwareMainBgPaletteAddress();
			v0->unk_04 = PaletteAnimator_0200CFE8;
		} else {
			v2 = (u16*)GetHardwareSubBgPaletteAddress();
			v0->unk_04 = PaletteAnimator_0200D000;
		}
	}
	
	MI_CpuCopy16(&v2[param2 * 16], v0->unk_0C, 0x20);
	MI_CpuCopy16(&v2[param2 * 16], v0->unk_2C, 0x20);
	
	v0->unk_00 = v1;
	v0->unk_08 = palette;
	v0->unk_4C = param1;
	v0->unk_4D = param2;
	v0->unk_4E = 1;
	v0->unk_4F = 0;
	
	return v0;
}


void PaletteAnimator_FinishTask(PaletteAnimator* paletteAnimator) {
	SysTask_FinishAndFreeParam(paletteAnimator->unk_00);
}


static void PaletteAnimator_0200CF14(SysTask* task, void* param) {
	PaletteAnimator* v0 = (PaletteAnimator*)param;
	
	switch (v0->unk_4E) {
	case 0:
		v0->unk_4F = 0;
		v0->unk_4E = 1;
		break;
	case 1:
		if (PaletteAnimator_0200CF8C(v0) == 1) {
			v0->unk_04(v0, v0->unk_2C);
		}
		
		v0->unk_4F++;
		
		if (v0->unk_4F == 32) {
			v0->unk_4F = 0;
		}
		break;
	case 2:
		break;
	case 3:
		v0->unk_04(v0, v0->unk_0C);
		SysTask_FinishAndFreeParam(task);
	}
}


static u8 PaletteAnimator_0200CF8C(PaletteAnimator* param0) {
	u32 v0;
	
	if (param0->unk_4F == 0) {
		for (v0 = 0; v0 < 16; v0++) {
			if ((0x7800 & (1 << v0)) == 0) {
				continue;
			}
			
			param0->unk_2C[v0] = param0->unk_0C[v0];
		}
		
		return 1;
	} else if (param0->unk_4F == 24) {
		for (v0 = 0; v0 < 16; v0++) {
			if ((0x7800 & (1 << v0)) == 0) {
				continue;
			}
			
			param0->unk_2C[v0] = param0->unk_0C[15];
		}
		
		return 1;
	}
	
	return 0;
}


static void PaletteAnimator_0200CFE8(PaletteAnimator* param0, u16* param1) {
	Bg_LoadPalette(BG_LAYER_MAIN_0, param1, PALETTE_SIZE_BYTES, PLTT_OFFSET(param0->unk_4D));
}


static void PaletteAnimator_0200D000(PaletteAnimator* param0, u16* param1) {
	Bg_LoadPalette(BG_LAYER_SUB_0, param1, PALETTE_SIZE_BYTES, PLTT_OFFSET(param0->unk_4D));
}


static void PaletteAnimator_0200D018(PaletteAnimator* param0, u16* param1) {
	PaletteData_LoadBuffer(param0->unk_08, param1, PLTTBUF_MAIN_BG, PLTT_DEST(param0->unk_4D), PALETTE_SIZE_BYTES);
}


static void PaletteAnimator_0200D034(PaletteAnimator* param0, u16* param1) {
	PaletteData_LoadBuffer(param0->unk_08, param1, PLTTBUF_SUB_BG, PLTT_DEST(param0->unk_4D), PALETTE_SIZE_BYTES);
}

