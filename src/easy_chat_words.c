#include <nitro.h>

#include "easy_chat_words.h"

#include "common.h"
#include "constants/versions.h"


u32 UnlockedEasyChatWords_GetSaveSize(void) {
	return sizeof(UnlockedEasyChatWords);
}


enum {
	GREETING_JAPANESE = 0,
	GREETING_ENGLISH,
	GREETING_FRENCH, 
	GREETING_ITALIAN,
	GREETING_GERMAN, 
	GREETING_SPANISH
};

void UnlockedEasyChatWords_Init(UnlockedEasyChatWords* unlockedWords) {
	static const struct {
		u8  language;
		u8  bankEntry;
	} greetingMapping[] = {
		{ LANGUAGE_JAPANESE,  GREETING_JAPANESE },
		{ LANGUAGE_ENGLISH,   GREETING_ENGLISH  },
		{ LANGUAGE_FRENCH,    GREETING_FRENCH   },
		{ LANGUAGE_ITALIAN,   GREETING_ITALIAN  },
		{ LANGUAGE_GERMAN,    GREETING_GERMAN   },
		{ LANGUAGE_SPANISH,   GREETING_SPANISH  }
	};
	
	unlockedWords->unlockedGreetingBits = 0;
	unlockedWords->unlockedToughWordBits = 0;
	
	for (int i = 0; i < NELEMS(greetingMapping); i++) {
		if (GAME_LANGUAGE == greetingMapping[i].language) {
			EasyChatWords_UnlockGreeting(unlockedWords, greetingMapping[i].bankEntry);
			break;
		}
	}
}


void EasyChatWords_UnlockGreeting(UnlockedEasyChatWords* unlockedWords, int bankEntry) {
	unlockedWords->unlockedGreetingBits |= (1 << bankEntry);
}
