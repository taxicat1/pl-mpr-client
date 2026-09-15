#ifndef MPR_SCREEN_FADE_FUNCS_H
#define MPR_SCREEN_FADE_FUNCS_H

#include "screen_fade.h"

typedef BOOL (*ScreenFadeFunc)(ScreenFade*);

// So many unused functions, what a waste of space
BOOL ScreenFadeFunc_BrightnessOut(ScreenFade* fade);
BOOL ScreenFadeFunc_BrightnessIn(ScreenFade* fade);
BOOL ScreenFadeFunc_DownwardOut(ScreenFade* fade);
BOOL ScreenFadeFunc_DownwardIn(ScreenFade* fade);
BOOL ScreenFadeFunc_UpwardOut(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused5(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused6(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused7(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused8(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused9(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused10(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused11(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused12(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused13(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused14(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused15(ScreenFade* fade);
BOOL ScreenFadeFunc_CircleOut(ScreenFade* fade);
BOOL ScreenFadeFunc_CircleIn(ScreenFade* fade);
BOOL ScreenFadeFunc_TopHalfCircleOut(ScreenFade* fade);
BOOL ScreenFadeFunc_TopHalfCircleIn(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused20(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused21(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused22(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused23(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused24(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused25(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused26(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused27(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused28(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused29(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused30(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused31(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused32(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused33(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused34(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused35(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused36(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused37(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused38(ScreenFade* fade);
BOOL ScreenFadeFunc_Unused39(ScreenFade* fade);
BOOL ScreenFadeFunc_ClampOut(ScreenFade* fade);
BOOL ScreenFadeFunc_ClampIn(ScreenFade* fade);

#endif /* MPR_SCREEN_FADE_FUNCS_H */
