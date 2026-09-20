#include <nitro.h>
#include <string.h>

#include "graphics.h"

#include "bg_window.h"
#include "heap.h"
#include "narc.h"


u32 Graphics_LoadTilesToBgLayer(NarcID narcID, u32 narcMemberIdx, BgConfig* bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID) {
	void* ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
	if (ncgrBuffer != NULL) {
		NNSG2dCharacterData* tiles;
		if (NNS_G2dGetUnpackedBGCharacterData(ncgrBuffer, &tiles)) {
			if (size == 0) {
				size = tiles->szByte;
			}
			
			Bg_LoadTiles(bgConfig, bgLayer, tiles->pRawData, size, offset);
		}
		
		Heap_Free(ncgrBuffer);
	}
	
	return size;
}


void Graphics_LoadTilemapToBgLayer(NarcID narcID, u32 narcMemberIdx, BgConfig* bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, HeapID heapID) {
	void* nscrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
	if (nscrBuffer != NULL) {
		NNSG2dScreenData* tilemap;
		if (NNS_G2dGetUnpackedScreenData(nscrBuffer, &tilemap)) {
			if (size == 0) {
				size = tilemap->szByte;
			}
			
			if (Bg_GetTilemapBuffer(bgConfig, bgLayer) != NULL) {
				Bg_LoadTilemapBuffer(bgConfig, bgLayer, tilemap->rawData, size);
			}
			
			Bg_CopyTilemapBufferRangeToVRAM(bgConfig, bgLayer, tilemap->rawData, size, offset);
		}
		
		Heap_Free(nscrBuffer);
	}
}


void Graphics_LoadPalette(NarcID narcID, u32 narcMemberIdx, PaletteLoadLocation loadLocation, u32 palOffset, u32 size, HeapID heapID) {
	Graphics_LoadPaletteWithSrcOffset(narcID, narcMemberIdx, loadLocation, 0, palOffset, size, heapID);
}


static void (*const sPaletteLoadFuncs[])(const void*, u32, u32) = {
	GX_LoadBGPltt,
	GX_LoadOBJPltt,
	GX_LoadBGExtPltt,
	GX_LoadOBJExtPltt,
	GXS_LoadBGPltt,
	GXS_LoadOBJPltt,
	GXS_LoadBGExtPltt,
	GXS_LoadOBJExtPltt
};

void Graphics_LoadPaletteWithSrcOffset(NarcID narcID, u32 narcMemberIdx, PaletteLoadLocation loadLocation, u32 srcOffset, u32 palOffset, u32 size, HeapID heapID) {
	void* nclrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, FALSE, heapID, TRUE);
	if (nclrBuffer != NULL) {
		NNSG2dPaletteData* palette;
		if (NNS_G2dGetUnpackedPaletteData(nclrBuffer, &palette)) {
			palette->pRawData = (void*)((u32)palette->pRawData + srcOffset);
			
			if (size == 0) {
				size = palette->szByte - srcOffset;
			}
			
			DC_FlushRange(palette->pRawData, size);
			
			switch (loadLocation) {
			case PAL_LOAD_MAIN_BGEXT:
				GX_BeginLoadBGExtPltt();
				sPaletteLoadFuncs[loadLocation](palette->pRawData, palOffset, size);
				GX_EndLoadBGExtPltt();
				break;
			
			case PAL_LOAD_SUB_BGEXT:
				GXS_BeginLoadBGExtPltt();
				sPaletteLoadFuncs[loadLocation](palette->pRawData, palOffset, size);
				GXS_EndLoadBGExtPltt();
				break;
			
			case PAL_LOAD_MAIN_OBJEXT:
				GX_BeginLoadOBJExtPltt();
				sPaletteLoadFuncs[loadLocation](palette->pRawData, palOffset, size);
				GX_EndLoadOBJExtPltt();
				break;
			
			case PAL_LOAD_SUB_OBJEXT:
				GXS_BeginLoadOBJExtPltt();
				sPaletteLoadFuncs[loadLocation](palette->pRawData, palOffset, size);
				GXS_EndLoadOBJExtPltt();
				break;
			
			default:
				sPaletteLoadFuncs[loadLocation](palette->pRawData, palOffset, size);
				break;
			}
		}
		
		Heap_Free(nclrBuffer);
	}
}


