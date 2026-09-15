#include <nitro.h>

#include "game_options.h"


void Options_Init(Options* options) {
	MI_CpuFill8(options, 0, sizeof(Options));
	
	options->textSpeed   = OPTIONS_TEXT_SPEED_NORMAL;
	options->soundMode   = OPTIONS_SOUND_MODE_STEREO;
	options->battleStyle = OPTIONS_BATTLE_STYLE_SHIFT;
	options->battleScene = OPTIONS_BATTLE_SCENE_ON;
	options->buttonMode  = OPTIONS_BUTTON_MODE_NORMAL;
	options->frame       = OPTIONS_FRAME_1;
}


int Options_GetFrame(const Options* options) {
	return options->frame;
}
