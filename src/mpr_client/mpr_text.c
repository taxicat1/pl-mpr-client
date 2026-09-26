#include <nitro.h>

#include "mpr_client/mpr_text.h"

#include "game_version.h"

#include "fs/local.h"

typedef struct {
	u32  offset;
	u32  size;
} StringEntry;

typedef struct {
	u32          magic;
	StringEntry  stringEntries[MPR_TEXT_COUNT];
	charcode_t   charBlob[];
} StringTable;

static const u8* sStringTableBinary = LOCAL_FILE(mpr_string_table_EN_bin);


void MPRText_SetLanguage(void) {
	static BOOL setLanguage = FALSE;
	
	if (!setLanguage) {
		switch (gGameLanguage) {
			default:
			case LANGUAGE_ENGLISH:
				// Default
				break;
			
			case LANGUAGE_GERMAN:
				sStringTableBinary = LOCAL_FILE(mpr_string_table_DE_bin);
				break;
			
			case LANGUAGE_ITALIAN:
				sStringTableBinary = LOCAL_FILE(mpr_string_table_IT_bin);
				break;
			
			case LANGUAGE_SPANISH:
				sStringTableBinary = LOCAL_FILE(mpr_string_table_ES_bin);
				break;
			
			case LANGUAGE_FRENCH:
				sStringTableBinary = LOCAL_FILE(mpr_string_table_FR_bin);
				break;
		}
		
		setLanguage = TRUE;
	}
}

static inline charcode_t* GetChars(int messageID) {
	StringTable* tbl = (StringTable*)sStringTableBinary;
	int offset = tbl->stringEntries[messageID].offset;
	return (charcode_t*)(sStringTableBinary + offset);
}


const charcode_t* MPRText_GetChars(int messageID) {
	return GetChars(messageID);
}


String* MPRText_CopyChars(String* dest, int messageID) {
	String_CopyChars(dest, GetChars(messageID));
	return dest;
}