static void (*const sDisplayObjectLoadFunc[])(const void*, u32, u32) = {
	GX_LoadOBJ,
	GXS_LoadOBJ
};

u32 Graphics_LoadObjectTiles(NarcID narcID, u32 narcMemberIdx, DSScreen display, u32 offset, u32 size, BOOL compressed, HeapID heapID) {
	void* ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
	if (ncgrBuffer != NULL) {
		NNSG2dCharacterData* chr;
		if (NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &chr)) {
			if (size == 0) {
				size = chr->szByte;
			}
			
			DC_FlushRange(chr->pRawData, size);
			sDisplayObjectLoadFunc[display](chr->pRawData, offset, size);
		}
		
		Heap_Free(ncgrBuffer);
	}
	
	return size;
}


void Graphics_LoadPartialPalette(NarcID narcID, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, HeapID heapID, NNSG2dImagePaletteProxy* paletteProxy) {
	void* nclrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, FALSE, heapID, TRUE);
	if (nclrBuffer != NULL) {
		NNSG2dPaletteData *palette;
		NNSG2dPaletteCompressInfo *compPalette;
		BOOL extended = NNS_G2dGetUnpackedPaletteCompressInfo(nclrBuffer, &compPalette);
		
		if (NNS_G2dGetUnpackedPaletteData(nclrBuffer, &palette)) {
			if (extended) {
				NNS_G2dLoadPaletteEx(palette, compPalette, baseAddr, vramType, paletteProxy);
			} else {
				NNS_G2dLoadPalette(palette, baseAddr, vramType, paletteProxy);
			}
		}
		
		Heap_Free(nclrBuffer);
	}
}


static void (*const sImageLayoutLoadFuncs[])(const NNSG2dCharacterData*, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy*) = {
	NNS_G2dLoadImage1DMapping,
	NNS_G2dLoadImage2DMapping
};

u32 Graphics_LoadImageMapping(NarcID narcID, u32 narcMemberIdx, BOOL compressed, ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, HeapID heapID, NNSG2dImageProxy* imageProxy) {
	void* ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
	u32 result = 0;
	if (ncgrBuffer != NULL) {
		NNSG2dCharacterData *tiles;
		if (NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &tiles)) {
			if (size) {
				tiles->szByte = size;
			}
			
			sImageLayoutLoadFuncs[layout](tiles, baseAddr, vramType, imageProxy);
			result = tiles->szByte;
		}
		
		Heap_Free(ncgrBuffer);
	}
	
	return result;
}


static void (*const sImageLayoutLoadFuncs2[])(const NNSG2dCharacterData*, u32, NNS_G2D_VRAM_TYPE, NNSG2dImageProxy*) = {
	NNS_G2dLoadImage1DMapping,
	NNS_G2dLoadImage2DMapping
};

void Graphics_LoadImageMappingAndSetVramMode(NarcID narcID, u32 narcMemberIdx, BOOL compressed, ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, HeapID heapID, NNSG2dImageProxy* imageProxy) {
	void* ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, TRUE);
	if (ncgrBuffer != NULL) {
		NNSG2dCharacterData *tiles;
		if (NNS_G2dGetUnpackedCharacterData(ncgrBuffer, &tiles)) {
			if (size) {
				tiles->szByte = size;
			}
			
			switch (vramType) {
			case NNS_G2D_VRAM_TYPE_2DMAIN:
				tiles->mappingType = GX_GetOBJVRamModeChar();
				break;
			case NNS_G2D_VRAM_TYPE_2DSUB:
				tiles->mappingType = GXS_GetOBJVRamModeChar();
				break;
			}
			
			sImageLayoutLoadFuncs2[layout](tiles, baseAddr, vramType, imageProxy);
		}
		
		Heap_Free(ncgrBuffer);
	}
}


void* Graphics_GetCharData(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dCharacterData** outCharData, HeapID heapID) {
	void* ncgrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
	if (ncgrBuffer != NULL) {
		if (NNS_G2dGetUnpackedBGCharacterData(ncgrBuffer, outCharData) == 0) {
			Heap_Free(ncgrBuffer);
			return NULL;
		}
	}
	
	return ncgrBuffer;
}


