#include <nitro.h>

#include "mpr_client/mpr_text.h"

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


static inline charcode_t* GetChars(int messageID) {
	StringTable* tbl = (StringTable*)LOCAL_FILE(mpr_string_table_bin);
	int offset = tbl->stringEntries[messageID].offset;
	return (charcode_t*)(LOCAL_FILE(mpr_string_table_bin) + offset);
}


const charcode_t* MPRText_GetChars(int messageID) {
	return GetChars(messageID);
}


String* MPRText_CopyChars(String* dest, int messageID) {
	String_CopyChars(dest, GetChars(messageID));
	
	// BUG: Returning local variable address (but this is never used)
	return (String*)&dest;
}
