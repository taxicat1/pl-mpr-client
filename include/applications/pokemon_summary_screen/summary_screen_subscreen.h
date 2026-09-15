#ifndef MPR_POKEMON_SUMMARY_SCREEN_SUMMARY_SCREEN_SUBSCREEN_H
#define MPR_POKEMON_SUMMARY_SCREEN_SUMMARY_SCREEN_SUBSCREEN_H

#include "applications/pokemon_summary_screen/summary_screen_main.h"

typedef enum {
	SUMMARY_BUTTON_STATE_INIT_ANIM = 0,
	SUMMARY_BUTTON_STATE_SETUP_PAGE,
	SUMMARY_BUTTON_STATE_RUN_ANIM
} SummaryButtonState;

void PokemonSummaryScreen_SetSubscreenType(PokemonSummaryScreen* summaryScreen);
void PokemonSummaryScreen_UpdateSubscreenButtonGfx(PokemonSummaryScreen* summaryScreen);
u8 PokemonSummaryScreen_CheckSubscreenButtonPress(PokemonSummaryScreen* summaryScreen);
u8 PokemonSummaryScreen_DrawSubscreenButtonAnim(PokemonSummaryScreen* summaryScreen);
void PokemonSummaryScreen_CalcSubscreenButtonTapAnimPos(PokemonSummaryScreen* summaryScreen, s16* outX, s16* outY);
u8 PokemonSummaryScreen_GetSubscreenButtonPage(PokemonSummaryScreen* summaryScreen, u8 button);

#endif /* MPR_POKEMON_SUMMARY_SCREEN_SUMMARY_SCREEN_SUBSCREEN_H */
