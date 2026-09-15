#ifndef MPR_SOUND_PLAYBACK_H
#define MPR_SOUND_PLAYBACK_H

#include "constants/sound.h"
#include "constants/sdat.h"

#include "sound_system.h"

typedef enum {
	BGM_FADE_IN_TYPE_FROM_ZERO = 0,  // Fade in from volume 0
	BGM_FADE_IN_TYPE_FROM_CURRENT    // Fade in from the current volume
} BGMFadeInType;

BOOL Sound_PlayBGM(u16 bgmID);
void Sound_FadeInBGM(int targetVolume, int frames, BGMFadeInType fadeInType);
BOOL Sound_IsFadeActive(void);
void Sound_StopWaveOutAndSequences(void);
void Sound_StopAll(void);
BOOL Sound_PlayEffect(u16 seqID);
void Sound_StopEffectFromHandle(SoundHandleType handleType, int fadeOutFrames);
BOOL Sound_PlayPokemonCry(u16 species);
void Sound_StopPokemonCries(int fadeOutFrames);
BOOL Sound_IsPokemonCryPlaying(void);
BOOL Sound_PlayPokemonCryEx(PokemonCryMod cryMod, u16 species, int pan, int volume, HeapID heapID);

#endif /* MPR_SOUND_PLAYBACK_H */
