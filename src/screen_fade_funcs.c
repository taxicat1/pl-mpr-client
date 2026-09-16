// File partially documented. The overwhelming majority of functions in this file go unused.

#include <nitro.h>
#include <string.h>

#include "screen_fade_funcs.h"

#include "assert.h"
#include "constants/graphics.h"
#include "gx_gf.h"
#include "hardware_window.h"
#include "heap.h"
#include "screen_fade.h"
#include "sys_task.h"
#include "sys_task_manager.h"

typedef struct {
	int unk_00;
	int unk_04;
	int unk_08;
	int unk_0C;
	int unk_10;
	int unk_14;
	DSScreen screen;
} ScreenFadeFuncStruct1;

typedef struct {
	int unk_00;
	int unk_04;
	int unk_08;
	int unk_0C;
} ScreenFadeFuncStruct2;

typedef struct {
	ScreenFadeFuncStruct2 unk_00;
	ScreenFadeFuncStruct2 unk_10;
	ScreenFadeFuncStruct2 unk_20;
	DSScreen screen;
	int unk_34;
	int unk_38;
	int unk_3C;
	int unk_40;
	int unk_44;
	HardwareWindowSettings* unk_48;
} ScreenFadeFuncStruct4;

typedef struct {
	ScreenFadeFuncStruct2 unk_00;
	ScreenFadeFuncStruct2 unk_04;
	u8 unk_08;
	u8 unk_09;
	u8 unk_0A;
	u8 unk_0B;
} ScreenFadeFuncStruct5;

typedef struct {
	ScreenFadeFuncStruct4 unk_00;
	ScreenFadeFuncStruct4 unk_4C;
} ScreenFadeFuncStruct6;

typedef struct {
	short unk_00[2][192];
	short unk_300[2][192];
	int unk_600;
} ScreenFadeFuncStruct7;

typedef struct {
	ScreenFadeFuncStruct7* unk_00;
	int unk_04;
	DSScreen screen;
} ScreenFadeFuncStruct8;

typedef struct {
	ScreenFadeFuncStruct8 unk_00;
	int unk_0C;
	int unk_10;
	int unk_14;
	int unk_18;
	int unk_1C;
	int unk_20;
	int unk_24;
	HeapID heapID;
	int unk_2C;
	HardwareWindowSettings* unk_30;
	ScreenFadeHBlanks* unk_34;
} ScreenFadeFuncStruct9;

typedef struct {
	int unk_00;
	int unk_04;
	short unk_08;
	short unk_0A;
	u8 unk_0C;
	u8 unk_0D;
	u8 unk_0E;
	u8 unk_0F;
} ScreenFadeFuncStruct10;

typedef struct {
	ScreenFadeFuncStruct8 unk_00;
	int unk_0C;
	int unk_10;
	int unk_14;
	int unk_18;
	int unk_1C;
	int unk_20;
	HardwareWindowSettings* unk_24;
	ScreenFadeHBlanks* unk_28;
	HeapID heapID;
} ScreenFadeFuncStruct11;

typedef struct {
	int unk_00;
	int unk_04;
	u8 unk_08;
	u8 unk_09;
	u8 unk_0A;
	u8 unk_0B;
} ScreenFadeFuncStruct12;

typedef struct {
	ScreenFadeFuncStruct8 unk_00;
	int unk_0C;
	int unk_10;
	int unk_14;
	int unk_18;
	int unk_1C;
	int unk_20;
	int unk_24;
	HardwareWindowSettings* unk_28;
	ScreenFadeHBlanks* unk_2C;
	HeapID heapID;
} ScreenFadeFuncStruct13;

typedef struct {
	int unk_00;
	int unk_04;
	u8 unk_08;
	u8 unk_09;
	u8 unk_0A;
	u8 unk_0B;
} ScreenFadeFuncStruct14;

typedef struct {
	ScreenFadeFuncStruct2 unk_00;
	ScreenFadeFuncStruct2 unk_10;
	ScreenFadeFuncStruct2 unk_20;
} ScreenFadeFuncStruct15;

typedef struct {
	ScreenFadeFuncStruct8 unk_00;
	ScreenFadeFuncStruct15* unk_0C;
	int unk_10;
	int unk_14;
	int unk_18;
	int unk_1C;
	int unk_20;
	HardwareWindowSettings* unk_24;
	ScreenFadeHBlanks* unk_28;
	HeapID heapID;
} ScreenFadeFuncStruct16;

typedef struct {
	const ScreenFadeFuncStruct2* unk_00;
	const ScreenFadeFuncStruct2* unk_04;
	u8 unk_08;
	u8 unk_09;
	u8 unk_0A;
	u8 unk_0B;
	u8 unk_0C;
} ScreenFadeFuncStruct17;

typedef struct {
	int unk_00;
	int unk_04;
	int unk_08;
} ScreenFadeFuncStruct18;

typedef struct {
	ScreenFadeFuncStruct8 unk_00;
	ScreenFadeFuncStruct18 unk_0C;
	int unk_18;
	int unk_1C;
	int unk_20;
	int unk_24;
	int unk_28;
	HeapID heapID;
	HardwareWindowSettings* unk_30;
	ScreenFadeHBlanks* unk_34;
} ScreenFadeFuncStruct19;

typedef struct {
	int unk_00;
	int unk_04;
	u8 unk_08;
	u8 unk_09;
	u8 unk_0A;
} ScreenFadeFuncStruct20;

typedef struct {
	int unk_00;
	int unk_04;
	int unk_08;
} ScreenFadeFuncStruct21;

typedef struct {
	ScreenFadeFuncStruct8 unk_00;
	ScreenFadeFuncStruct21 unk_0C;
	int unk_18;
	int unk_1C;
	int unk_20;
	int unk_24;
	int unk_28;
	HeapID heapID;
	HardwareWindowSettings* unk_30;
	ScreenFadeHBlanks* unk_34;
} ScreenFadeFuncStruct22;

typedef struct {
	int unk_00;
	int unk_04;
	u8 unk_08;
	u8 unk_09;
	u8 unk_0A;
} ScreenFadeFuncStruct23;

typedef struct {
	u8 unk_00[192];
	u8 unk_C0[192];
	int unk_180;
} ScreenFadeFuncStruct24;

typedef struct {
	ScreenFadeFuncStruct24 unk_00[2];
	u8 unk_308;
	u8 screen;
} ScreenFadeFuncStruct25;

typedef struct {
	u8 unk_00;
	u8 unk_01;
	u8 unk_02;
} ScreenFadeFuncStruct26;

typedef struct {
	ScreenFadeFuncStruct25 unk_00;
	const ScreenFadeFuncStruct26* unk_30C;
	int unk_310;
	int unk_314;
	int unk_318;
	int unk_31C;
	int unk_320;
	int unk_324;
	HeapID heapID;
	HardwareWindowSettings* unk_32C;
	ScreenFadeHBlanks* unk_330;
} ScreenFadeFuncStruct27;

typedef struct {
	const ScreenFadeFuncStruct26* unk_00;
	u8 unk_04;
	u8 unk_05;
} ScreenFadeFuncStruct28;

typedef struct {
	ScreenFadeFuncStruct5 unk_00;
	ScreenFadeFuncStruct28 unk_0C;
	fx32 unk_14;
} ScreenFadeFuncStruct29;

typedef struct {
	ScreenFadeFuncStruct4 unk_00;
	ScreenFadeFuncStruct27 unk_4C;
	ScreenFadeFuncStruct29* unk_380;
	u8 unk_384;
	u8 unk_385;
	u8 unk_386;
	u8 unk_387;
} ScreenFadeFuncStruct30;

