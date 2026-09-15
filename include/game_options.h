#ifndef MPR_GAME_OPTIONS_H
#define MPR_GAME_OPTIONS_H

#include "constants/game_options.h"

#include "savedata.h"

typedef struct {
	u16  textSpeed   : 4;
	u16  soundMode   : 2;
	u16  battleStyle : 1;
	u16  battleScene : 1;
	u16  buttonMode  : 2;
	u16  frame       : 5;
	u16              : 1;
} Options;

void Options_Init(Options* options);
int Options_GetFrame(const Options* options);

#endif /* MPR_GAME_OPTIONS_H */
