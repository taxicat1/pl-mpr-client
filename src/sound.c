#include <nitro.h>
#include <string.h>

#include "sound.h"

#include "constants/species.h"
#include "assert.h"
#include "heap.h"
#include "sound_playback.h"
#include "sound_system.h"

#define BGM_PLAYER_NORMAL_CHANNELS 0x7FF
#define BGM_PLAYER_EXTRA_CHANNELS  0x7FFF

#define WAVEFORM_SAMPLE_WINDOW 100 // Used to generate amplitude graphs from waveform data
#define WAVEFORM_MAX_AMPLITUDE 9

static void Sound_LoadSoundEffectsForSceneWithState(u8 scene);
static void Sound_Impl_PlayFieldBGM(u16 bgmID, int unused);
static void Sound_Impl_ResumeAndSwitchFieldBGM(u16 bgmID, u16 unused);
static void Sound_Impl_PlayBattleBGM(u16 bgmID, int unused);
static void Sound_0200284C(u16 bgmID, int unused);
static void Sound_Impl_PlayContestBGM(u16 bgmID, int unused);
static void Sound_02002890(u16 bgmID, int unused);
static void Sound_Impl_LoadSubSceneSoundData(u8 scene);
static void Sound_Impl_PlayCutsceneBGM(u8 scene, u16 bgmID, int unused);
static void Sound_Impl_ReverseBuffer(u8* buffer, u32 size);
static void Sound_SetBGMAllocatableChannels(u16 channels);
static void Sound_Impl_PauseOrStopFieldBGM(void);

static s8 sWaveBuffer[SOUND_WAVE_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static int sPlaybackMode;


void Sound_SetCurrentBGM(u16 bgmID) {
	u16* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CURRENT_BGM);
	*param = bgmID;

	Sound_SetNextBGM(SEQ_NONE);
}


u16 Sound_Impl_GetCurrentBGM(void) {
	u16* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CURRENT_BGM);
	return *param;
}


void Sound_SetNextBGM(u16 bgmID) {
	u16* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_NEXT_BGM);
	*param = bgmID;
}


u16 Sound_GetNextBGM(void) {
	u16* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_NEXT_BGM);
	return *param;
}


void Sound_SetScene(u8 scene) {
	u8* mainScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_MAIN_SCENE);
	u8* subScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_SUB_SCENE);
	
	if (scene < SOUND_SCENE_MAX) {
		*mainScene = scene;
		*subScene = SOUND_SCENE_NONE;
	} else {
		*subScene = scene;
	}
}


int Sound_LoadSoundEffectsForScene(u8 scene) {
	int result;
	switch (scene) {
		case SOUND_SCENE_TITLE_SCREEN:
		case SOUND_SCENE_9:
		case SOUND_SCENE_10:
		case SOUND_SCENE_14:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
			break;
		
		case SOUND_SCENE_2:
		case SOUND_SCENE_13:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_BATTLE);
			break;
		
		case SOUND_SCENE_3:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_TRADE);
			break;
		
		case SOUND_SCENE_FIELD:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
			break;
		
		case SOUND_SCENE_BATTLE:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_BATTLE);
			break;
		
		case SOUND_SCENE_11:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
			break;
		
		case SOUND_SCENE_CONTEST:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_CONTEST);
			break;
		
		case SOUND_SCENE_8:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_FIELD);
			break;
		
		case SOUND_SCENE_12:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_NUTMIXER);
			break;
		
		case SOUND_SCENE_SUB_BAG:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_BAG);
			break;
		
		case SOUND_SCENE_SUB_66:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_SLOT);
			break;
		
		case SOUND_SCENE_SUB_52:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_NAMEIN);
			break;
		
		case SOUND_SCENE_7:
		case SOUND_SCENE_SUB_53:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_IMAGE);
			break;
		
		case SOUND_SCENE_SUB_54:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_ZUKAN);
			break;
		
		case SOUND_SCENE_SUB_55:
		case SOUND_SCENE_SUB_67:
			SoundSystem_LoadBank(BANK_SE_TOWNMAP);
			result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_TOWNMAP);
			break;
		
		case SOUND_SCENE_SUB_56:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_TRCARD);
			break;
		
		case SOUND_SCENE_SUB_57:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_POKELIST);
			break;
		
		case SOUND_SCENE_SUB_58:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_DIG);
			break;
		
		case SOUND_SCENE_SUB_59:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_CUSTOM);
			break;
		
		case SOUND_SCENE_SUB_60:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_BAG);
			break;
		
		case SOUND_SCENE_SUB_61:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_NAMEIN);
			break;
		
		case SOUND_SCENE_SUB_62:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_CUSTOM);
			break;
		
		case SOUND_SCENE_SUB_63:
			result = SoundSystem_LoadSoundGroup(GROUP_SE_CLIMAX);
			break;
		
		case SOUND_SCENE_SUB_64:
			SoundSystem_LoadBank(BANK_SE_SHIP);
			result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_SHIP);
			break;
		
		case SOUND_SCENE_SUB_65:
			SoundSystem_LoadBank(BANK_SE_TRAIN);
			result = SoundSystem_LoadWaveArc(WAVE_ARC_SE_TRAIN);
			break;
	}
	
	return result;
}


