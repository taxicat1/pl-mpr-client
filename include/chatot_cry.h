#ifndef MPR_CHATOT_CRY_H
#define MPR_CHATOT_CRY_H

#include "struct_defs/chatot_cry.h"

#include "savedata.h"

int ChatotCry_GetSaveSize(void);
void ChatotCry_Init(ChatotCry* chatotCry);
ChatotCry* SaveData_GetChatotCry(SaveData* saveData);
BOOL ChatotCry_IsValid(const ChatotCry* chatotCry);
void ChatotCry_ResetStatus(ChatotCry* chatotCry);
const void* ChatotCry_GetRawAudio(const ChatotCry* chatotCry);
void ChatotCry_GetUpsampledAudio(s8* outData, const s8* inCryData);

#endif /* MPR_CHATOT_CRY_H */
