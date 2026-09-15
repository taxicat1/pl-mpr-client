#include <nitro.h>
#include <string.h>

#include "sound_playback.h"

#include "constants/species.h"
#include "constants/sdat.h"
#include "struct_defs/chatot_cry.h"
#include "assert.h"
#include "heap.h"
#include "sound.h"
#include "sound_chatot.h"
#include "sound_system.h"
#include "sys_task.h"
#include "sys_task_manager.h"

#define WAVE_OUT_SPEED_HYPERVOICE_1  (WAVE_OUT_SPEED(1.046875))
#define WAVE_OUT_SPEED_UPROAR_1      (WAVE_OUT_SPEED(0.8125))

#define DEFAULT_FANFARE_DELAY 15

typedef struct {
	int       remainingFrames;
	SysTask*  task;
} PokemonCryDurationParam;

static void Sound_Impl_HandleBGMChange(u16 param0, SoundHandleType param1);
static BOOL Sound_Impl_PlayBGM(u16 seqID, u8 playerID, SoundHandleType handleType);
static BOOL Sound_Impl_PlayFieldBGM(u16 seqID, u8 playerID, SoundHandleType handleType);
static void Sound_Impl_ResetBGM(void);
static void Sound_Impl_SetPokemonCryVolume(u16 param0, SoundHandleType param1, int param2);
static void Sound_SetPokemonCryDuration(int param0, HeapID heapID);
static void Sound_Impl_CryDurationTask(SysTask* param0, void* param1);
static void Sound_Impl_DestroyCryDurationTask(void);
static BOOL Sound_Impl_PlayPokemonCryEcho(u16 species, s8 pitch);
static BOOL Sound_PlayPokemonCryReversedEcho(u16 param0, s8 param1, int param2, int param3, HeapID heapID);


BOOL Sound_PlayBGM(u16 bgmID) {
	BOOL result;
	u8 player = Sound_GetPlayerForSequence(bgmID);
	SoundHandleType handleType = SoundSystem_GetSoundHandleTypeFromPlayerID(player);
	
	if (player == PLAYER_BGM) {
		result = Sound_Impl_PlayBGM(bgmID, player, handleType);
	} else if (player == PLAYER_FIELD) {
		result = Sound_Impl_PlayFieldBGM(bgmID, player, handleType);
	} else {
		GF_ASSERT(FALSE);
		return FALSE;
	}
	
	// Field BGM Bank may or may not have been switched, so set it to idle
	Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_IDLE);
	
	Sound_Impl_HandleBGMChange(bgmID, handleType);
	return result;
}


static void Sound_Impl_HandleBGMChange(u16 seqID, SoundHandleType handleType) {
	Sound_SetCurrentBGM(seqID);
	Sound_AdjustVolumeForVoiceChatEx(seqID, handleType);
	SoundSystem_SetState(SOUND_SYSTEM_STATE_PLAY);
}


static BOOL Sound_Impl_PlayBGM(u16 seqID, u8 playerID, SoundHandleType handleType) {
	SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_SFX));
	SoundSystem_LoadSequence(seqID);
	SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM));
	
	return NNS_SndArcPlayerStartSeq(SoundSystem_GetSoundHandle(handleType), seqID);
}


static BOOL Sound_Impl_PlayFieldBGM(u16 seqID, u8 playerID, SoundHandleType handleType) {
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_BANK_STATE);
	
	u16* newFieldBGM = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);
	
	int currentSeqID = Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_FIELD_BGM));
	Sound_LoadSoundDataForFieldBGM(seqID, Sound_GetBankIDFromSequenceID(currentSeqID));
	
	return NNS_SndArcPlayerStartSeqEx(
		SoundSystem_GetSoundHandle(handleType),
		-1,
		Sound_GetBankIDFromSequenceID(*newFieldBGM),
		-1,
		seqID);
}


static void Sound_Impl_ResetBGM(void) {
	Sound_SetCurrentBGM(SEQ_NONE);
	Sound_SetNextBGM(SEQ_NONE);
	SoundSystem_SetState(SOUND_SYSTEM_STATE_IDLE);
}


