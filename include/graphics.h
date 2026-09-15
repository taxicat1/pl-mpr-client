#ifndef MPR_GRAPHICS_H
#define MPR_GRAPHICS_H

#include <nitro/types.h>
#include <nnsys.h>

#include "constants/graphics.h"

#include "bg_window.h"
#include "narc.h"

typedef enum {
    PAL_LOAD_MAIN_BG = 0,
    PAL_LOAD_MAIN_OBJ,
    PAL_LOAD_MAIN_BGEXT,
    PAL_LOAD_MAIN_OBJEXT,
    PAL_LOAD_SUB_BG,
    PAL_LOAD_SUB_OBJ,
    PAL_LOAD_SUB_BGEXT,
    PAL_LOAD_SUB_OBJEXT,
} PaletteLoadLocation;

typedef enum {
    IMAGE_MAPPING_LAYOUT_1D = 0,
    IMAGE_MAPPING_LAYOUT_2D,
} ImageMappingLayout;

u32 Graphics_LoadTilesToBgLayer(NarcID narcID, u32 narcMemberIdx, BgConfig* bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, u32 heapID);
void Graphics_LoadTilemapToBgLayer(NarcID narcID, u32 narcMemberIdx, BgConfig* bgConfig, u32 bgLayer, u32 offset, u32 size, BOOL compressed, HeapID heapID);
void Graphics_LoadPalette(NarcID narcID, u32 narcMemberIdx, PaletteLoadLocation loadLocation, u32 palOffset, u32 size, HeapID heapID);
void Graphics_LoadPaletteWithSrcOffset(NarcID narcID, u32 narcMemberIdx, PaletteLoadLocation loadLocation, u32 srcOffset, u32 palOffset, u32 size, HeapID heapID);
u32 Graphics_LoadObjectTiles(NarcID narcID, u32 narcMemberIdx, DSScreen display, u32 offset, u32 size, BOOL compressed, HeapID heapID);
void Graphics_LoadPartialPalette(NarcID narcID, u32 narcMemberIdx, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, HeapID heapID, NNSG2dImagePaletteProxy* paletteProxy);
u32 Graphics_LoadImageMapping(NarcID narcID, u32 narcMemberIdx, BOOL compressed, ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, HeapID heapID, NNSG2dImageProxy* imageProxy);
void Graphics_LoadImageMappingAndSetVramMode(NarcID narcID, u32 narcMemberIdx, BOOL compressed, ImageMappingLayout layout, u32 size, NNS_G2D_VRAM_TYPE vramType, u32 baseAddr, HeapID heapID, NNSG2dImageProxy* imageProxy);
void* Graphics_GetCharData(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dCharacterData* *outCharData, HeapID heapID);
void* Graphics_GetScrnData(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dScreenData* *outScreenData, HeapID heapID);
void* Graphics_GetPlttData(NarcID narcID, u32 narcMemberIdx, NNSG2dPaletteData* *outPaletteData, HeapID heapID);
void* Graphics_GetCellBank(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dCellDataBank* *outCellBank, HeapID heapID);
void* Graphics_GetAnimBank(NarcID narcID, u32 narcMemberIdx, BOOL compressed, NNSG2dAnimBankData* *outAnimBank, HeapID heapID);
void* LoadMemberFromNARC(NarcID narcID, u32 narcMemberIdx, BOOL compressed, HeapID heapID, BOOL allocAtEnd);
void* LoadMemberFromNARC_OutFileSize(NarcID narcID, u32 narcMemberIdx, BOOL compressed, HeapID heapID, BOOL allocAtEnd, u32* fileSize);

#endif /* MPR_GRAPHICS_H */