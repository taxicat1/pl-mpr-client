#include <nitro.h>
#include <string.h>

#include "palette.h"


void PaletteData_LoadBuffer(PaletteData* paletteData, const void* src, PaletteBufferID bufferID, u16 destStart, u16 srcSize) {
	MI_CpuCopy16(src, paletteData->buffers[bufferID].unfaded + destStart, srcSize);
	MI_CpuCopy16(src, paletteData->buffers[bufferID].faded + destStart, srcSize);
}


u16* PaletteData_GetUnfadedBuffer(PaletteData* palette, PaletteBufferID bufferID) {
	return palette->buffers[bufferID].unfaded;
}


void BlendPalette(const u16* src, u16* dest, u16 size, u8 fraction, u16 target) {
	u16 i;
	int srcR, srcG, srcB;
	int targetR = ((RgbColor*)&target)->r;
	int targetG = ((RgbColor*)&target)->g;
	int targetB = ((RgbColor*)&target)->b;
	
	for (i = 0; i < size; i++) {
		srcR = ((RgbColor*)&src[i])->r;
		srcG = ((RgbColor*)&src[i])->g;
		srcB = ((RgbColor*)&src[i])->b;
		
		dest[i] = BlendColor(srcR, targetR, fraction) | (BlendColor(srcG, targetG, fraction) << 5) | (BlendColor(srcB, targetB, fraction) << 10);
	}
}
