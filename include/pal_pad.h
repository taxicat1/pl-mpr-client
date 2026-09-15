#ifndef MPR_PAL_PAD_H
#define MPR_PAL_PAD_H

#include "constants/string.h"
#include "savedata.h"

#define PAL_PAD_ENTRIES  16

typedef struct {
	u16  trainerName[TRAINER_NAME_LEN + 1];
	u32  trainerId;
	u8   language;
	u8   gameCode;
	u8   gender;
	u8   padding_17;
	u32  associatedTrainerIds[PAL_PAD_ENTRIES];
	u8   associatedTrainerGameCodes[PAL_PAD_ENTRIES];
	u8   associatedTrainerLanguages[PAL_PAD_ENTRIES];
	u8   associatedTrainerGenders[PAL_PAD_ENTRIES];
} PalPad;

int PalPad_GetSaveSize(void);
void PalPad_Init(PalPad* palPad);

#endif /* MPR_PAL_PAD_H */
