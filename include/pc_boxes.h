#ifndef MPR_PC_BOXES_H
#define MPR_PC_BOXES_H

#include "struct_defs/pokemon.h"
#include "constants/pokemon_data_params.h"
#include "string_gf.h"

#define MAX_PC_BOXES               18
#define MAX_PC_ROWS                5
#define MAX_PC_COLS                6
#define MAX_MONS_PER_BOX           (MAX_PC_ROWS * MAX_PC_COLS)
#define MAX_DEFAULT_WALLPAPERS     16
#define MAX_UNLOCKABLE_WALLPAPERS  8
#define MAX_WALLPAPERS             (MAX_DEFAULT_WALLPAPERS + MAX_UNLOCKABLE_WALLPAPERS)
#define PC_BOX_NAME_BUFFER_LEN     20

#define BOX_DATA_SIZE  (sizeof(BoxPokemon) * MAX_MONS_PER_BOX)

#define USE_CURRENT_BOX  -1

typedef struct {
	u32         currentBoxID;
	BoxPokemon  boxMons[MAX_PC_BOXES][MAX_MONS_PER_BOX];
	u16         names[MAX_PC_BOXES][PC_BOX_NAME_BUFFER_LEN];
	u8          wallpapers[MAX_PC_BOXES];
	u8          unlockedWallpapers;
} PCBoxes;

void PCBoxes_Init(PCBoxes* pcBoxes);
u32 PCBoxes_GetSaveSize(void);
BOOL PCBoxes_TryStoreBoxMon(PCBoxes* pcBoxes, BoxPokemon* boxMon);
BOOL PCBoxes_TryStoreBoxMonInBox(PCBoxes* pcBoxes, u32 boxID, BoxPokemon* boxMon);
BOOL PCBoxes_TryStoreBoxMonAt(PCBoxes* pcBoxes, u32 boxID, u32 monPosInBox, BoxPokemon* boxMon);
void PCBoxes_InitBoxMonAt(PCBoxes* pcBoxes, u32 boxID, u32 monPosInBox);
u32 PCBoxes_GetCurrentBoxID(const PCBoxes* pcBoxes);
u32 PCBoxes_FirstEmptyBox(PCBoxes* pcBoxes);
u32 PCBoxes_CountAllEmptySpace(PCBoxes* pcBoxes);
u32 PCBoxes_CountEmptySpaceInBox(PCBoxes* pcBoxes, u32 boxID);
void PCBoxes_SetCurrentBox(PCBoxes* pcBoxes, u32 boxID);
u32 PCBoxes_GetWallpaper(const PCBoxes* pcBoxes, u32 boxID);
void PCBoxes_SetWallpaper(PCBoxes* pcBoxes, u32 boxID, u32 wallpaper);
void PCBoxes_BufferBoxName(const PCBoxes* pcBoxes, u32 boxID, String* dest);
void PCBoxes_RenameBox(PCBoxes* pcBoxes, u32 boxID, const String* newName);
u32 PCBoxes_CountMonsInBox(PCBoxes* pcBoxes, u32 boxID);
void PCBoxes_SetBoxMonData(PCBoxes* pcBoxes, u32 boxID, u32 slot, PokemonDataParam pokemonData, void* value);
BoxPokemon* PCBoxes_GetBoxMonAt(PCBoxes* pcBoxes, u32 boxID, u32 slot);
BOOL PCBoxes_CheckHasUnlockedWallpaper(const PCBoxes* pcBoxes, u32 wallpaper);
u32 PCBoxes_CountUnlockedWallpapers(const PCBoxes* pcBoxes);

#endif /* MPR_PC_BOXES_H */