void Sound_FadeInBGM(int targetVolume, int frames, BGMFadeInType fadeInType) {
	u16 currentBGM = Sound_GetCurrentBGM();
	u8 playerID = Sound_GetPlayerForSequence(currentBGM);
	if (playerID == SOUND_PLAYER_INVALID) {
		return;
	}
	
	SoundHandleType handleType = SoundSystem_GetSoundHandleTypeFromPlayerID(playerID);
	
	if (fadeInType == BGM_FADE_IN_TYPE_FROM_ZERO) {
		Sound_FadeVolumeForHandle(handleType, 0, 0);
	}
	
	Sound_FadeVolumeForHandle(handleType, targetVolume, frames);
	Sound_SetFadeCounter(frames);
	SoundSystem_SetState(SOUND_SYSTEM_STATE_FADE_IN);
}


BOOL Sound_IsFadeActive() {
	u16* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FADE_COUNTER);
	return* param;
}


void Sound_StopWaveOutAndSequences(void) {
	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	
	NNS_SndPlayerStopSeqAll(0);
	
	if (*primaryAllocated == TRUE) {
		Sound_StopWaveOut(WAVE_OUT_CHANNEL_PRIMARY);
	}
	
	if (*secondaryAllocated == TRUE) {
		Sound_StopWaveOut(WAVE_OUT_CHANNEL_SECONDARY);
	}
	
	SoundSystem_SetState(SOUND_SYSTEM_STATE_IDLE);
}


void Sound_StopAll(void) {
	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	
	NNS_SndPlayerStopSeq(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_BGM), 0);
	Sound_Impl_ResetBGM();
	
	for (int i = 0; i < NUM_SFX_HANDLES; i++) {
		Sound_StopEffectFromHandle(SOUND_HANDLE_TYPE_SFX_1 + i, 0);
	}
	
	Sound_StopPokemonCries(0);
	
	if (*primaryAllocated == TRUE) {
		Sound_StopWaveOut(WAVE_OUT_CHANNEL_PRIMARY);
	}
	
	if (*secondaryAllocated == TRUE) {
		Sound_StopWaveOut(WAVE_OUT_CHANNEL_SECONDARY);
	}
}


BOOL Sound_PlayEffect(u16 seqID) {
	SoundHandleType handleType = SoundSystem_GetSoundHandleTypeFromPlayerID(Sound_GetPlayerForSequence(seqID));
	BOOL result = NNS_SndArcPlayerStartSeq(SoundSystem_GetSoundHandle(handleType), seqID);
	
	Sound_AdjustVolumeForVoiceChatEx(seqID, handleType);
	
	return result;
}


void Sound_StopEffectFromHandle(SoundHandleType handleType, int fadeOutFrames) {
	NNS_SndPlayerStopSeq(SoundSystem_GetSoundHandle(handleType), fadeOutFrames);
}


BOOL Sound_PlayPokemonCry(u16 species) {
	BOOL success;
	u8* echoEnabled = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_ECHO_ENABLED);
	ChatotCry** chatotCry = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CHATOT_CRY);
	
	if (species != SPECIES_CHATOT) {
		if (species > MAX_SPECIES || species == SPECIES_NONE) {
			species = SPECIES_BULBASAUR;
		}
	}
	
	if (species == SPECIES_CHATOT) {
		if (Sound_Impl_PlayChatotCry(*chatotCry, 0, SOUND_VOLUME_MAX, 0) == TRUE) {
			Sound_SetUsingDefaultChatotCry(FALSE);
			return TRUE;
		}
	}
	
	if (*echoEnabled == FALSE) {
		Sound_StopPokemonCries(0);
		success = NNS_SndArcPlayerStartSeqEx(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_POKEMON_CRY), -1, species, -1, SEQ_PV);
		Sound_AdjustVolumeForVoiceChatEx(species, SOUND_HANDLE_TYPE_POKEMON_CRY);
	} else {
		success = NNS_SndArcPlayerStartSeqEx(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_ECHO), -1, species, -1, SEQ_PV);
		Sound_AdjustVolumeForVoiceChatEx(species, SOUND_HANDLE_TYPE_ECHO);
	}
	
	Sound_SetUsingDefaultChatotCry(FALSE);
	
	return success;
}