void* Graphics_GetScrnData(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dScreenData** outScreenData, HeapID heapID) {
	void* nscrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
	if (nscrBuffer != NULL) {
		if (NNS_G2dGetUnpackedScreenData(nscrBuffer, outScreenData) == 0) {
			Heap_Free(nscrBuffer);
			return NULL;
		}
	}
	
	return nscrBuffer;
}


void* Graphics_GetPlttData(NarcID narcID, u32 narcMemberIdx, NNSG2dPaletteData** outPaletteData, HeapID heapID) {
	void* nclrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, FALSE, heapID, FALSE);
	if (nclrBuffer != NULL) {
		if (NNS_G2dGetUnpackedPaletteData(nclrBuffer, outPaletteData) == 0) {
			Heap_Free(nclrBuffer);
			return NULL;
		}
	}
	
	return nclrBuffer;
}


void* Graphics_GetCellBank(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dCellDataBank** outCellBank, HeapID heapID) {
	void* ncerBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
	if (ncerBuffer != NULL) {
		if (NNS_G2dGetUnpackedCellBank(ncerBuffer, outCellBank) == 0) {
			Heap_Free(ncerBuffer);
			return NULL;
		}
	}
	
	return ncerBuffer;
}


void* Graphics_GetAnimBank(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dAnimBankData** outAnimBank, HeapID heapID) {
	void* nanrBuffer = LoadMemberFromNARC(narcID, narcMemberIdx, compressed, heapID, FALSE);
	if (nanrBuffer != NULL) {
		if (NNS_G2dGetUnpackedAnimBank(nanrBuffer, outAnimBank) == 0) {
			Heap_Free(nanrBuffer);
			return NULL;
		}
	}
	
	return nanrBuffer;
}


void* LoadMemberFromNARC(NarcID narcID, u32 narcMemberIdx, BOOL compressed, HeapID heapID, BOOL allocAtEnd) {
	void* data;
	if (compressed || allocAtEnd == TRUE) {
		data = Heap_AllocAtEnd(heapID, NARC_GetMemberSizeByIndexPair(narcID, narcMemberIdx));
	} else {
		data = Heap_Alloc(heapID, NARC_GetMemberSizeByIndexPair(narcID, narcMemberIdx));
	}
	
	if (data != NULL) {
		NARC_ReadWholeMemberByIndexPair(data, narcID, narcMemberIdx);
		if (compressed) {
			void* uncompBuf;
			if (allocAtEnd == FALSE) {
				uncompBuf = Heap_Alloc(heapID, MI_GetUncompressedSize(data));
			} else {
				uncompBuf = Heap_AllocAtEnd(heapID, MI_GetUncompressedSize(data));
			}
			
			if (uncompBuf) {
				MI_UncompressLZ8(data, uncompBuf);
				Heap_Free(data);
			}
			
			data = uncompBuf;
		}
	}
	
	return data;
}


void* LoadMemberFromNARC_OutFileSize(NarcID narcID, u32 narcMemberIdx, BOOL compressed, HeapID heapID, BOOL allocAtEnd, u32* fileSize) {
	void* data;
	*fileSize = NARC_GetMemberSizeByIndexPair(narcID, narcMemberIdx);
	if (compressed || allocAtEnd == TRUE) {
		data = Heap_AllocAtEnd(heapID, *fileSize);
	} else {
		data = Heap_Alloc(heapID, *fileSize);
	}
	
	if (data != NULL) {
		NARC_ReadWholeMemberByIndexPair(data, narcID, narcMemberIdx);
		if (compressed) {
			*fileSize = MI_GetUncompressedSize(data);
			void *uncompBuf;
			if (allocAtEnd == FALSE) {
				uncompBuf = Heap_Alloc(heapID, *fileSize);
			} else {
				uncompBuf = Heap_AllocAtEnd(heapID, *fileSize);
			}
			
			if (uncompBuf) {
				MI_UncompressLZ8(data, uncompBuf);
				Heap_Free(data);
			}
			
			data = uncompBuf;
		}
	}
	
	return data;
}
