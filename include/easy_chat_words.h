#ifndef MPR_EASY_CHAT_WORDS_H
#define MPR_EASY_CHAT_WORDS_H

#include <nitro/types.h>

typedef struct {
	u32  unlockedGreetingBits;
	u32  unlockedToughWordBits;
} UnlockedEasyChatWords;

u32 UnlockedEasyChatWords_GetSaveSize(void);
void UnlockedEasyChatWords_Init(UnlockedEasyChatWords* unlockedWords);
void EasyChatWords_UnlockGreeting(UnlockedEasyChatWords* unlockedWords, int bankEntry);

#endif /* MPR_EASY_CHAT_WORDS_H */