void Sound_StopPokemonCries(int fadeOutFrames) {
	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSED_PLAYBACK);
	
	NNS_SndPlayerStopSeq(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_POKEMON_CRY), fadeOutFrames);
	NNS_SndPlayerStopSeq(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_ECHO), fadeOutFrames);
	
	if (*primaryAllocated == TRUE) {
		Sound_StopWaveOutReversed(WAVE_OUT_CHANNEL_PRIMARY);
		Sound_FreeWaveOutChannel(WAVE_OUT_CHANNEL_PRIMARY);
	}
	
	if (*secondaryAllocated == TRUE) {
		Sound_StopWaveOutReversed(WAVE_OUT_CHANNEL_SECONDARY);
		Sound_FreeWaveOutChannel(WAVE_OUT_CHANNEL_SECONDARY);
	}
	
	Sound_StopChatotCry();
}


BOOL Sound_IsPokemonCryPlaying(void) {
	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSED_PLAYBACK);
	
	if (*primaryAllocated == TRUE) {
		return Sound_IsWaveOutPlaying(WAVE_OUT_CHANNEL_PRIMARY);
	}
	
	if (*secondaryAllocated == TRUE) {
		return Sound_IsWaveOutPlaying(WAVE_OUT_CHANNEL_SECONDARY);
	}
	
	return Sound_GetNumberOfPlayingSequencesForPlayer(PLAYER_PV);
}


