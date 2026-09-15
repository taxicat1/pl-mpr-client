#ifndef MPR_STRUCT_DAYCARE_H
#define MPR_STRUCT_DAYCARE_H

#include "struct_defs/pokemon.h"
#include "mail.h"

typedef struct {
	Mail  mail;
	u16   otName[TRAINER_NAME_LEN + 1];
	u16   monName[MON_NAME_LEN + 1];
	u8    unk_5E_0 : 4;
	u8    unk_5E_4 : 4;
} DaycareMail;

typedef struct {
	BoxPokemon   boxMon;
	DaycareMail  daycareMail;
	u32          steps;
} DaycareMon;

typedef struct {
	DaycareMon  mons[2];
	u32         offspringPersonality;
	u8          stepCounter;
} Daycare;

#endif /* MPR_STRUCT_DAYCARE_H */
