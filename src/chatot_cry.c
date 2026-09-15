#include <nitro.h>

#include "chatot_cry.h"

#include "savedata.h"


int ChatotCry_GetSaveSize(void) {
	return sizeof(ChatotCry);
}


void ChatotCry_Init(ChatotCry* chatotCry) {
	MI_CpuClear32(chatotCry, sizeof(ChatotCry));
	chatotCry->valid = FALSE;
}


ChatotCry* SaveData_GetChatotCry(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_CHATOT);
}


BOOL ChatotCry_IsValid(const ChatotCry* cry) {
	return cry->valid;
}


void ChatotCry_ResetStatus(ChatotCry* chatotCry) {
	chatotCry->valid = FALSE;
}


const void* ChatotCry_GetRawAudio(const ChatotCry* chatotCry) {
	return chatotCry->audioData;
}


void ChatotCry_GetUpsampledAudio(s8* outData, const s8* inCryData) {
	int i, index = 0;
	u8 sample;
	s8 convertedSample;
	
	for (i = 0; i < CHATOT_CRY_SIZE; i++) {
		// First sample (lower 4 bits)
		sample = inCryData[i] & 0xf;
		convertedSample = sample - 8;
		outData[index] = convertedSample * 16;
		
		// Second sample (upper 4 bits)
		sample = inCryData[i] >> 4;
		convertedSample = sample - 8;
		outData[index + 1] = convertedSample * 16;
		
		index += 2;
	}
}
