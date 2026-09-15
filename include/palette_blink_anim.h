#ifndef MPR_PALETTE_BLINK_ANIM_H
#define MPR_PALETTE_BLINK_ANIM_H

#include <nitro.h>

#include "palette.h"
#include "sys_task_manager.h"

// TODO document field and args
typedef struct PaletteAnimator PaletteAnimator;
struct PaletteAnimator {
	SysTask*      unk_00;
	void          (*unk_04)(PaletteAnimator*, u16*);
	PaletteData*  unk_08;
	u16           unk_0C[16];
	u16           unk_2C[16];
	u8            unk_4C;
	u8            unk_4D;
	u8            unk_4E;
	u8            unk_4F;
};

PaletteAnimator* PaletteAnimator_CreateTask(PaletteData* palette, u16 param1, u16 param2, HeapID heapID);
void PaletteAnimator_FinishTask(PaletteAnimator* paletteAnimator);

#endif /* MPR_PALETTE_BLINK_ANIM_H */