BOOL Sound_SetSceneAndPlayBGM(u8 scene, u16 bgmID, int unused) {
	u8* mainScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_MAIN_SCENE);
	u8* subScene = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_SUB_SCENE);
	u16* fanfareDelay = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FANFARE_DELAY);
	
	if (scene < SOUND_SCENE_MAX) {
		if (*mainScene == scene) {
			return FALSE;
		}
	} else {
		if (*subScene == scene) {
			return FALSE;
		}
	}
	
	Sound_SetScene(scene);
	
	switch (scene) {
		case SOUND_SCENE_FIELD:
			Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_DEFAULT);
			Sound_Impl_PlayFieldBGM(bgmID, unused);
			*fanfareDelay = 0;
			break;
		
		case SOUND_SCENE_BATTLE:
			Sound_Impl_PlayBattleBGM(bgmID, unused);
			break;
		
		case SOUND_SCENE_11:
			Sound_0200284C(bgmID, unused);
			break;
		
		case SOUND_SCENE_CONTEST:
			Sound_Impl_PlayContestBGM(bgmID, unused);
			break;
		
		case SOUND_SCENE_7:
			Sound_02002890(bgmID, unused);
			break;
		
		case SOUND_SCENE_SUB_BAG:
		case SOUND_SCENE_SUB_52:
		case SOUND_SCENE_SUB_53:
		case SOUND_SCENE_SUB_54:
		case SOUND_SCENE_SUB_55:
		case SOUND_SCENE_SUB_56:
		case SOUND_SCENE_SUB_57:
		case SOUND_SCENE_SUB_58:
		case SOUND_SCENE_SUB_59:
		case SOUND_SCENE_SUB_60:
		case SOUND_SCENE_SUB_61:
		case SOUND_SCENE_SUB_62:
		case SOUND_SCENE_SUB_63:
		case SOUND_SCENE_SUB_64:
		case SOUND_SCENE_SUB_65:
		case SOUND_SCENE_SUB_66:
		case SOUND_SCENE_SUB_67:
			Sound_Impl_LoadSubSceneSoundData(scene);
			break;
		
		case SOUND_SCENE_TITLE_SCREEN:
			Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_TITLE);
			Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
			break;
		
		case SOUND_SCENE_14:
			Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_ENDING);
			Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
			break;
		
		case SOUND_SCENE_2:
			Sound_ConfigureBGMChannelsAndReverb(SOUND_CHANNEL_CONFIG_DEFAULT);
			Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
			break;
		
		case SOUND_SCENE_3:
		case SOUND_SCENE_8:
		case SOUND_SCENE_9:
		case SOUND_SCENE_10:
		case SOUND_SCENE_12:
		case SOUND_SCENE_13:
			Sound_Impl_PlayCutsceneBGM(scene, bgmID, unused);
			break;
	}
	
	return TRUE;
}