BOOL Sound_PlayPokemonCryEx(PokemonCryMod cryMod, u16 species, int pan, int volume, HeapID heapID) {
	int waveOutPan, echoVolume; // Need to be declared up here to match
	u8* wavePrimaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* waveSecondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	u8* echoEnabled = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_ECHO_ENABLED);
	u8* chatotCryPlaying = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CHATOT_CRY_PLAYING);
	
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CHATOT_CRY);
	
	waveOutPan = WAVE_OUT_PAN_CENTER + (pan / 2);
	echoVolume = volume - 30;
	
	if (echoVolume <= 0) {
		echoVolume = 1;
	}
	
	*echoEnabled = FALSE;
	
	if (*wavePrimaryAllocated == TRUE) {
		Sound_StopWaveOutReversed(WAVE_OUT_CHANNEL_PRIMARY);
		Sound_FreeWaveOutChannel(WAVE_OUT_CHANNEL_PRIMARY);
	}
	
	if (*waveSecondaryAllocated == TRUE) {
		Sound_StopWaveOutReversed(WAVE_OUT_CHANNEL_SECONDARY);
		Sound_FreeWaveOutChannel(WAVE_OUT_CHANNEL_SECONDARY);
	}
	
	u16 speciesNo = species;
	if (species != SPECIES_CHATOT) {
		if (species > MAX_SPECIES || species == SPECIES_NONE) {
			speciesNo = SPECIES_BULBASAUR;
		}
	}
	
	if (species == SPECIES_CHATOT) {
		switch (cryMod) {
			case POKECRY_NORMAL:
			case POKECRY_HALF_DURATION:
			case POKECRY_FIELD_EVENT:
			case POKECRY_FAINT:
			case POKECRY_PINCH_NORMAL:
			case POKECRY_PINCH_HALF_DURATION:
				Sound_PlayPokemonCry(SPECIES_CHATOT);
				
				if (*chatotCryPlaying == FALSE) {
					Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
					Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
				} else {
					Sound_SetWaveOutPan(WAVE_OUT_CHANNEL_PRIMARY, waveOutPan);
					Sound_SetWaveOutVolume(WAVE_OUT_CHANNEL_PRIMARY, volume);
				}
				
				return TRUE;
			default:
				Sound_SetUsingDefaultChatotCry(TRUE);
				break;
		}
	}
	
	switch (cryMod) {
		case POKECRY_NORMAL:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			break;
		case POKECRY_HALF_DURATION:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPokemonCryDuration(20, heapID);
			break;
		case POKECRY_FIELD_EVENT:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(1));
			
			Sound_Impl_PlayPokemonCryEcho(speciesNo, SOUND_SEMITONES(0.3125));
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_ECHO, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_ECHO, echoVolume);
			break;
		case POKECRY_MID_MOVE:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPokemonCryDuration(30, heapID);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(3));
			
			Sound_Impl_PlayPokemonCryEcho(speciesNo, SOUND_SEMITONES(0.25));
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_ECHO, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_ECHO, echoVolume);
			break;
		case POKECRY_HYPERVOICE_1:
			Sound_AllocateWaveOutChannel(WAVE_OUT_CHANNEL_PRIMARY);
			Sound_PlayWaveOutReversed(speciesNo, volume, waveOutPan, WAVE_OUT_CHANNEL_PRIMARY, heapID);
			Sound_SetWaveOutPan(WAVE_OUT_CHANNEL_PRIMARY, waveOutPan);
			Sound_SetPokemonCryDuration(15, heapID);
			Sound_SetWaveOutSpeed(WAVE_OUT_CHANNEL_PRIMARY, WAVE_OUT_SPEED_HYPERVOICE_1);
			Sound_PlayPokemonCryReversedEcho(speciesNo, SOUND_SEMITONES(-1), echoVolume, waveOutPan, heapID);
			Sound_SetWaveOutSpeed(WAVE_OUT_CHANNEL_SECONDARY, WAVE_OUT_SPEED_HYPERVOICE_1);
			break;
		case POKECRY_FAINT:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(-3.5));
			break;
		case POKECRY_HYPERVOICE_2:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(0.6875));
			
			Sound_Impl_PlayPokemonCryEcho(speciesNo, SOUND_SEMITONES(-1));
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_ECHO, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_ECHO, echoVolume);
			break;
		case POKECRY_HOWL_1:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPokemonCryDuration(11, heapID);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(-2));
			break;
		case POKECRY_HOWL_2:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPokemonCryDuration(60, heapID);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(0.9375));
			break;
		case POKECRY_UPROAR_1:
			Sound_AllocateWaveOutChannel(WAVE_OUT_CHANNEL_PRIMARY);
			Sound_PlayWaveOutReversed(speciesNo, volume, waveOutPan, WAVE_OUT_CHANNEL_PRIMARY, heapID);
			Sound_SetWaveOutPan(WAVE_OUT_CHANNEL_PRIMARY, waveOutPan);
			Sound_SetPokemonCryDuration(13, heapID);
			Sound_SetWaveOutSpeed(WAVE_OUT_CHANNEL_PRIMARY, WAVE_OUT_SPEED_UPROAR_1);
			break;
		case POKECRY_UPROAR_2:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPokemonCryDuration(100, heapID);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(-0.6875));
			break;
		case POKECRY_PINCH_NORMAL:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(-1.5));
			break;
		case POKECRY_PINCH_HALF_DURATION:
			Sound_PlayPokemonCry(speciesNo);
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, volume);
			Sound_SetPokemonCryDuration(20, heapID);
			Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_PLAYBACK_TRACK_ALL, SOUND_SEMITONES(-1.5));
			break;
		case POKECRY_POKEDEX_CHORUS:
			Sound_PlayPokemonCry(speciesNo);
			Sound_Impl_SetPokemonCryVolume(speciesNo, SOUND_HANDLE_TYPE_POKEMON_CRY, SOUND_VOLUME_MAX);
			Sound_Impl_PlayPokemonCryEcho(speciesNo, SOUND_SEMITONES(0.3125));
			Sound_SetPanForHandle(SOUND_HANDLE_TYPE_ECHO, SOUND_PLAYBACK_TRACK_ALL, pan);
			Sound_FadeVolumeForHandle(SOUND_HANDLE_TYPE_ECHO, volume, 0);
			break;
		case POKECRY_POKEDEX:
			Sound_PlayPokemonCry(speciesNo);
			break;
	}
	
	return TRUE;
}


