#ifndef MPR_GAME_VERSION_H
#define MPR_GAME_VERSION_H

#include "constants/versions.h"

extern const u8 gGameVersion;
extern const u8 gGameLanguage;

extern BOOL gIsDiamondPearl;
extern BOOL gIsPlatinum;

void GameVersion_Set(u8 newVersion);

#endif /* MPR_GAME_VERSION_H */