static void Sound_LoadSoundEffectsForSceneWithState(u8 scene) {
	int* persistentState = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	
	SoundSystem_LoadHeapState(*persistentState);
	SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM_BANK));
	Sound_LoadSoundEffectsForScene(scene);
	SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));
}


static void Sound_Impl_PlayFieldBGM(u16 bgmID, int unused) {
	u8* fieldBGMPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_PAUSED);
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);
	
	int currentFieldBGM = Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_FIELD_BGM));
	
	if (*fieldBGMPaused == FALSE) {
		if (currentFieldBGM == bgmID) {
			if (Sound_GetNextBGM() != SEQ_BICYCLE) {
				return;
			}
		}
	}
	
	Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
	Sound_StopAll();

	if (currentFieldBGM != bgmID) {
		Sound_SetBGMPlayerPaused(PLAYER_FIELD, FALSE);
		Sound_StopWaveOutAndSequences();
	}
	
	if (*fieldBGMPaused == TRUE) {
		SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_BGM_BANK));
		Sound_LoadSoundEffectsForScene(SOUND_SCENE_FIELD);
		SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));

		if (currentFieldBGM != bgmID) {
			Sound_SetBGMPlayerPaused(PLAYER_FIELD, FALSE);
		}


		Sound_Impl_ResumeAndSwitchFieldBGM(bgmID, currentFieldBGM);
		return;
	}
	
	Sound_PlayBGM(bgmID);
}


static void Sound_Impl_ResumeAndSwitchFieldBGM(u16 bgmID, u16 unused) {
	u16* currentBGM = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);
	u16 currentBankID = Sound_GetBankIDFromSequenceID(*currentBGM);
	
	if (currentBankID != BANK_BGM_FIELD && currentBankID != BANK_BGM_DUNGEON) {
		SoundSystem_LoadSequenceEx(bgmID, NNS_SND_ARC_LOAD_WAVE);
		GF_ASSERT(FALSE);
	} else {
		SoundSystem_LoadSequenceEx(*currentBGM, NNS_SND_ARC_LOAD_WAVE | NNS_SND_ARC_LOAD_BANK);
	}
	
	SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM));
	Sound_SetBGMPlayerPaused(PLAYER_FIELD, FALSE);
	Sound_FadeInBGM(SOUND_VOLUME_MAX, 40, BGM_FADE_IN_TYPE_FROM_ZERO);
	Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_IDLE);
}


void Sound_LoadSoundDataForFieldBGM(u16 seqID, u16 currentBankID) {
	u8* bankState = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_BANK_STATE);
	u16* newFieldBGM = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM);
	
	if (*bankState == FIELD_BGM_BANK_STATE_SWITCH || currentBankID == 0) {
		SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_PERSISTENT));
		SoundSystem_LoadSequenceEx(*newFieldBGM, NNS_SND_ARC_LOAD_BANK);
		SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM_BANK));
		Sound_LoadSoundEffectsForScene(SOUND_SCENE_FIELD);
		SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));
		
		u16 newBankID = Sound_GetBankIDFromSequenceID(*newFieldBGM);
		if (newBankID != BANK_BGM_FIELD && newBankID != BANK_BGM_DUNGEON) {
			SoundSystem_LoadSequenceEx(seqID, NNS_SND_ARC_LOAD_WAVE);
			GF_ASSERT(FALSE);
		} else {
			SoundSystem_LoadSequenceEx(*newFieldBGM, NNS_SND_ARC_LOAD_WAVE);
		}
		
		
		SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM));
	}
}


static void Sound_Impl_PlayBattleBGM(u16 bgmID, int unused) {
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	
	Sound_Impl_PauseOrStopFieldBGM();
	SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_BGM_BANK));
	Sound_LoadSoundEffectsForScene(SOUND_SCENE_BATTLE);
	SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX));
	Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
	Sound_PlayBGM(bgmID);
}


static void Sound_0200284C(u16 param0, int param1) {
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	
	Sound_StopWaveOutAndSequences();
	Sound_ClearBGMPauseFlags();
	Sound_LoadSoundEffectsForSceneWithState(SOUND_SCENE_FIELD);
	Sound_PlayBGM(param0);
}


