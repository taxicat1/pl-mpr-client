#ifndef MPR_SOUND_CHATOT_H
#define MPR_SOUND_CHATOT_H

#include "constants/sound.h"

#include "struct_defs/chatot_cry.h"

BOOL Sound_UpdateChatotCry(void);
BOOL Sound_IsRecordedChatotCryPlayable(const ChatotCry* cry);
BOOL Sound_Impl_PlayChatotCry(const ChatotCry* cry, u32 unused, int volume, int pan);
void Sound_StopChatotCry(void);
void Sound_SetUsingDefaultChatotCry(u8 value);
BOOL Sound_PlayChatotCry(ChatotCry* cry, u32 unused, int volume, int pan);

#endif /* MPR_SOUND_CHATOT_H */
