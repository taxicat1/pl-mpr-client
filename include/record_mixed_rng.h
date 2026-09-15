#ifndef MPR_RECORD_MIXED_RNG_H
#define MPR_RECORD_MIXED_RNG_H

#include "constants/string.h"
#include "charcode.h"
#include "savedata.h"
#include "string_gf.h"

#define RECORD_MIXED_RNG_MAX  6

typedef struct {
	charcode_t  groupName[UNION_GROUP_NAME_LEN + 1];
	charcode_t  playerName[TRAINER_NAME_LEN + 1];
	u8          gender;
	u8          language;
	u16         dummy;
	u32         seed;
	u32         rand;
} RecordMixedRNG;

int RecordMixedRNG_GetSaveSize();
void RecordMixedRNG_Init(RecordMixedRNG* rngCollection);

#endif /* MPR_RECORD_MIXED_RNG_H */