static void Sound_Impl_PlayContestBGM(u16 bgmID, int unused) {
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	
	Sound_StopWaveOutAndSequences();
	Sound_LoadSoundEffectsForSceneWithState(SOUND_SCENE_CONTEST);
	Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
	Sound_PlayBGM(bgmID);
}


static void Sound_02002890(u16 bgmID, int unused) {
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	
	Sound_StopWaveOutAndSequences();
	Sound_LoadSoundEffectsForSceneWithState(SOUND_SCENE_7);
	Sound_SetFieldBGMBankState(FIELD_BGM_BANK_STATE_SWITCH);
	Sound_PlayBGM(bgmID);
}


static void Sound_Impl_LoadSubSceneSoundData(u8 scene) {
	Sound_LoadHeapStateBGM();
	Sound_LoadSoundEffectsForScene(scene);
	SoundSystem_SaveHeapState(SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SUB_SFX));
}


static void Sound_Impl_PlayCutsceneBGM(u8 scene, u16 bgmID, int unused) {
	SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
	
	Sound_StopWaveOutAndSequences();
	Sound_LoadSoundEffectsForSceneWithState(scene);
	Sound_PlayBGM(bgmID);
}


void Sound_LoadHeapStateBGM(void) {
	SoundSystem_LoadHeapState(Sound_GetHeapState(SOUND_HEAP_STATE_BGM));
}


int Sound_GetHeapState(SoundHeapState state) {
	int* param;
	SoundSystem* soundSys = SoundSystem_Get();
	
	if (state >= SOUND_HEAP_STATE_COUNT) {
		GF_ASSERT(FALSE);
		param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM);
		return* param;
	}
	
	switch (state) {
		case SOUND_HEAP_STATE_EMPTY:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_EMPTY);
			break;
		case SOUND_HEAP_STATE_PERSISTENT:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_PERSISTENT);
			break;
		case SOUND_HEAP_STATE_BGM_BANK:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM_BANK);
			break;
		case SOUND_HEAP_STATE_SFX:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SFX);
			break;
		case SOUND_HEAP_STATE_BGM:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_BGM);
			break;
		case SOUND_HEAP_STATE_SUB_SFX:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_SUB_SFX);
			break;
		case SOUND_HEAP_STATE_FANFARE:
			param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_HEAP_STATE_FANFARE);
			break;
	}
	
	return *param;
}


void Sound_SetBGMPlayerPaused(u8 playerID, BOOL paused) {
	u8 handleType;
	u8* playerPaused;
	
	if (playerID == PLAYER_FIELD) {
		playerPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_PAUSED);
		handleType = SOUND_HANDLE_TYPE_FIELD_BGM;
	} else if (playerID == PLAYER_BGM) {
		playerPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_BGM_PAUSED);
		handleType = SOUND_HANDLE_TYPE_BGM;
	} else {
		return;
	}
	
	if (paused == FALSE) {
		Sound_SetCurrentBGM(Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(handleType)));
	}
	
	NNS_SndPlayerPause(SoundSystem_GetSoundHandle(handleType), paused);
	*playerPaused = paused;
}


void Sound_ClearBGMPauseFlags(void) {
	u8* fieldBGMPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_PAUSED);
	u8* bgmPaused = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_BGM_PAUSED);
	
	*fieldBGMPaused = FALSE;
	*bgmPaused = FALSE;
}


void Sound_FadeVolumeForHandle(SoundHandleType handleType, int targetVolume, int frames) {
	NNS_SndPlayerMoveVolume(SoundSystem_GetSoundHandle(handleType), targetVolume, frames);
}


void Sound_SetInitialVolumeForHandle(SoundHandleType handleType, int volume) {
	if (volume < SOUND_VOLUME_MIN) {
		volume = SOUND_VOLUME_MIN;
	}
	
	if (volume > SOUND_VOLUME_MAX) {
		volume = SOUND_VOLUME_MAX;
	}
	
	NNS_SndPlayerSetInitialVolume(SoundSystem_GetSoundHandle(handleType), volume);
}


