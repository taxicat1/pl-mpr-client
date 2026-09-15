#ifndef MPR_PALETTE_H
#define MPR_PALETTE_H

#include "constants/colors.h"
#include "constants/heap.h"
#include "constants/narc.h"

#define BlendColor(source, target, fraction) ((source) + (((target) - (source)) * (fraction) >> 4))

typedef enum {
	PLTTBUF_MAIN_BG = 0,
	PLTTBUF_SUB_BG,
	PLTTBUF_MAIN_OBJ,
	PLTTBUF_SUB_OBJ,
	PLTTBUF_EX_BEGIN,
	
	PLTTBUF_MAIN_EX_BG_0 = PLTTBUF_EX_BEGIN,
	PLTTBUF_MAIN_EX_BG_1,
	PLTTBUF_MAIN_EX_BG_2,
	PLTTBUF_MAIN_EX_BG_3,
	PLTTBUF_SUB_EX_BG_0,
	PLTTBUF_SUB_EX_BG_1,
	PLTTBUF_SUB_EX_BG_2,
	PLTTBUF_SUB_EX_BG_3,
	PLTTBUF_MAIN_EX_OBJ,
	PLTTBUF_SUB_EX_OBJ,
	
	PLTTBUF_MAX,
} PaletteBufferID;

#define PLTTBUF_MAIN_BG_F       (1 << PLTTBUF_MAIN_BG)
#define PLTTBUF_SUB_BG_F        (1 << PLTTBUF_SUB_BG)
#define PLTTBUF_MAIN_OBJ_F      (1 << PLTTBUF_MAIN_OBJ)
#define PLTTBUF_SUB_OBJ_F       (1 << PLTTBUF_SUB_OBJ)
#define PLTTBUF_MAIN_EX_BG_0_F  (1 << PLTTBUF_MAIN_EX_BG_0)
#define PLTTBUF_MAIN_EX_BG_1_F  (1 << PLTTBUF_MAIN_EX_BG_1)
#define PLTTBUF_MAIN_EX_BG_2_F  (1 << PLTTBUF_MAIN_EX_BG_2)
#define PLTTBUF_MAIN_EX_BG_3_F  (1 << PLTTBUF_MAIN_EX_BG_3)
#define PLTTBUF_SUB_EX_BG_0_F   (1 << PLTTBUF_SUB_EX_BG_0)
#define PLTTBUF_SUB_EX_BG_1_F   (1 << PLTTBUF_SUB_EX_BG_1)
#define PLTTBUF_SUB_EX_BG_2_F   (1 << PLTTBUF_SUB_EX_BG_2)
#define PLTTBUF_SUB_EX_BG_3_F   (1 << PLTTBUF_SUB_EX_BG_3)
#define PLTTBUF_MAIN_EX_OBJ_F   (1 << PLTTBUF_MAIN_EX_OBJ)
#define PLTTBUF_SUB_EX_OBJ_F    (1 << PLTTBUF_SUB_EX_OBJ)
#define PLTTBUF_ALL_F           ((1 << PLTTBUF_MAX) - 1)

typedef enum {
	PLTTSEL_FADED,
	PLTTSEL_UNFADED,
	PLTTSEL_BOTH
} PaletteSelector;

typedef struct {
	u16  r : 5;
	u16  g : 5;
	u16  b : 5;
	u16    : 1;
} RgbColor;

typedef struct {
	u16  unfadedMask;
	u16  wait     : 6;
	u16  cur      : 5;
	u16  end      : 5;
	u16  target   : 15;
	u16  sign     : 1;
	u16  step     : 4;
	u16  waitStep : 6;
	u16  unused   : 6;
} PaletteFadeControl;

typedef struct {
	u16*                unfaded;
	u16*                faded;
	u32                 size;
	PaletteFadeControl  selected;
} PaletteBuffer;

typedef struct {
	PaletteBuffer  buffers[14];
	
	u16  selectedFlag    : 2;
	u16  selectedBuffers : 14;
	u16  fadedBuffers    : 14;
	u16  fadeInProgress  : 1;
	u16  autoTransparent : 1;
	u8   forceExit;
} PaletteData;

void BlendPalette(const u16* src, u16* dest, u16 size, u8 fraction, u16 target);
void PaletteData_LoadBuffer(PaletteData* paletteData, const void* src, PaletteBufferID bufferID, u16 destStart, u16 srcSize);
u16* PaletteData_GetUnfadedBuffer(PaletteData* palette, PaletteBufferID bufferID);

#endif /* MPR_PALETTE_H */
