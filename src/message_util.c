#include <nitro.h>
#include <string.h>

#include "message_util.h"

#include "message.h"
#include "game_version.h"
#include "string_gf.h"
#include "string_template.h"
#include "constants/heap.h"
#include "constants/narc.h"


String* MessageUtil_ExpandedString(StringTemplate* template, MessageLoader* loader, u32 entryID, HeapID heapID) {
	String* ret = NULL;
	String* buf = String_Init(EXPANDED_STRING_SIZE, HEAP_ID_SYSTEM);
	if (buf) {
		String* entry = MessageLoader_GetNewString(loader, entryID);
		if (entry) {
			StringTemplate_Format(template, buf, entry);
			ret = String_Clone(buf, heapID);
			String_Free(entry);
		}
		
		String_Free(buf);
	}
	
	return ret;
}


String* MessageUtil_SpeciesName(u32 species, HeapID heapID) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	MessageLoader* loader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_SPECIES_NAME, heapID);
	if (loader) {
		String* speciesName = MessageLoader_GetNewString(loader, species);
		MessageLoader_Free(loader);
		return speciesName;
	}
	
	return NULL;
}