void Sound_AdjustVolumeForVoiceChatEx(int seqID, SoundHandleType handleType) {
	// Nonfunctional
	const NNSSndSeqParam* param = NNS_SndArcGetSeqParam(seqID);
	
	switch (handleType) {
		case SOUND_HANDLE_TYPE_POKEMON_CRY:
		case SOUND_HANDLE_TYPE_ECHO:
			break;
		
		default:
			if (param == NULL) {
				return;
			}
			
			break;
	}
}


int Sound_GetNumberOfPlayingSequencesForPlayer(int playerID) {
	if (playerID < PLAYER_PV) {
		GF_ASSERT(FALSE);
	}
	
	return NNS_SndPlayerCountPlayingSeqByPlayerNo(playerID);
}


u8 Sound_GetPlayerForSequence(u16 seqID) {
	if (seqID == 0) {
		return 0xFF;
	}
	
	const NNSSndSeqParam* param = NNS_SndArcGetSeqParam(seqID);
	if (param == NULL) {
		return 0xFF;
	}
	
	return param->playerNo;
}


int Sound_GetSequenceIDFromSoundHandle(NNSSndHandle* handle) {
	return NNS_SndPlayerGetSeqNo(handle);
}


u16 Sound_GetBankIDFromSequenceID(int seqID) {
	const NNSSndSeqParam* param = NNS_SndArcGetSeqParam(seqID);
	if (param == NULL) {
		return 0;
	}
	
	return param->bankNo;
}


NNSSndWaveOutHandle* Sound_GetWaveOutHandle(WaveOutChannel channel) {
	SoundSystem_Get();
	
	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);
	
	if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
		GF_ASSERT(FALSE);
	}
	
	if (channel == WAVE_OUT_CHANNEL_PRIMARY && *primaryAllocated == FALSE) {
		GF_ASSERT(FALSE);
	}
	
	if (channel == WAVE_OUT_CHANNEL_SECONDARY && *secondaryAllocated == FALSE) {
		GF_ASSERT(FALSE);
	}
	
	if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
		return SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_HANDLE);
	} else {
		return SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_HANDLE);
	}
}


BOOL Sound_AllocateWaveOutChannel(WaveOutChannel channel) {
	NNSSndWaveOutHandle* handle;
	
	SoundSystem_Get();
	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);

	if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
		GF_ASSERT(FALSE);
	}
	
	if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
		if (*primaryAllocated == FALSE) {
			handle = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_HANDLE);
			*handle = NNS_SndWaveOutAllocChannel(channel);
			
			if (*handle == NNS_SND_WAVEOUT_INVALID_HANDLE) {
				return FALSE;
			}
			
			*primaryAllocated = TRUE;
		} else {
			GF_ASSERT(FALSE);
		}
	} else {
		if (*secondaryAllocated == FALSE) {
			handle = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_HANDLE);
			*handle = NNS_SndWaveOutAllocChannel(channel);

			if (*handle == NNS_SND_WAVEOUT_INVALID_HANDLE) {
				return FALSE;
			}
			
			*secondaryAllocated = TRUE;
		} else {
			GF_ASSERT(FALSE);
		}
	}
	
	return TRUE;
}


void Sound_FreeWaveOutChannel(WaveOutChannel channel) {
	SoundSystem_Get();

	u8* primaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_PRIMARY_ALLOCATED);
	u8* secondaryAllocated = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_SECONDARY_ALLOCATED);

	if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
		GF_ASSERT(FALSE);
		return;
	}
	
	if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
		if (*primaryAllocated == TRUE) {
			NNS_SndWaveOutFreeChannel(*Sound_GetWaveOutHandle(channel));
			*primaryAllocated = FALSE;
		} else {
			GF_ASSERT(FALSE);
		}
	} else {
		if (*secondaryAllocated == TRUE) {
			NNS_SndWaveOutFreeChannel(*Sound_GetWaveOutHandle(channel));
			*secondaryAllocated = FALSE;
		} else {
			GF_ASSERT(FALSE);
		}
	}
}