static void Sound_Impl_SetPokemonCryVolume(u16 species, SoundHandleType handleType, int volume) {
	Sound_SetInitialVolumeForHandle(handleType, volume);
	Sound_AdjustVolumeForVoiceChatEx(species, handleType);
}


static void Sound_SetPokemonCryDuration(int duration, HeapID heapID) {
	PokemonCryDurationParam* param = NULL;
	SysTask** task = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CRY_DURATION_TASK);
	
	Sound_Impl_DestroyCryDurationTask();
	
	param = Heap_Alloc(heapID, sizeof(PokemonCryDurationParam));
	if (param == NULL) {
		GF_ASSERT(FALSE);
		return;
	}
	
	memset(param, 0, sizeof(PokemonCryDurationParam));
	
	param->remainingFrames = duration;
	param->task = SysTask_Start(Sound_Impl_CryDurationTask, param, 0);
	*task = param->task;
}


static void Sound_Impl_CryDurationTask(SysTask* task, void* arg) {
	u8* wavePrimaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* waveSecondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	PokemonCryDurationParam* param = (PokemonCryDurationParam*)arg;
	
	if (param->remainingFrames == 10) {
		Sound_FadeVolumeForHandle(SOUND_HANDLE_TYPE_POKEMON_CRY, 0, param->remainingFrames);
		Sound_FadeVolumeForHandle(SOUND_HANDLE_TYPE_ECHO, 0, param->remainingFrames);
	}
	
	param->remainingFrames--;
	
	if (Sound_IsPokemonCryPlaying() == FALSE) {
		param->remainingFrames = 0;
	}
	
	if (param->remainingFrames <= 0) {
		Sound_StopPokemonCries(0);
		
		if (*wavePrimaryAllocated == TRUE) {
			Sound_StopWaveOutReversed(WAVE_OUT_CHANNEL_PRIMARY);
			Sound_FreeWaveOutChannel(WAVE_OUT_CHANNEL_PRIMARY);
		}
		
		if (*waveSecondaryAllocated == TRUE) {
			Sound_StopWaveOutReversed(WAVE_OUT_CHANNEL_SECONDARY);
			Sound_FreeWaveOutChannel(WAVE_OUT_CHANNEL_SECONDARY);
		}
		
		Sound_Impl_DestroyCryDurationTask();
	}
}


static void Sound_Impl_DestroyCryDurationTask() {
	SysTask** task = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CRY_DURATION_TASK);
	
	if (*task != NULL) {
		void* param = SysTask_GetParam(*task);
		SysTask_Done(*task);
		Heap_Free(param);
	}
	
	*task = NULL;
}


static BOOL Sound_Impl_PlayPokemonCryEcho(u16 species, s8 pitch) {
	u8* echoEnabled = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_ECHO_ENABLED);
	*echoEnabled = TRUE;
	
	Sound_SetUsingDefaultChatotCry(TRUE);
	BOOL success = Sound_PlayPokemonCry(species);
	Sound_SetPitchForHandle(SOUND_HANDLE_TYPE_ECHO, SOUND_PLAYBACK_TRACK_ALL, pitch);
	
	return success;
}


static BOOL Sound_PlayPokemonCryReversedEcho(u16 species, s8 unused, int volume, int pan, HeapID heapID) {
	u8* echoEnabled = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_ECHO_ENABLED);
	*echoEnabled = TRUE;
	
	BOOL success = Sound_AllocateWaveOutChannel(WAVE_OUT_CHANNEL_SECONDARY);
	success = Sound_PlayWaveOutReversed(species, volume, pan, WAVE_OUT_CHANNEL_SECONDARY, heapID);
	
	return success;
}