static fx32 ScreenFade_02009F50(int param0);
static int ScreenFade_02009F6C(int param0, int param1);
static void ScreenFade_02009F94(int param0, int* param1, int param2, int param3);
static int ScreenFade_02009FE8(int param0, int param1);
static int ScreenFade_0200A000(int param0, int param1, int param2);
static int ScreenFade_0200A010(int param0, int param1);
static void ScreenFade_0200A020(ScreenFadeFuncStruct2* param0, ScreenFadeFuncStruct2* param1);
static void ScreenFade_0200A044(ScreenFadeFuncStruct2* param0, ScreenFadeFuncStruct2* param1, ScreenFadeFuncStruct2* param2, const ScreenFadeFuncStruct2* param3, const ScreenFadeFuncStruct2* param4, int param5);
static void ScreenFade_0200A4C8(int param0, HardwareWindowSettings* param1, DSScreen screen);
static void ScreenFade_0200A1CC(void* param0);
static void ScreenFade_0200A45C(ScreenFadeFuncStruct8* param0);
static void ScreenFade_0200A464(ScreenFadeFuncStruct8* param0);
static ScreenFadeFuncStruct7 *ScreenFade_0200A474(ScreenFadeFuncStruct8* param0, int param1);
static void ScreenFade_0200A494(SysTask* param0, void* param1);
static void ScreenFade_0200A5DC(ScreenFadeFuncStruct25* param0);
static void ScreenFade_0200A5F4(ScreenFadeHBlanks* param0, ScreenFadeFuncStruct25* param1, u32 heapID);
static void ScreenFade_0200A60C(ScreenFadeHBlanks* param0, ScreenFadeFuncStruct25* param1, u32 param2);
static void ScreenFade_0200A61C(SysTask* param0, void* param1);
static void ScreenFade_0200A648(void* param0);
static void ScreenFade_0200A0A8(ScreenFade* fade, int param1);
static BOOL ScreenFade_0200A148(ScreenFade* fade);
static BOOL ScreenFade_0200A188(ScreenFadeFuncStruct1* param0);
static void ScreenFade_0200AB64(ScreenFade* fade, const ScreenFadeFuncStruct5* param1);
static BOOL ScreenFade_0200ABB8(ScreenFade* fade);
static void ScreenFade_0200AC08(ScreenFade* fade, const ScreenFadeFuncStruct5* param1, const ScreenFadeFuncStruct5* param2);
static BOOL ScreenFade_0200AC60(ScreenFade* fade);
static void ScreenFade_0200ACBC(ScreenFadeFuncStruct4* param0, const ScreenFadeFuncStruct5* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5);
static BOOL ScreenFade_0200AD2C(ScreenFadeFuncStruct4* param0);
static void ScreenFade_0200ADB4(ScreenFade* fade, const ScreenFadeFuncStruct10* param1);
static BOOL ScreenFade_0200ADEC(ScreenFade* fade);
static void ScreenFade_0200AE48(ScreenFadeFuncStruct9* param0, const ScreenFadeFuncStruct10* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200AF20(ScreenFadeFuncStruct9* param0);
static void ScreenFade_0200B004(ScreenFadeFuncStruct9* param0);
static void ScreenFade_0200B088(ScreenFade* fade, const ScreenFadeFuncStruct12* param1);
static BOOL ScreenFade_0200B0C0(ScreenFade* fade);
static void ScreenFade_0200B11C(ScreenFadeFuncStruct11* param0, const ScreenFadeFuncStruct12* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200B1E4(ScreenFadeFuncStruct11* param0);
static void ScreenFade_0200B234(ScreenFadeFuncStruct11* param0);
static void ScreenFade_0200B28C(ScreenFade* fade, const ScreenFadeFuncStruct14* param1);
static BOOL ScreenFade_0200B2C4(ScreenFade* fade);
static void ScreenFade_0200B318(ScreenFadeFuncStruct13* param0, const ScreenFadeFuncStruct14* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200B3EC(ScreenFadeFuncStruct13* param0);
static void ScreenFade_0200B43C(ScreenFadeFuncStruct13* param0);
static void ScreenFade_0200B524(ScreenFade* fade, const ScreenFadeFuncStruct17* param1);
static BOOL ScreenFade_0200B55C(ScreenFade* fade);
static BOOL ScreenFade_0200B6D0(ScreenFadeFuncStruct16* param0);
static void ScreenFade_0200B5BC(ScreenFadeFuncStruct16* param0, const ScreenFadeFuncStruct17* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static void ScreenFade_0200B720(ScreenFadeFuncStruct16* param0);
static void ScreenFade_0200B730(ScreenFadeFuncStruct16* param0);
static void ScreenFade_0200B7BC(ScreenFadeFuncStruct16* param0);
static void ScreenFade_0200B764(ScreenFadeFuncStruct8* param0, ScreenFadeFuncStruct2* param1);
static void ScreenFade_0200B7E4(ScreenFade* fade, ScreenFadeFuncStruct20* param1);
static BOOL ScreenFade_0200B824(ScreenFade* fade);
static void ScreenFade_0200B884(ScreenFadeFuncStruct19* param0, ScreenFadeFuncStruct20* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200B980(ScreenFadeFuncStruct19* param0);
static void ScreenFade_0200B9D8(ScreenFadeFuncStruct19* param0);
static void ScreenFade_0200B9DC(ScreenFadeFuncStruct19* param0);
static void ScreenFade_0200BB00(ScreenFadeFuncStruct18* param0, int param1, int param2);
static void ScreenFade_0200BB18(ScreenFade* fade, ScreenFadeFuncStruct23* param1);
static BOOL ScreenFade_0200BB58(ScreenFade* fade);
static void ScreenFade_0200BBB8(ScreenFadeFuncStruct22* param0, ScreenFadeFuncStruct23* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200BC88(ScreenFadeFuncStruct22* param0);
static void ScreenFade_0200BCE0(ScreenFadeFuncStruct22* param0);
static void ScreenFade_0200BCE4(ScreenFadeFuncStruct22* param0);
static void ScreenFade_0200BDB0(ScreenFadeFuncStruct21* param0, int param1, int param2);
static void ScreenFade_0200BDC8(ScreenFade* fade, ScreenFadeFuncStruct28* param1);
static BOOL ScreenFade_0200BE0C(ScreenFade* fade);
static void ScreenFade_0200BE6C(ScreenFadeFuncStruct27* param0, ScreenFadeFuncStruct28* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200BF58(ScreenFadeFuncStruct27* param0);
static void ScreenFade_0200BFB8(ScreenFadeFuncStruct27* param0);
static void ScreenFade_0200BFBC(ScreenFadeFuncStruct27* param0);
static void ScreenFade_0200BFF8(const ScreenFadeFuncStruct26* param0, ScreenFadeFuncStruct24* param1, int param2, int param3);
static void ScreenFade_0200C048(ScreenFade* fade, ScreenFadeFuncStruct29* param1);
static BOOL ScreenFade_0200C0B0(ScreenFade* fade);
static void ScreenFade_0200C118(ScreenFadeFuncStruct30* param0, ScreenFadeFuncStruct29* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, int param7);
static BOOL ScreenFade_0200C1A4(ScreenFadeFuncStruct30* param0, ScreenFade* param1);
static void ScreenFade_0200C218(ScreenFadeFuncStruct30* param0, ScreenFadeFuncStruct29* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID);
static BOOL ScreenFade_0200C288(ScreenFadeFuncStruct30* param0, ScreenFade* param1);


BOOL ScreenFadeFunc_BrightnessOut(ScreenFade* fade) {
	if (fade->state == 0) {
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_BRIGHTNESS;
		ScreenFade_0200A0A8(fade, 1);
		return FALSE;
	}
	
	return ScreenFade_0200A148(fade);
}


BOOL ScreenFadeFunc_BrightnessIn(ScreenFade* fade) {
	if (fade->state == 0) {
		fade->direction = FADE_IN;
		fade->method = FADE_BY_BRIGHTNESS;
		ScreenFade_0200A0A8(fade, 0);
		return FALSE;
	}
	
	return ScreenFade_0200A148(fade);
}


BOOL ScreenFadeFunc_DownwardOut(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0 = {
			0, 192, 1
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL, 1, 1
		};
		
		v1.unk_00 = &v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_DownwardIn(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0 = {
			0, 192, 0
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL, 1, 0
		};
		
		v1.unk_00 = &v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_UpwardOut(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0 = {
			192,
			0,
			1
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL,
			1,
			1
		};
		
		v1.unk_00 = &v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return 0;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused5(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0 = {
			192,
			0,
			0
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL,
			1,
			0
		};
		
		v1.unk_00 = &v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused6(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 255, 192 },
			{ 0, 0, 0, 192 },
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused7(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 0, 192 },
			{ 0, 0, 255, 192 },
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused8(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[2] = {
			{ 0, 96, 1 },
			{ 192, 96, 1 }
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL,
			2,
			1
		};
		
		v1.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused9(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[2] = {
			{ 96, 0, 0 },
			{ 96, 192, 0 }
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL,
			2,
			0
		};
		
		v1.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused10(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[2] = {
			{ 96, 0, 1 },
			{ 96, 192, 1 }
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL,
			2,
			1
		};
		
		v1.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused11(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[2] = {
			{ 0, 96, 0 },
			{ 192, 96, 0 }
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL,
			2,
			0
		};
		
		v1.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused12(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 255, 192 },
			{ 128, 0, 128, 192 },
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused13(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 128, 0, 128, 192 },
			{ 0, 0, 255, 192 },
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused14(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 128, 0, 128, 192 },
			{ 0, 0, 128, 192 },
			0,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			1
		};
		
		static const ScreenFadeFuncStruct5 v1 = {
			{ 128, 0, 128, 192 },
			{ 128, 0, 255, 192 },
			1,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AC08(fade, &v0, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200AC60(fade);
}


BOOL ScreenFadeFunc_Unused15(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 128, 192 },
			{ 128, 0, 128, 192 },
			0,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			0
		};
		
		static const ScreenFadeFuncStruct5 v1 = {
			{ 128, 0, 255, 192 },
			{ 128, 0, 128, 192 },
			1,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AC08(fade, &v0, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200AC60(fade);
}


BOOL ScreenFadeFunc_CircleOut(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct10 v0 = {
			256,
			0,
			128,
			96,
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200ADB4(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ADEC(fade);
}


BOOL ScreenFadeFunc_CircleIn(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct10 v0 = {
			0,
			256,
			128,
			96,
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200ADB4(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ADEC(fade);
}


BOOL ScreenFadeFunc_TopHalfCircleOut(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct10 v0 = {
			512,
			0,
			128,
			288,
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200ADB4(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200ADEC(fade);
}


BOOL ScreenFadeFunc_TopHalfCircleIn(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct10 v0 = {
			0,
			512,
			128,
			288,
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200ADB4(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200ADEC(fade);
}


BOOL ScreenFadeFunc_Unused20(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct12 v0 = {
			((0xFFFF * 90) / 360),
			0,
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B088(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B0C0(fade);
}


BOOL ScreenFadeFunc_Unused21(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct12 v0 = {
			0,
			((0xFFFF * 90) / 360),
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B088(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B0C0(fade);
}


BOOL ScreenFadeFunc_Unused22(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 255, 192 },
			{ 128, 96, 128, 96 },
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused23(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 128, 96, 128, 96 },
			{ 0, 0, 255, 192 },
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused24(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 128, 96, 128, 96 },
			{ 0, 0, 255, 192 },
			0,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused25(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 255, 192 },
			{ 128, 96, 128, 96 },
			0,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused26(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct14 v0 = {
			((0xFFFF * 90) / 360),
			((0xFFFF * 0) / 360),
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B28C(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}

	return ScreenFade_0200B2C4(fade);
}


BOOL ScreenFadeFunc_Unused27(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct14 v0 = {
			((0xFFFF * 0) / 360),
			((0xFFFF * 90) / 360),
			0,
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B28C(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B2C4(fade);
}


BOOL ScreenFadeFunc_Unused28(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct2 v0[] = {
			{ 0, 0, 255, 48 },
			{ 0, 47, 255, 96 },
			{ 0, 96, 255, 144 },
			{ 0, 144, 255, 192 }
		};
		
		static const ScreenFadeFuncStruct2 v1[] = {
			{ 0, 0, 0, 48 },
			{ 255, 47, 255, 96 },
			{ 0, 96, 0, 144 },
			{ 255, 144, 255, 192 }
		};
		
		ScreenFadeFuncStruct17 v2;
		
		v2.unk_00 = v0;
		v2.unk_04 = v1;
		v2.unk_08 = 4;
		v2.unk_09 = 0;
		v2.unk_0A = GX_BLEND_ALL;
		v2.unk_0B = GX_BLEND_PLANEMASK_BD;
		v2.unk_0C = 1;
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B524(fade, &v2);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B55C(fade);
}


BOOL ScreenFadeFunc_Unused29(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct2 v0[] = {
			{ 255, 0, 255, 48 },
			{ 0, 47, 0, 96 },
			{ 255, 96, 255, 144 },
			{ 0, 144, 0, 192 }
		};
		
		static const ScreenFadeFuncStruct2 v1[] = {
			{ 0, 0, 255, 48 },
			{ 0, 47, 255, 96 },
			{ 0, 96, 255, 144 },
			{ 0, 144, 255, 192 }
		};
		
		ScreenFadeFuncStruct17 v2;
		
		v2.unk_00 = v0;
		v2.unk_04 = v1;
		v2.unk_08 = 4;
		v2.unk_09 = 0;
		v2.unk_0A = GX_BLEND_ALL;
		v2.unk_0B = GX_BLEND_PLANEMASK_BD;
		v2.unk_0C = 0;
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B524(fade, &v2);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B55C(fade);
}


BOOL ScreenFadeFunc_Unused30(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[3] = {
			{ 0, 64, 1 },
			{ 64, 128, 1 },
			{ 128, 192, 1 },
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL, 3, 1
		};
		
		v1.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused31(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[3] = {
			{ 64, 0, 0 },
			{ 128, 64, 0 },
			{ 192, 128, 0 },
		};
		
		static ScreenFadeFuncStruct28 v1 = {
			NULL, 3, 0
		};
		
		v1.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BDC8(fade, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BE0C(fade);
}


BOOL ScreenFadeFunc_Unused32(ScreenFade* fade) {
	if (fade->state == 0) {
		ScreenFadeFuncStruct20 v0 = {
			((0 * 0xFFFF) / 360),
			((179 * 0xFFFF) / 360),
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B7E4(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B824(fade);
}


BOOL ScreenFadeFunc_Unused33(ScreenFade* fade) {
	if (fade->state == 0) {
		ScreenFadeFuncStruct20 v0 = {
			((0 * 0xFFFF) / 360),
			((179 * 0xFFFF) / 360),
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200B7E4(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200B824(fade);
}


BOOL ScreenFadeFunc_Unused34(ScreenFade* fade) {
	if (fade->state == 0) {
		ScreenFadeFuncStruct23 v0 = {
			((0 * 0xFFFF) / 360),
			((45 * 0xFFFF) / 360),
			GX_BLEND_ALL,
			GX_BLEND_PLANEMASK_BD,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BB18(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BB58(fade);
}


BOOL ScreenFadeFunc_Unused35(ScreenFade* fade) {
	if (fade->state == 0) {
		ScreenFadeFuncStruct23 v0 = {
			((0 * 0xFFFF) / 360),
			((45 * 0xFFFF) / 360),
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200BB18(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200BB58(fade);
}


BOOL ScreenFadeFunc_Unused36(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct10 v0 = {
			512, 0, 128, -80, 0, GX_BLEND_ALL, GX_BLEND_PLANEMASK_BD, 1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200ADB4(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ADEC(fade);
}


BOOL ScreenFadeFunc_Unused37(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct10 v0 = {
			0, 512, 128, -80, 0, GX_BLEND_ALL, GX_BLEND_PLANEMASK_BD, 0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200ADB4(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ADEC(fade);
}


BOOL ScreenFadeFunc_Unused38(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 0, 192 },
			{ 0, 0, 255, 192 },
			0,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			1
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_Unused39(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct5 v0 = {
			{ 0, 0, 255, 192 },
			{ 0, 0, 0, 192 },
			0,
			GX_BLEND_PLANEMASK_BD,
			GX_BLEND_ALL,
			0
		};
		
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200AB64(fade, &v0);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200ABB8(fade);
}


BOOL ScreenFadeFunc_ClampOut(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[2] = {
			{ 0, 94, 1 },
			{ 192, 98, 1 },
		};
		
		static ScreenFadeFuncStruct29 v1 = {
			{
				{ 0, 94, 255, 98 },
				{ 128, 96, 128, 96 },
				0,
				GX_BLEND_ALL,
				GX_BLEND_PLANEMASK_BD,
				1,
			},
			{
				NULL,
				2,
				1,
			},
			FX32_CONST(0.70f),
		};
		
		v1.unk_0C.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200C048(fade, &v1);
		fade->direction = FADE_OUT;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200C0B0(fade);
}


BOOL ScreenFadeFunc_ClampIn(ScreenFade* fade) {
	if (fade->state == 0) {
		static const ScreenFadeFuncStruct26 v0[2] = {
			{ 94, 0, 0 },
			{ 98, 192, 0 },
		};
		
		static ScreenFadeFuncStruct29 v1 = {
			{
				{ 128, 96, 128, 96 },
				{ 0, 94, 255, 98 },
				0,
				GX_BLEND_ALL,
				GX_BLEND_PLANEMASK_BD,
				0,
			},
			{
				NULL,
				2,
				0,
			},
			FX32_CONST(0.70f),
		};
		
		v1.unk_0C.unk_00 = v0;
		SetScreenBackgroundColor(fade->color);
		ScreenFade_0200C048(fade, &v1);
		fade->direction = FADE_IN;
		fade->method = FADE_BY_WINDOW;
		
		return FALSE;
	}
	
	return ScreenFade_0200C0B0(fade);
}


static fx32 ScreenFade_02009F50(int param0) {
	return FX_Div(FX_SinIdx(param0), FX_CosIdx(param0));
}


static int ScreenFade_02009F6C(int param0, int param1) {
	fx32 v0;
	fx32 v1;

	v0 = ScreenFade_02009F50(param0);

	v1 = FX_Mul(v0, param1 << FX32_SHIFT);
	v1 >>= FX32_SHIFT;

	return v1;
}


static void ScreenFade_02009F94(int param0, int* param1, int param2, int param3) {
	int v0;
	fx32 v1;
	fx32 v2, v3;
	int v4, v5;

	v1 = ScreenFade_02009F50(param0);

	for (v0 = param3; v0 < param2; v0++) {
		v3 = v0 << FX32_SHIFT;
		v2 = FX_Mul(v1, v3);
		v2 >>= FX32_SHIFT;
		*(param1 + v0) = v2;
	}
}


static int ScreenFade_02009FE8(int param0, int param1) {
	fx32 v0;
	fx32 v1;
	int v2;

	v0 = ScreenFade_02009F50(param0);
	v1 = (param1 / 2) << FX32_SHIFT;
	v2 = FX_Div(v1, v0);

	return v2;
}


static int ScreenFade_0200A000(int param0, int param1, int param2) {
	int v0 = param1 - param0;
	v0 *= 128;
	v0 /= param2;

	return v0;
}


static int ScreenFade_0200A010(int param0, int param1) {
	int v0 = param0 + param1;

	if (v0 < 0) {
		v0 = 0;
	}

	if (v0 > 255) {
		v0 = 255;
	}

	return v0;
}


static void ScreenFade_0200A020(ScreenFadeFuncStruct2* param0, ScreenFadeFuncStruct2* param1) {
	param0->unk_00 += param1->unk_00;
	param0->unk_04 += param1->unk_04;
	param0->unk_08 += param1->unk_08;
	param0->unk_0C += param1->unk_0C;
}


static void ScreenFade_0200A044(ScreenFadeFuncStruct2* param0, ScreenFadeFuncStruct2* param1, ScreenFadeFuncStruct2* param2, const ScreenFadeFuncStruct2* param3, const ScreenFadeFuncStruct2* param4, int param5) {
	param0->unk_00 = param3->unk_00 * 128;
	param0->unk_04 = param3->unk_04 * 128;
	param0->unk_08 = param3->unk_08 * 128;
	param0->unk_0C = param3->unk_0C * 128;
	
	*param1 = *param4;
	
	param2->unk_00 = ScreenFade_0200A000(param3->unk_00, param4->unk_00, param5);
	param2->unk_04 = ScreenFade_0200A000(param3->unk_04, param4->unk_04, param5);
	param2->unk_08 = ScreenFade_0200A000(param3->unk_08, param4->unk_08, param5);
	param2->unk_0C = ScreenFade_0200A000(param3->unk_0C, param4->unk_0C, param5);
}


static void ScreenFade_0200A0A8(ScreenFade* param0, int param1) {
	int v0, v1;
	ScreenFadeFuncStruct1 *v2;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct1));
	memset(param0->data, 0, sizeof(ScreenFadeFuncStruct1));
	v2 = param0->data;

	if (param1 == 0) {
		if (param0->color == 0x7fff) {
			v0 = 16;
			v1 = 0;
		} else if (param0->color == 0x0) {
			v0 = -16;
			v1 = 0;
		} else {
			v0 = -16;
			v1 = 0;

			GF_ASSERT(FALSE);
		}
	} else {
		if (param0->color == 0x7fff) {
			v0 = 0;
			v1 = 16;
		} else if (param0->color == 0x0) {
			v0 = 0;
			v1 = -16;
		} else {
			v0 = 0;
			v1 = -16;

			GF_ASSERT(FALSE);
		}
	}

	SetScreenMasterBrightness(param0->screen, v0);

	v2->unk_00 = param0->steps;
	v2->unk_04 = param0->framesPerStep;
	v2->unk_08 = 0;
	v2->unk_0C = v0 * 128;
	v2->unk_10 = v1 * 128;
	v2->unk_14 = ScreenFade_0200A000(v0, v1, param0->steps);
	v2->screen = param0->screen;

	param0->state++;
}


static BOOL ScreenFade_0200A148(ScreenFade* param0) {
	ScreenFadeFuncStruct1 *v0 = param0->data;
	BOOL v1;
	BOOL v2 = 0;

	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200A188(v0);

			if (v1 == 1) {
				param0->state++;
			}
			break;
		case 2:
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		case 3:
			v2 = 1;
			break;
		default:
			break;
	}

	return v2;
}


static BOOL ScreenFade_0200A188(ScreenFadeFuncStruct1* param0) {
	BOOL v0 = FALSE;

	param0->unk_08++;

	if (param0->unk_08 >= param0->unk_04) {
		param0->unk_08 = 0;

		if ((param0->unk_00 - 1) > 0) {
			param0->unk_00--;

			param0->unk_0C += param0->unk_14;
		} else {
			param0->unk_0C = param0->unk_10;
			v0 = 1;
		}

		SetScreenMasterBrightness(param0->screen, param0->unk_0C / 128);
	}

	return v0;
}


static inline void inline_0201035C_sub(int param0, int param1, int param2, int param3, int param4, DSScreen screen) {
	if (param4 == 0) {
		if (screen == DS_SCREEN_MAIN) {
			if (GX_IsHBlank()) {
				G2_SetWnd0Position(param0, param1, param2, param3);
			}
		} else {
			if (GX_IsHBlank()) {
				G2S_SetWnd0Position(param0, param1, param2, param3);
			}
		}
	} else {
		if (screen == DS_SCREEN_MAIN) {
			if (GX_IsHBlank()) {
				G2_SetWnd1Position(param0, param1, param2, param3);
			}
		} else {
			if (GX_IsHBlank()) {
				G2S_SetWnd1Position(param0, param1, param2, param3);
			}
		}
	}
}


static inline void inline_0201035C(ScreenFadeFuncStruct8* param0, int param1, int param2) {
	ScreenFadeFuncStruct7 *v0 = ScreenFade_0200A474(param0, param2);
	inline_0201035C_sub(v0->unk_00[0][param1], 0, v0->unk_00[1][param1], 192, v0->unk_600, param0->screen);
}


static void ScreenFade_0200A1CC(void* param0) {
	ScreenFadeFuncStruct8 *v0 = (ScreenFadeFuncStruct8*)param0;
	int v1;
	int v2;

	GF_ASSERT(param0);

	v1 = GX_GetVCount();

	if (v1 < 192) {
		v1++;

		if (v1 > 191) {
			v1 -= 192;
		}

		if (v0->unk_04 == 1) {
			inline_0201035C(v0, v1, 0);
		} else {
			inline_0201035C(v0, v1, 0);
			inline_0201035C(v0, v1, 1);
		}
	}
}


static void ScreenFade_0200A3F8(ScreenFadeFuncStruct8* param0, int param1, DSScreen screen, HeapID heapID) {
	switch (param1) {
		case 0:
		case 1:
			param0->unk_00 = Heap_Alloc(heapID, sizeof(ScreenFadeFuncStruct7));
			param0->unk_04 = 1;
			param0->screen = screen;
			param0->unk_00->unk_600 = param1;
			break;
		case 2: {
			int v0;
			
			param0->unk_00 = Heap_Alloc(heapID, sizeof(ScreenFadeFuncStruct7) * 2);
			param0->unk_04 = 2;
			param0->screen = screen;

			for (v0 = 0; v0 < 2; v0++) {
				param0->unk_00[v0].unk_600 = v0;
			}
			break;
		}
		default:
			break;
	}
}


static void ScreenFade_0200A45C(ScreenFadeFuncStruct8* param0) {
	ScreenFade_0200A464(param0);
}


static void ScreenFade_0200A464(ScreenFadeFuncStruct8* param0) {
	Heap_Free(param0->unk_00);
	param0->unk_00 = NULL;
}


static ScreenFadeFuncStruct7 *ScreenFade_0200A474(ScreenFadeFuncStruct8* param0, int param1) {
	GF_ASSERT(param0->unk_04 > param1);
	return param0->unk_00 + param1;
}


static void ScreenFade_0200A494(SysTask* param0, void* param1) {
	ScreenFadeFuncStruct8 *v0 = (ScreenFadeFuncStruct8*)param1;
	ScreenFadeFuncStruct7 *v1;
	int v2;

	for (v2 = 0; v2 < v0->unk_04; v2++) {
		v1 = ScreenFade_0200A474(v0, v2);
		memcpy(v1->unk_00, v1->unk_300, sizeof(short) * 2 * 192);
	}

	SysTask_Done(param0);
}


static void ScreenFade_0200A4C8(int param0, HardwareWindowSettings* param1, DSScreen screen) {
	if (param0 == 0) {
		RequestVisibleHardwareWindows(param1, GX_WNDMASK_NONE, screen);
	} else {
		RequestVisibleHardwareWindows(param1, GX_WNDMASK_W0, screen);
		RequestHardwareWindowMaskInsidePlane(param1, GX_BLEND_ALL, 0, 0, screen);
		RequestHardwareWindowDimensions(param1, 0, 0, 0, 0, 0, screen);
		RequestHardwareWindowMaskOutsidePlane(param1, GX_BLEND_PLANEMASK_BD, 0, screen);
	}
}


static void ScreenFade_0200A518(HardwareWindowSettings* param0, int param1, int param2, int param3, DSScreen screen, int param5, int param6, int param7, int param8, int param9) {
	if (param9 == 0) {
		SetHardwareWindowMaskInsidePlane(param1, 0, param3, screen);
		SetHardwareWindowMaskOutsidePlane(param2, 0, screen);
		SetHardwareWindowDimensions(param5, param6, param7, param8, param3, screen);
	} else {
		RequestHardwareWindowMaskInsidePlane(param0, param1, 0, param3, screen);
		RequestHardwareWindowMaskOutsidePlane(param0, param2, 0, screen);
		RequestHardwareWindowDimensions(param0, param5, param6, param7, param8, param3, screen);
	}
}


static void ScreenFade_0200A580(HardwareWindowSettings* param0, int param1, DSScreen screen, int param3) {
	if (param3 == 0) {
		SetVisibleHardwareWindows(param1, screen);
	} else {
		RequestVisibleHardwareWindows(param0, param1, screen);
	}
}


static void ScreenFade_0200A598(ScreenFadeFuncStruct25* param0, DSScreen screen, int param2, int param3, int param4) {
	memset(param0, 0, sizeof(ScreenFadeFuncStruct25));

	if (param2 == 1) {
		param0->unk_00[0].unk_180 = param3;
		param0->unk_308 = param2;
		param0->screen = screen;
	} else {
		param0->unk_00[0].unk_180 = param3;
		param0->unk_00[1].unk_180 = param4;
		param0->unk_308 = param2;
		param0->screen = screen;
	}
}


static void ScreenFade_0200A5DC(ScreenFadeFuncStruct25* param0) {
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A61C, param0, 1023);
}


static void ScreenFade_0200A5F4(ScreenFadeHBlanks* param0, ScreenFadeFuncStruct25* param1, u32 heapID) {
	RequestEnableScreenHBlank(param0, param1, ScreenFade_0200A648, param1->screen, heapID);
}


static void ScreenFade_0200A60C(ScreenFadeHBlanks* param0, ScreenFadeFuncStruct25* param1, u32 heapID) {
	RequestDisableScreenHBlank(param0, param1->screen, heapID);
}


static void ScreenFade_0200A61C(SysTask* param0, void* param1) {
	ScreenFadeFuncStruct25 *v0 = param1;
	int v1;

	for (v1 = 0; v1 < 2; v1++) {
		memcpy(v0->unk_00[v1].unk_C0, v0->unk_00[v1].unk_00, sizeof(u8) * 192);
	}

	SysTask_Done(param0);
}


static inline void inline_020107D8_sub(int param0, BOOL param1, DSScreen screen) {
	if (screen == DS_SCREEN_MAIN) {
		if (GX_IsHBlank()) {
			G2_SetWndOutsidePlane(param0, param1);
		}
	} else {
		if (GX_IsHBlank()) {
			G2S_SetWndOutsidePlane(param0, param1);
		}
	}
}


static inline void inline_020107D8_plsub_1(int param0, BOOL param1, int param4, DSScreen screen) {
	if (param4 == 0) {
		if (screen == DS_SCREEN_MAIN) {
			if (GX_IsHBlank()) {
				G2_SetWnd0InsidePlane(param0, param1);
			}
		} else {
			if (GX_IsHBlank()) {
				G2S_SetWnd0InsidePlane(param0, param1);
			}
		}
	} else {
		if (screen == DS_SCREEN_MAIN) {
			if (GX_IsHBlank()) {
				G2_SetWnd1InsidePlane(param0, param1);
			}
		} else {
			if (GX_IsHBlank()) {
				G2S_SetWnd1InsidePlane(param0, param1);
			}
		}
	}
}


static inline void inline_020107D8(ScreenFadeFuncStruct25* param0, int param1, int param2) {
	ScreenFadeFuncStruct24 *v0 = &param0->unk_00[param2];

	if (v0->unk_C0[param1] == 0) {
		inline_020107D8_sub(GX_BLEND_ALL, 1, param0->screen);
		inline_020107D8_plsub_1(GX_BLEND_PLANEMASK_BD, 1, v0->unk_180, param0->screen);
	} else {
		inline_020107D8_sub(GX_BLEND_PLANEMASK_BD, 1, param0->screen);
		inline_020107D8_plsub_1(GX_BLEND_ALL, 1, v0->unk_180, param0->screen);
	}
}


static void ScreenFade_0200A648(void* param0) {
	ScreenFadeFuncStruct25 *v0 = (ScreenFadeFuncStruct25*)param0;
	int v1;
	int v2;

	GF_ASSERT(param0);

	v1 = GX_GetVCount();

	if (v1 < 192) {
		v1++;

		if (v1 > 191) {
			v1 -= 192;
		}

		if (v0->unk_308 == 1) {
			inline_020107D8(v0, v1, 0);
		} else {
			inline_020107D8(v0, v1, 0);
			inline_020107D8(v0, v1, 1);
		}
	}
}


static void ScreenFade_0200AB64(ScreenFade* param0, const ScreenFadeFuncStruct5* param1) {
	ScreenFadeFuncStruct4 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct4));

	v0 = (ScreenFadeFuncStruct4*)param0->data;

	ScreenFade_0200ACBC(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings);

	if (param1->unk_08 == 0) {
		ScreenFade_0200A580(param0->hwSettings, GX_WNDMASK_W0, v0->screen, v0->unk_44);
	} else {
		ScreenFade_0200A580(param0->hwSettings, GX_WNDMASK_W1, v0->screen, v0->unk_44);
	}

	param0->state++;
}


static BOOL ScreenFade_0200ABB8(ScreenFade* param0) {
	ScreenFadeFuncStruct4 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct4*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200AD2C(v0);

			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_44, param0->hwSettings, param0->screen);
				param0->state++;
			}
			break;
		case 2:
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			break;
	}
	
	return v2;
}


static void ScreenFade_0200AC08(ScreenFade* param0, const ScreenFadeFuncStruct5* param1, const ScreenFadeFuncStruct5* param2) {
	ScreenFadeFuncStruct6 *v0;
	
	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct6));
	v0 = (ScreenFadeFuncStruct6*)param0->data;
	
	ScreenFade_0200ACBC(&v0->unk_00, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings);
	ScreenFade_0200ACBC(&v0->unk_4C, param2, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings);
	ScreenFade_0200A580(param0->hwSettings, GX_WNDMASK_W0 | GX_WNDMASK_W1, param0->screen, v0->unk_00.unk_44);
	
	param0->state++;
}


static BOOL ScreenFade_0200AC60(ScreenFade* param0) {
	ScreenFadeFuncStruct6 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct6*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200AD2C(&v0->unk_00);
			v1 += ScreenFade_0200AD2C(&v0->unk_4C);
			
			if (v1 == 2) {
				ScreenFade_0200A4C8(v0->unk_00.unk_44, param0->hwSettings, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
	}
	
	return v2;
}


static void ScreenFade_0200ACBC(ScreenFadeFuncStruct4* param0, const ScreenFadeFuncStruct5* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5) {
	ScreenFade_0200A044(&param0->unk_00, &param0->unk_20, &param0->unk_10, &param1->unk_00, &param1->unk_04, param2);

	param0->screen = screen;
	param0->unk_34 = param1->unk_08;
	param0->unk_38 = param2;
	param0->unk_3C = param3;
	param0->unk_40 = 0;
	param0->unk_48 = param5;
	param0->unk_44 = param1->unk_0B;

	ScreenFade_0200A518(param5, param1->unk_09, param1->unk_0A, param1->unk_08, screen, param1->unk_00.unk_00, param1->unk_00.unk_04, param1->unk_00.unk_08, param1->unk_00.unk_0C, param0->unk_44);
}


static BOOL ScreenFade_0200AD2C(ScreenFadeFuncStruct4* param0) {
	param0->unk_40++;

	if (param0->unk_40 >= param0->unk_3C) {
		param0->unk_40 = 0;

		if ((param0->unk_38 - 1) > 0) {
			param0->unk_38--;
			ScreenFade_0200A020(&param0->unk_00, &param0->unk_10);
		} else {
			RequestHardwareWindowDimensions(param0->unk_48, param0->unk_20.unk_00, param0->unk_20.unk_04, param0->unk_20.unk_08, param0->unk_20.unk_0C, param0->unk_34, param0->screen);
			return 1;
		}

		RequestHardwareWindowDimensions(param0->unk_48, param0->unk_00.unk_00 / 128, param0->unk_00.unk_04 / 128, param0->unk_00.unk_08 / 128, param0->unk_00.unk_0C / 128, param0->unk_34, param0->screen);
	}

	return 0;
}


static void ScreenFade_0200ADB4(ScreenFade* param0, const ScreenFadeFuncStruct10* param1) {
	ScreenFadeFuncStruct9 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct9));
	v0 = (ScreenFadeFuncStruct9*)param0->data;

	ScreenFade_0200AE48(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);

	param0->state++;
}


static BOOL ScreenFade_0200ADEC(ScreenFade* param0) {
	ScreenFadeFuncStruct9 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct9*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200AF20(v0);
			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_2C, v0->unk_30, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200A45C(&v0->unk_00);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200AE48(ScreenFadeFuncStruct9* param0, const ScreenFadeFuncStruct10* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	int v0;
	ScreenFadeFuncStruct7* v1;
	
	v0 = ScreenFade_0200A000(param1->unk_00, param1->unk_04, param2);
	ScreenFade_0200A3F8(&param0->unk_00, param1->unk_0C, screen, heapID);
	
	param0->unk_0C = param1->unk_00 * 128;
	param0->unk_10 = param1->unk_08;
	param0->unk_14 = param1->unk_0A;
	param0->unk_18 = v0;
	param0->unk_1C = param2;
	param0->unk_20 = param3;
	param0->unk_24 = 0;
	param0->unk_30 = param5;
	param0->unk_34 = param6;
	param0->heapID = heapID;
	param0->unk_2C = param1->unk_0F;
	
	ScreenFade_0200B004(param0);
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
	
	v1 = ScreenFade_0200A474(&param0->unk_00, 0);
	ScreenFade_0200A518(param5, param1->unk_0D, param1->unk_0E, param1->unk_0C, screen, v1->unk_300[0][0], 0, v1->unk_300[1][0], 192, param0->unk_2C);
	
	if (param1->unk_0C == 0) {
		ScreenFade_0200A580(param5, GX_WNDMASK_W0, screen, param0->unk_2C);
	} else {
		ScreenFade_0200A580(param5, GX_WNDMASK_W1, screen, param0->unk_2C);
	}
	
	RequestEnableScreenHBlank(param0->unk_34, &param0->unk_00, ScreenFade_0200A1CC, screen, heapID);
}


static BOOL ScreenFade_0200AF20(ScreenFadeFuncStruct9* param0) {
	param0->unk_24++;

	if (param0->unk_24 >= param0->unk_20) {
		param0->unk_24 = 0;

		if ((param0->unk_1C - 1) > 0) {
			param0->unk_1C--;
			param0->unk_0C += param0->unk_18;
			ScreenFade_0200B004(param0);
			SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
		} else {
			RequestDisableScreenHBlank(param0->unk_34, param0->unk_00.screen, param0->heapID);
			return 1;
		}
	}

	return 0;
}


static void ScreenFade_0200AF70(int param0, int param1, int param2, int param3, int* param4, int* param5) {
	fx32 v0;
	fx32 v1;
	fx32 v2;

	v0 = param0 / 128;
	v1 = param3 - param2;

	if (v1 < 0) {
		v1 = -v1;
	}

	if (v1 >= v0) {
		*param4 = 0;
		*param5 = 0;
	} else {
		v1 <<= FX32_SHIFT;
		v0 <<= FX32_SHIFT;
		v2 = FX_Sqrt(FX_Mul(v0, v0) - FX_Mul(v1, v1));
		v2 >>= FX32_SHIFT;

		*param4 = param1 - v2;

		if (*param4 < 0) {
			*param4 = 0;
		}

		*param5 = *param4 + (v2 * 2);

		if (*param5 > 255) {
			*param5 = 255;
		}
	}
}


static void ScreenFade_0200B004(ScreenFadeFuncStruct9* param0) {
	ScreenFadeFuncStruct8 *v0 = &param0->unk_00;
	int v1;
	int v2;
	int v3;
	int v4;
	ScreenFadeFuncStruct7 *v5 = ScreenFade_0200A474(v0, 0);

	for (v1 = 0; v1 < 192; v1++) {
		if (v1 <= param0->unk_14) {
			ScreenFade_0200AF70(param0->unk_0C, param0->unk_10, param0->unk_14, v1, &v2, &v3);
		} else {
			if (v1 <= (param0->unk_14 * 2)) {
				v2 = v5->unk_300[0][(param0->unk_14 * 2) - v1];
				v3 = v5->unk_300[1][(param0->unk_14 * 2) - v1];
			} else {
				ScreenFade_0200AF70(param0->unk_0C, param0->unk_10, param0->unk_14, v1, &v2, &v3);
			}
		}

		v5->unk_300[0][v1] = v2;
		v5->unk_300[1][v1] = v3;
	}
}


static void ScreenFade_0200B088(ScreenFade* param0, const ScreenFadeFuncStruct12* param1) {
	ScreenFadeFuncStruct11 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct11));
	v0 = (ScreenFadeFuncStruct11*)param0->data;

	ScreenFade_0200B11C(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);
	param0->state++;
}


static BOOL ScreenFade_0200B0C0(ScreenFade* param0) {
	ScreenFadeFuncStruct11 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct11*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200B1E4(v0);

			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_20, v0->unk_24, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200A45C(&v0->unk_00);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200B11C(ScreenFadeFuncStruct11* param0, const ScreenFadeFuncStruct12* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	ScreenFadeFuncStruct7 *v0;
	
	param0->unk_10 = ScreenFade_0200A000(param1->unk_00, param1->unk_04, param2);
	ScreenFade_0200A3F8(&param0->unk_00, param1->unk_08, screen, heapID);
	
	param0->unk_0C = param1->unk_00 * 128;
	param0->unk_14 = param2;
	param0->unk_18 = param3;
	param0->unk_1C = 0;
	param0->unk_24 = param5;
	param0->unk_28 = param6;
	param0->heapID = heapID;
	param0->unk_20 = param1->unk_0B;
	
	ScreenFade_0200B234(param0);
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
	
	v0 = ScreenFade_0200A474(&param0->unk_00, 0);
	ScreenFade_0200A518(param5, param1->unk_09, param1->unk_0A, param1->unk_08, screen, v0->unk_300[0][0], 0, v0->unk_300[1][0], 192, param0->unk_20);
	
	if (param1->unk_08 == 0) {
		ScreenFade_0200A580(param5, GX_WNDMASK_W0, screen, param0->unk_20);
	} else {
		ScreenFade_0200A580(param5, GX_WNDMASK_W1, screen, param0->unk_20);
	}
	
	RequestEnableScreenHBlank(param0->unk_28, &param0->unk_00, ScreenFade_0200A1CC, screen, heapID);
}


static BOOL ScreenFade_0200B1E4(ScreenFadeFuncStruct11* param0) {
	param0->unk_1C++;

	if (param0->unk_1C >= param0->unk_18) {
		param0->unk_1C = 0;

		if ((param0->unk_14 - 1) > 0) {
			param0->unk_14--;
			param0->unk_0C += param0->unk_10;
			ScreenFade_0200B234(param0);
			SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
		} else {
			RequestDisableScreenHBlank(param0->unk_28, param0->unk_00.screen, param0->heapID);
			return 1;
		}
	}

	return 0;
}


static void ScreenFade_0200B234(ScreenFadeFuncStruct11* param0) {
	int v0;
	int v1, v2;
	int v3[192];
	ScreenFadeFuncStruct7 *v4 = ScreenFade_0200A474(&param0->unk_00, 0);
	ScreenFade_02009F94(param0->unk_0C / 128, v3, 192, 0);
	
	for (v0 = 0; v0 < 192; v0++) {
		v4->unk_300[0][v0] = ScreenFade_0200A010(128, -v3[v0]);
		v4->unk_300[1][v0] = ScreenFade_0200A010(128, v3[v0]);
	}
}


static void ScreenFade_0200B28C(ScreenFade* param0, const ScreenFadeFuncStruct14* param1) {
	ScreenFadeFuncStruct13 *v0;
	
	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct13));
	v0 = (ScreenFadeFuncStruct13*)param0->data;
	
	ScreenFade_0200B318(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);
	param0->state++;
}


static BOOL ScreenFade_0200B2C4(ScreenFade* param0) {
	ScreenFadeFuncStruct13 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct13*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200B3EC(v0);
			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_24, v0->unk_28, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200A45C(&v0->unk_00);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
	}
	
	return v2;
}


static void ScreenFade_0200B318(ScreenFadeFuncStruct13* param0, const ScreenFadeFuncStruct14* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	int v0;
	ScreenFadeFuncStruct7 *v1;

	v0 = (param1->unk_04 - param1->unk_00);
	v0 /= param2;

	ScreenFade_0200A3F8(&param0->unk_00, param1->unk_08, screen, heapID);

	param0->unk_0C = 128 * FX32_ONE;
	param0->unk_10 = param1->unk_00;
	param0->unk_14 = v0;
	param0->unk_18 = param2;
	param0->unk_1C = param3;
	param0->unk_20 = 0;
	param0->unk_28 = param5;
	param0->unk_2C = param6;
	param0->heapID = heapID;
	param0->unk_24 = param1->unk_0B;

	ScreenFade_0200B43C(param0);
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);

	v1 = ScreenFade_0200A474(&param0->unk_00, 0);
	ScreenFade_0200A518(param5, param1->unk_09, param1->unk_0A, param1->unk_08, screen, v1->unk_300[0][96], 0, v1->unk_300[1][96], 192, param0->unk_24);

	if (param1->unk_08 == 0) {
		ScreenFade_0200A580(param5, GX_WNDMASK_W0, screen, param0->unk_24);
	} else {
		ScreenFade_0200A580(param5, GX_WNDMASK_W1, screen, param0->unk_24);
	}

	RequestEnableScreenHBlank(param0->unk_2C, &param0->unk_00, ScreenFade_0200A1CC, screen, heapID);
}


static BOOL ScreenFade_0200B3EC(ScreenFadeFuncStruct13* param0) {
	param0->unk_20++;

	if (param0->unk_20 >= param0->unk_1C) {
		param0->unk_20 = 0;

		if ((param0->unk_18 - 1) > 0) {
			param0->unk_18--;
			param0->unk_10 += param0->unk_14;
			ScreenFade_0200B43C(param0);
			SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
		} else {
			RequestDisableScreenHBlank(param0->unk_2C, param0->unk_00.screen, param0->heapID);
			return 1;
		}
	}

	return 0;
}


static void ScreenFade_0200B43C(ScreenFadeFuncStruct13* param0) {
	int v0;
	int v1;
	int v2;
	int v3[192];
	int v4;
	int v5;
	int v6;
	int v7, v8;
	ScreenFadeFuncStruct7 *v9 = ScreenFade_0200A474(&param0->unk_00, 0);
	v5 = FX_Mul(FX_SinIdx(param0->unk_10), param0->unk_0C);

	v5 >>= FX32_SHIFT;

	v2 = v5 * 2;
	v2 = v2 / 21;
	v2 += 1;
	v2 = 180 - (v2 * 2);
	v2 = ((0xFFFF * (v2)) / 360);
	v2 /= 2;
	v1 = ScreenFade_02009FE8(v2, 256);
	v1 >>= FX32_SHIFT;

	GF_ASSERT(v1 < 192);

	ScreenFade_02009F94(v2, v3, v1, 0);

	for (v0 = 0; v0 < 96; v0++) {
		v4 = v1 - (v0 + 1);
		v6 = v5;

		if (v4 > 0) {
			if (v3[v4] > v6) {
				v6 = v3[v4];
			}
		}

		v7 = ScreenFade_0200A010(128, -v6);
		v8 = ScreenFade_0200A010(128, v6);

		v9->unk_300[0][v0] = v7;
		v9->unk_300[1][v0] = v8;
		v9->unk_300[0][191 - v0] = v7;
		v9->unk_300[1][191 - v0] = v8;
	}
}


static void ScreenFade_0200B524(ScreenFade* param0, const ScreenFadeFuncStruct17* param1) {
	ScreenFadeFuncStruct16 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct16));
	v0 = (ScreenFadeFuncStruct16*)param0->data;

	ScreenFade_0200B5BC(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);
	param0->state++;
}


static BOOL ScreenFade_0200B55C(ScreenFade* param0) {
	ScreenFadeFuncStruct16 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct16*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200B6D0(v0);
			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_20, v0->unk_24, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200B720(v0);
			ScreenFade_0200A45C(&v0->unk_00);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200B5BC(ScreenFadeFuncStruct16* param0, const ScreenFadeFuncStruct17* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	int v0;
	ScreenFadeFuncStruct7 *v1;
	
	param0->unk_0C = Heap_Alloc(heapID, sizeof(ScreenFadeFuncStruct15) * param1->unk_08);
	GF_ASSERT(param0->unk_0C != NULL);
	param0->unk_10 = param1->unk_08;
	
	for (v0 = 0; v0 < param1->unk_08; v0++) {
		ScreenFade_0200A044(&param0->unk_0C[v0].unk_00, &param0->unk_0C[v0].unk_20, &param0->unk_0C[v0].unk_10, (param1->unk_00 + v0), (param1->unk_04 + v0), param2);
	}
	
	ScreenFade_0200A3F8(&param0->unk_00, param1->unk_09, screen, heapID);
	
	param0->unk_14 = param2;
	param0->unk_18 = param3;
	param0->unk_1C = 0;
	param0->unk_24 = param5;
	param0->unk_28 = param6;
	param0->heapID = heapID;
	param0->unk_20 = param1->unk_0C;
	
	ScreenFade_0200B730(param0);
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
	
	v1 = ScreenFade_0200A474(&param0->unk_00, 0);
	ScreenFade_0200A518(param5, param1->unk_0A, param1->unk_0B, param1->unk_09, screen, v1->unk_300[0][0], 0, v1->unk_300[1][0], 192, param0->unk_20);
	
	if (param1->unk_09 == 0) {
		ScreenFade_0200A580(param0->unk_24, GX_WNDMASK_W0, screen, param0->unk_20);
	} else {
		ScreenFade_0200A580(param0->unk_24, GX_WNDMASK_W1, screen, param0->unk_20);
	}
	
	RequestEnableScreenHBlank(param0->unk_28, &param0->unk_00, ScreenFade_0200A1CC, screen, heapID);
}


static BOOL ScreenFade_0200B6D0(ScreenFadeFuncStruct16* param0) {
	param0->unk_1C++;
	
	if (param0->unk_1C >= param0->unk_18) {
		param0->unk_1C = 0;
		
		if ((param0->unk_14 - 1) > 0) {
			param0->unk_14--;
			ScreenFade_0200B7BC(param0);
			ScreenFade_0200B730(param0);
			SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
		} else {
			RequestDisableScreenHBlank(param0->unk_28, param0->unk_00.screen, param0->heapID);
			return 1;
		}
	}
	
	return 0;
}


static void ScreenFade_0200B720(ScreenFadeFuncStruct16* param0) {
	Heap_Free(param0->unk_0C);
	param0->unk_0C = NULL;
}


static void ScreenFade_0200B730(ScreenFadeFuncStruct16* param0) {
	int v0;
	ScreenFadeFuncStruct7 *v1 = ScreenFade_0200A474(&param0->unk_00, 0);
	memset(v1->unk_300, 0, 768);

	for (v0 = (param0->unk_10 - 1); v0 >= 0; v0--) {
		ScreenFade_0200B764(&param0->unk_00, &param0->unk_0C[v0].unk_00);
	}
}


static void ScreenFade_0200B764(ScreenFadeFuncStruct8* param0, ScreenFadeFuncStruct2* param1) {
	int v0;
	ScreenFadeFuncStruct7 *v1;
	ScreenFadeFuncStruct2 v2;

	v1 = ScreenFade_0200A474(param0, 0);

	v2.unk_00 = param1->unk_00 / 128;
	v2.unk_04 = param1->unk_04 / 128;
	v2.unk_08 = param1->unk_08 / 128;
	v2.unk_0C = param1->unk_0C / 128;

	for (v0 = v2.unk_04; v0 < v2.unk_0C; v0++) {
		v1->unk_300[0][v0] = v2.unk_00;
		v1->unk_300[1][v0] = v2.unk_08;
	}
}


static void ScreenFade_0200B7BC(ScreenFadeFuncStruct16* param0) {
	int v0;

	for (v0 = 0; v0 < param0->unk_10; v0++) {
		ScreenFade_0200A020(&param0->unk_0C[v0].unk_00, &param0->unk_0C[v0].unk_10);
	}
}


static void ScreenFade_0200B7E4(ScreenFade* param0, ScreenFadeFuncStruct20* param1) {
	ScreenFadeFuncStruct19 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct19));
	memset(param0->data, 0, sizeof(ScreenFadeFuncStruct19));

	v0 = param0->data;
	ScreenFade_0200B884(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);

	param0->state++;
}


static BOOL ScreenFade_0200B824(ScreenFade* param0) {
	ScreenFadeFuncStruct19 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct19*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200B980(v0);
			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_28, v0->unk_30, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200B9D8(v0);
			ScreenFade_0200A45C(&v0->unk_00);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200B884(ScreenFadeFuncStruct19* param0, ScreenFadeFuncStruct20* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	ScreenFadeFuncStruct7 *v0;
	ScreenFadeFuncStruct7 *v1;

	param0->unk_0C.unk_00 = 0;
	param0->unk_0C.unk_04 = param1->unk_00;
	param0->unk_0C.unk_08 = param1->unk_04 - param1->unk_00;

	ScreenFade_0200A3F8(&param0->unk_00, 2, screen, heapID);

	param0->unk_18 = param2;
	param0->unk_1C = 0;
	param0->unk_20 = param3;
	param0->unk_24 = 0;
	param0->unk_30 = param5;
	param0->unk_34 = param6;
	param0->heapID = heapID;
	param0->unk_28 = param1->unk_0A;

	ScreenFade_0200BB00(&param0->unk_0C, param0->unk_1C, param0->unk_18);
	ScreenFade_0200B9DC(param0);
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);

	v0 = ScreenFade_0200A474(&param0->unk_00, 0);
	v1 = ScreenFade_0200A474(&param0->unk_00, 1);

	ScreenFade_0200A518(param5, param1->unk_08, param1->unk_09, 0, screen, v0->unk_300[0][0], 0, v0->unk_300[1][0], 192, param0->unk_28);
	ScreenFade_0200A518(param5, param1->unk_08, param1->unk_09, 1, screen, v1->unk_300[0][0], 0, v1->unk_300[1][0], 192, param0->unk_28);
	ScreenFade_0200A580(param5, GX_WNDMASK_W0 | GX_WNDMASK_W1, screen, param0->unk_28);
	RequestEnableScreenHBlank(param0->unk_34, &param0->unk_00, ScreenFade_0200A1CC, screen, heapID);
}


static BOOL ScreenFade_0200B980(ScreenFadeFuncStruct19* param0) {
	param0->unk_24++;

	if (param0->unk_24 >= param0->unk_20) {
		param0->unk_24 = 0;

		if ((param0->unk_1C + 1) <= param0->unk_18) {
			param0->unk_1C++;

			ScreenFade_0200BB00(&param0->unk_0C, param0->unk_1C, param0->unk_18);
			ScreenFade_0200B9DC(param0);
			SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
		} else {
			RequestDisableScreenHBlank(param0->unk_34, param0->unk_00.screen, param0->heapID);
			return 1;
		}
	}

	return 0;
}


static void ScreenFade_0200B9D8(ScreenFadeFuncStruct19* param0) {
	return;
}


static void ScreenFade_0200B9DC(ScreenFadeFuncStruct19* param0) {
	ScreenFadeFuncStruct7 *v0;
	ScreenFadeFuncStruct7 *v1;
	u16 v2;
	int v3, v4;
	int v5;

	v2 = param0->unk_0C.unk_00 % ((90 * 0xFFFF) / 360);
	v0 = ScreenFade_0200A474(&param0->unk_00, 0);
	v1 = ScreenFade_0200A474(&param0->unk_00, 1);

	for (v5 = 0; v5 < 96; v5++) {
		if (param0->unk_0C.unk_00 < ((90 * 0xFFFF) / 360)) {
			v3 = 128;
			v4 = ScreenFade_02009F6C(v2, (96 - v5));

			if (v4 > 127) {
				v4 = 127;
			}

			v0->unk_300[0][191 - v5] = v3 - v4;
			v0->unk_300[1][191 - v5] = v3;

			v1->unk_300[0][v5] = v3;
			v1->unk_300[1][v5] = v3 + v4;
		} else {
			v0->unk_300[0][191 - v5] = 0;
			v0->unk_300[1][191 - v5] = 128;

			v1->unk_300[0][v5] = 128;
			v1->unk_300[1][v5] = 255;
		}
	}

	for (v5 = 96; v5 < 192; v5++) {
		if (param0->unk_0C.unk_00 < ((90 * 0xFFFF) / 360)) {
			v0->unk_300[0][191 - v5] = 128;
			v0->unk_300[1][191 - v5] = 128;

			v1->unk_300[0][v5] = 128;
			v1->unk_300[1][v5] = 128;
		} else {
			v3 = ScreenFade_02009F6C(((90 * 0xFFFF) / 360) - v2, (v5 - 96));

			if (v3 > 127) {
				v3 = 127;
			}

			v0->unk_300[0][191 - v5] = 0;
			v0->unk_300[1][191 - v5] = 128 - v3;

			v1->unk_300[0][v5] = 128 + v3;
			v1->unk_300[1][v5] = 255;
		}
	}
}


static void ScreenFade_0200BB00(ScreenFadeFuncStruct18* param0, int param1, int param2) {
	int v0 = param0->unk_08 * param1;
	v0 = v0 / param2;

	param0->unk_00 = v0 + param0->unk_04;
}


static void ScreenFade_0200BB18(ScreenFade* param0, ScreenFadeFuncStruct23* param1) {
	ScreenFadeFuncStruct22 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct22));
	memset(param0->data, 0, sizeof(ScreenFadeFuncStruct22));

	v0 = param0->data;
	ScreenFade_0200BBB8(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);

	param0->state++;
}


static BOOL ScreenFade_0200BB58(ScreenFade* param0) {
	ScreenFadeFuncStruct22 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct22*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200BC88(v0);
			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_28, v0->unk_30, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200BCE0(v0);
			ScreenFade_0200A45C(&v0->unk_00);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200BBB8(ScreenFadeFuncStruct22* param0, ScreenFadeFuncStruct23* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	ScreenFadeFuncStruct7 *v0;
	ScreenFadeFuncStruct7 *v1;
	
	param0->unk_0C.unk_00 = param1->unk_00;
	param0->unk_0C.unk_04 = param1->unk_00;
	param0->unk_0C.unk_08 = param1->unk_04 - param1->unk_00;
	
	ScreenFade_0200A3F8(&param0->unk_00, 2, screen, heapID);
	
	param0->unk_18 = param2;
	param0->unk_1C = 0;
	param0->unk_20 = param3;
	param0->unk_24 = 0;
	param0->unk_30 = param5;
	param0->unk_34 = param6;
	param0->heapID = heapID;
	param0->unk_28 = param1->unk_0A;
	
	ScreenFade_0200BCE4(param0);
	SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
	
	v0 = ScreenFade_0200A474(&param0->unk_00, 0);
	v1 = ScreenFade_0200A474(&param0->unk_00, 1);
	
	ScreenFade_0200A518(param5, param1->unk_08, param1->unk_09, 0, screen, 0, 0, 255, 192, param0->unk_28);
	ScreenFade_0200A518(param5, param1->unk_08, param1->unk_09, 1, screen, 0, 0, 255, 192, param0->unk_28);
	ScreenFade_0200A580(param5, GX_WNDMASK_W0 | GX_WNDMASK_W1, screen, param0->unk_28);
	RequestEnableScreenHBlank(param0->unk_34, &param0->unk_00, ScreenFade_0200A1CC, screen, heapID);
}


static BOOL ScreenFade_0200BC88(ScreenFadeFuncStruct22* param0) {
	param0->unk_24++;

	if (param0->unk_24 >= param0->unk_20) {
		param0->unk_24 = 0;

		if ((param0->unk_1C + 1) <= param0->unk_18) {
			param0->unk_1C++;

			ScreenFade_0200BDB0(&param0->unk_0C, param0->unk_1C, param0->unk_18);
			ScreenFade_0200BCE4(param0);
			SysTask_ExecuteAfterVBlank(ScreenFade_0200A494, &param0->unk_00, 1023);
		} else {
			RequestDisableScreenHBlank(param0->unk_34, param0->unk_00.screen, param0->heapID);
			return 1;
		}
	}

	return 0;
}


static void ScreenFade_0200BCE0(ScreenFadeFuncStruct22* param0) {
	return;
}


static void ScreenFade_0200BCE4(ScreenFadeFuncStruct22* param0) {
	ScreenFadeFuncStruct7 *v0;
	ScreenFadeFuncStruct7 *v1;
	u16 v2;
	int v3, v4;
	int v5;

	v2 = param0->unk_0C.unk_00;
	v0 = ScreenFade_0200A474(&param0->unk_00, 0);
	v1 = ScreenFade_0200A474(&param0->unk_00, 1);

	for (v5 = 0; v5 < 96; v5++) {
		v3 = ScreenFade_02009F6C(v2, (96 - v5));
		v4 = ScreenFade_02009F6C(((90 * 0xFFFF) / 360) - v2, (96 - v5));

		if (v3 > 127) {
			v3 = 127;
		}

		if (v4 > 127) {
			v4 = 127;
		}

		v0->unk_300[0][v5] = 128 - v4;
		v0->unk_300[1][v5] = 128 - v3;

		v0->unk_300[0][191 - v5] = 128 - v4;
		v0->unk_300[1][191 - v5] = 128 - v3;

		v1->unk_300[0][v5] = 128 + v3;
		v1->unk_300[1][v5] = 128 + v4;

		v1->unk_300[0][191 - v5] = 128 + v3;
		v1->unk_300[1][191 - v5] = 128 + v4;
	}
}


static void ScreenFade_0200BDB0(ScreenFadeFuncStruct21* param0, int param1, int param2) {
	int v0 = param0->unk_08 * param1;
	v0 = v0 / param2;

	param0->unk_00 = v0 + param0->unk_04;
}


static void ScreenFade_0200BDC8(ScreenFade* param0, ScreenFadeFuncStruct28* param1) {
	ScreenFadeFuncStruct27 *v0;
	
	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct27));
	memset(param0->data, 0, sizeof(ScreenFadeFuncStruct27));
	
	v0 = param0->data;
	ScreenFade_0200BE6C(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);
	
	param0->state++;
}


static BOOL ScreenFade_0200BE0C(ScreenFade* param0) {
	ScreenFadeFuncStruct27 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct27*)param0->data;
	
	switch (param0->state) {
		case 1:
			v1 = ScreenFade_0200BF58(v0);
			if (v1 == 1) {
				ScreenFade_0200A4C8(v0->unk_324, v0->unk_32C, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			ScreenFade_0200BFB8(v0);
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200BE6C(ScreenFadeFuncStruct27* param0, ScreenFadeFuncStruct28* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	ScreenFade_0200A598(&param0->unk_00, screen, 1, 0, 0);
	
	if (param1->unk_05 == 0) {
		memset(param0->unk_00.unk_00[0].unk_00, 1, sizeof(u8) * 192);
		memset(param0->unk_00.unk_00[0].unk_C0, 1, sizeof(u8) * 192);
	} else {
		memset(param0->unk_00.unk_00[0].unk_00, 0, sizeof(u8) * 192);
		memset(param0->unk_00.unk_00[0].unk_C0, 0, sizeof(u8) * 192);
	}
	
	param0->unk_30C = param1->unk_00;
	param0->unk_310 = param1->unk_04;
	param0->unk_324 = param1->unk_05;
	param0->heapID = heapID;
	param0->unk_314 = param2;
	param0->unk_318 = 0;
	param0->unk_31C = param3;
	param0->unk_320 = 0;
	param0->unk_32C = param5;
	param0->unk_330 = param6;
	
	ScreenFade_0200A5F4(param6, &param0->unk_00, heapID);
	
	if (param1->unk_05 == 1) {
		ScreenFade_0200A518(param5, GX_BLEND_PLANEMASK_BD, GX_BLEND_ALL, 0, screen, 0, 0, 0, 0, param1->unk_05);
	} else {
		ScreenFade_0200A518(param5, GX_BLEND_ALL, GX_BLEND_PLANEMASK_BD, 0, screen, 0, 0, 0, 0, param1->unk_05);
	}
	
	ScreenFade_0200A580(param5, GX_WNDMASK_W0, screen, param0->unk_324);
}


static BOOL ScreenFade_0200BF58(ScreenFadeFuncStruct27* param0) {
	param0->unk_320++;

	if (param0->unk_320 >= param0->unk_31C) {
		param0->unk_320 = 0;

		if ((param0->unk_318 + 1) <= param0->unk_314) {
			param0->unk_318++;

			ScreenFade_0200BFBC(param0);
			ScreenFade_0200A5DC(&param0->unk_00);
		} else {
			ScreenFade_0200A60C(param0->unk_330, &param0->unk_00, param0->heapID);
			return 1;
		}
	}

	return 0;
}


static void ScreenFade_0200BFB8(ScreenFadeFuncStruct27* param0) {
}


static void ScreenFade_0200BFBC(ScreenFadeFuncStruct27* param0) {
	int v0;
	ScreenFadeFuncStruct24 *v1;
	const ScreenFadeFuncStruct26 *v2;

	v1 = &param0->unk_00.unk_00[0];

	for (v0 = 0; v0 < param0->unk_310; v0++) {
		v2 = &param0->unk_30C[v0];
		ScreenFade_0200BFF8(v2, v1, param0->unk_318, param0->unk_314);
	}
}


static void ScreenFade_0200BFF8(const ScreenFadeFuncStruct26* param0, ScreenFadeFuncStruct24* param1, int param2, int param3) {
	int v0;
	int v1;
	int v2;
	int v3;
	int v4;
	int v5;

	v1 = (param0->unk_01 - param0->unk_00) * param2;
	v2 = v1 / param3;

	v2 += param0->unk_00;

	if (param0->unk_00 <= param0->unk_01) {
		v3 = param0->unk_00;
		v4 = param0->unk_01;
		v5 = param0->unk_02;
	} else {
		v3 = param0->unk_01;
		v4 = param0->unk_00;

		if (param0->unk_02 == 0) {
			v5 = 1;
		} else {
			v5 = 0;
		}
	}

	for (v0 = v3; v0 < v4; v0++) {
		if (v0 == v2) {
			if (v5 == 0) {
				v5 = 1;
			} else {
				v5 = 0;
			}
		}

		param1->unk_00[v0] = v5;
	}
}


static void ScreenFade_0200C048(ScreenFade* param0, ScreenFadeFuncStruct29* param1) {
	ScreenFadeFuncStruct30 *v0;

	param0->data = Heap_Alloc(param0->heapID, sizeof(ScreenFadeFuncStruct30));
	memset(param0->data, 0, sizeof(ScreenFadeFuncStruct30));

	v0 = param0->data;

	if (param1->unk_00.unk_0B == 0) {
		ScreenFade_0200C118(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);
	} else {
		ScreenFade_0200C218(v0, param1, param0->steps, param0->framesPerStep, param0->screen, param0->hwSettings, param0->hblanks, param0->heapID);
	}

	param0->state++;
}


static BOOL ScreenFade_0200C0B0(ScreenFade* param0) {
	ScreenFadeFuncStruct30 *v0;
	BOOL v1;
	BOOL v2 = 0;
	
	v0 = (ScreenFadeFuncStruct30*)param0->data;
	
	switch (param0->state) {
		case 1:
			if (v0->unk_386 == 0) {
				v1 = ScreenFade_0200C1A4(v0, param0);
			} else {
				v1 = ScreenFade_0200C288(v0, param0);
			}
			
			if (v1 == 1) {
				ScreenFade_0200A4C8(param0->direction, param0->hwSettings, param0->screen);
				param0->state++;
			}
			break;
		
		case 2:
			Heap_Free(param0->data);
			param0->data = NULL;
			param0->state++;
			v2 = 1;
			break;
		
		case 3:
			v2 = 1;
			break;
		
		default:
			GF_ASSERT(FALSE);
			break;
	}
	
	return v2;
}


static void ScreenFade_0200C118(ScreenFadeFuncStruct30* param0, ScreenFadeFuncStruct29* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, int param7) {
	int v0 = FX_Mul(param2 * FX32_ONE, param1->unk_14) >> FX32_SHIFT;
	
	param0->unk_384 = param2 - v0;
	param0->unk_380 = param1;
	param0->unk_386 = param1->unk_00.unk_0B;
	
	ScreenFade_0200ACBC(&param0->unk_00, &param1->unk_00, v0, param3, screen, param5);
	
	if (param1->unk_00.unk_08 == 0) {
		ScreenFade_0200A580(param5, GX_WNDMASK_W0, screen, param1->unk_00.unk_0B);
	} else {
		ScreenFade_0200A580(param5, GX_WNDMASK_W1, screen, param1->unk_00.unk_0B);
	}
	
	param0->unk_385 = 0;
}


static BOOL ScreenFade_0200C1A4(ScreenFadeFuncStruct30* param0, ScreenFade* param1) {
	BOOL v0;
	BOOL v1 = FALSE;
	
	switch (param0->unk_385) {
		case 0:
			v0 = ScreenFade_0200AD2C(&param0->unk_00);
			if (v0 == 1) {
				param0->unk_385++;
				ScreenFade_0200BE6C(&param0->unk_4C, &param0->unk_380->unk_0C, param0->unk_384, param1->framesPerStep, param1->screen, param1->hwSettings, param1->hblanks, param1->heapID);
			}
			break;
		
		case 1:
			v0 = ScreenFade_0200BF58(&param0->unk_4C);
			if (v0 == 1) {
				v1 = 1;
				param0->unk_385++;
			}
			break;
		
		case 2:
			v1 = 1;
			break;
	}
	
	return v1;
}


static void ScreenFade_0200C218(ScreenFadeFuncStruct30* param0, ScreenFadeFuncStruct29* param1, int param2, int param3, DSScreen screen, HardwareWindowSettings* param5, ScreenFadeHBlanks* param6, HeapID heapID) {
	int v0;
	
	param0->unk_384 = FX_Mul(param2 * FX32_ONE, param1->unk_14) >> FX32_SHIFT;
	
	v0 = param2 - param0->unk_384;
	
	param0->unk_380 = param1;
	param0->unk_386 = param1->unk_00.unk_0B;
	
	ScreenFade_0200BE6C(&param0->unk_4C, &param0->unk_380->unk_0C, v0, param3, screen, param5, param6, heapID);
	
	param0->unk_385 = 0;
}


static BOOL ScreenFade_0200C288(ScreenFadeFuncStruct30* param0, ScreenFade* param1) {
	BOOL v0;
	BOOL v1 = FALSE;
	
	switch (param0->unk_385) {
		case 0:
			v0 = ScreenFade_0200BF58(&param0->unk_4C);
			if (v0 == 1) {
				param0->unk_385++;
				ScreenFade_0200ACBC(&param0->unk_00, &param0->unk_380->unk_00, param0->unk_384, param1->framesPerStep, param1->screen, param1->hwSettings);
				
				if (param0->unk_380->unk_00.unk_08 == 0) {
					ScreenFade_0200A580(param1->hwSettings, GX_WNDMASK_W0, param1->screen, param0->unk_380->unk_00.unk_0B);
				} else {
					ScreenFade_0200A580(param1->hwSettings, GX_WNDMASK_W1, param1->screen, param0->unk_380->unk_00.unk_0B);
				}
			}
			break;
		
		case 1:
			v0 = ScreenFade_0200AD2C(&param0->unk_00);
			
			if (v0 == 1) {
				v1 = 1;
				param0->unk_385++;
			}
			break;
		
		case 2:
			v1 = 1;
			break;
	}
	
	return v1;
}