BOOL Sound_PlayWaveOut(WaveOutParam* param, WaveOutChannel channel) {
	BOOL success = NNS_SndWaveOutStart(
		*param->handle,
		param->format,
		param->data,
		param->loop,
		param->loopStartSample,
		param->samples,
		param->sampleRate,
		param->volume,
		param->speed,
		param->pan);
	
	if (success == FALSE) {
		Sound_FreeWaveOutChannel(channel);
	}
	
	return success;
}


void Sound_StopWaveOut(WaveOutChannel channel) {
	NNS_SndWaveOutStop(*Sound_GetWaveOutHandle(channel));
}


BOOL Sound_IsWaveOutPlaying(WaveOutChannel channel) {
	return NNS_SndWaveOutIsPlaying(*Sound_GetWaveOutHandle(channel));
}


void Sound_SetWaveOutPan(WaveOutChannel channel, u8 pan) {
	u8 clampedPan;
	if (pan > 127) {
		clampedPan = 127;
	} else {
		clampedPan = pan;
	}
	
	NNS_SndWaveOutSetPan(*Sound_GetWaveOutHandle(channel), clampedPan);
}


void Sound_SetWaveOutSpeed(WaveOutChannel channel, u32 speed) {
	NNS_SndWaveOutSetSpeed(*Sound_GetWaveOutHandle(channel), speed);
}


void Sound_SetWaveOutVolume(WaveOutChannel channel, int volume) {
	NNS_SndWaveOutSetVolume(*Sound_GetWaveOutHandle(channel), volume);
}


BOOL Sound_PlayWaveOutReversed(u16 waveArcID, int volume, int pan, WaveOutChannel channel, HeapID heapID) {
	SoundSystem_Get();
	
	void** reverseBuffer = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSE_BUFFER);
	
	if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
		GF_ASSERT(FALSE);
	}
	
	const NNSSndArcWaveArcInfo* info = NNS_SndArcGetWaveArcInfo(waveArcID);
	if (info == NULL) {
		GF_ASSERT(FALSE);
		return FALSE;
	}
	
	u32 fileSize = NNS_SndArcGetFileSize(info->fileId);
	if (fileSize == 0) {
		GF_ASSERT(FALSE);
		return FALSE;
	}
	
	if (channel == WAVE_OUT_CHANNEL_PRIMARY) {
		*reverseBuffer = Heap_Alloc(heapID, fileSize);
		if (*reverseBuffer == NULL) {
			GF_ASSERT(FALSE);
			return FALSE;
		}
		
		memset(*reverseBuffer, 0, fileSize);

		if (NNS_SndArcReadFile(info->fileId, *reverseBuffer, fileSize, 0) == -1) {
			GF_ASSERT(FALSE);
			return FALSE;
		}
		
		Sound_Impl_ReverseBuffer(*reverseBuffer, fileSize);
	}
	
	WaveOutParam param;
	param.handle = Sound_GetWaveOutHandle(channel);
	param.format = NNS_SND_WAVE_FORMAT_PCM8;
	param.data = *reverseBuffer;
	param.loop = FALSE;
	param.loopStartSample = 0;
	param.samples = fileSize;
	param.sampleRate = 13379;
	param.volume = volume;
	param.speed = WAVE_OUT_SPEED(0.75);
	param.pan = pan;
	
	BOOL result = Sound_PlayWaveOut(&param, channel);
	
	Sound_SetWaveOutVolume(channel, volume);
	
	u8* reversedPlayback = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSED_PLAYBACK);
	*reversedPlayback = TRUE;
	
	return result;
}


static void Sound_Impl_ReverseBuffer(u8* buffer, u32 size) {
	for (int i = 0; i < (size / 2); i++) {
		u8 tmp = buffer[i];
		buffer[i] = buffer[size - 1 - i];
		buffer[size - 1 - i] = tmp;
	}
}


void Sound_StopWaveOutReversed(WaveOutChannel channel) {
	SoundSystem_Get();
	
	u8* reversedPlaybackEnabled = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSED_PLAYBACK);
	void** reverseBuffer = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_WAVE_OUT_REVERSE_BUFFER);
	
	if (channel != WAVE_OUT_CHANNEL_PRIMARY && channel != WAVE_OUT_CHANNEL_SECONDARY) {
		GF_ASSERT(FALSE);
	}
	
	Sound_StopWaveOut(channel);
	
	if (*reversedPlaybackEnabled == TRUE) {
		*reversedPlaybackEnabled = FALSE;
		Heap_Free(*reverseBuffer);
	}
}


BOOL Sound_IsCaptureActive(void) {
	return NNS_SndCaptureIsActive();
}


BOOL Sound_StartReverb(int volume) {
	SoundSystem_Get();
	
	void* buffer = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_CAPTURE_BUFFER);
	return NNS_SndCaptureStartReverb(buffer, 0x1000, (NNS_SND_CAPTURE_FORMAT_PCM16), 16000, volume);
}


void Sound_StopReverb(int frames) {
	NNS_SndCaptureStopReverb(frames);
}


void Sound_SetPitchForHandle(SoundHandleType handleType, u16 tracks, int pitch) {
	NNS_SndPlayerSetTrackPitch(SoundSystem_GetSoundHandle(handleType), tracks, pitch);
}


void Sound_SetPanForHandle(SoundHandleType handleType, u16 tracks, int pan) {
	NNS_SndPlayerSetTrackPan(SoundSystem_GetSoundHandle(handleType), tracks, pan);
}


void Sound_SetPlaybackMode(int mode) {
	NNS_SndSetMonoFlag(mode);
	sPlaybackMode = mode;
}


void Sound_SetFadeCounter(int frames) {
	int* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FADE_COUNTER);
	*param = frames;
}


void Sound_SetFollowUpWaitFrames(int frames) {
	int* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FOLLOW_UP_WAIT_FRAMES);
	*param = frames;
}


BOOL Sound_UpdateFollowUpWaitFrames() {
	u16* waitFrames = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FOLLOW_UP_WAIT_FRAMES);
	if (*waitFrames <= 0) {
		*waitFrames = 0;
		return FALSE;
	}
	
	(*waitFrames)--;
	return *waitFrames;
}


void* Sound_GetWaveBuffer(void) {
	return sWaveBuffer;
}


// Needs to be set to FIELD_BGM_BANK_STATE_SWITCH before calling
// Sound_PlayBGM, if the bank needs to be switched
void Sound_SetFieldBGMBankState(int state) {
	u8* param = SoundSystem_GetParam(SOUND_SYSTEM_PARAM_FIELD_BGM_BANK_STATE);
	*param = state;
}


static void Sound_SetBGMAllocatableChannels(u16 channels) {
	NNS_SndPlayerSetAllocatableChannel(PLAYER_BGM, channels);
}


void Sound_ConfigureBGMChannelsAndReverb(SoundChannelConfig config) {
	if (config == SOUND_CHANNEL_CONFIG_DEFAULT) {
		Sound_SetBGMAllocatableChannels(BGM_PLAYER_NORMAL_CHANNELS);
		Sound_StopReverb(0);
	} else if (config == SOUND_CHANNEL_CONFIG_TITLE) {
		Sound_SetBGMAllocatableChannels(BGM_PLAYER_EXTRA_CHANNELS);
		Sound_StartReverb(30);
	} else { // SOUND_CHANNEL_CONFIG_ENDING or anything else
		Sound_SetBGMAllocatableChannels(BGM_PLAYER_EXTRA_CHANNELS);
		Sound_StartReverb(15);
	}
	
	Sound_IsCaptureActive();
}


static void Sound_Impl_PauseOrStopFieldBGM(void) {
	if ((Sound_IsFadeActive() == FALSE) && (Sound_GetSequenceIDFromSoundHandle(SoundSystem_GetSoundHandle(SOUND_HANDLE_TYPE_FIELD_BGM)) != -1) && (Sound_GetCurrentBGM() != SEQ_POKERADAR)) {
		Sound_StopAll();
		Sound_SetBGMPlayerPaused(PLAYER_FIELD, TRUE);
	} else {
		Sound_StopWaveOutAndSequences();
	}
}
