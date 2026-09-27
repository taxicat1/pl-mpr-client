#include <nitro.h>
#include <string.h>

#include "message.h"

#include "assert.h"
#include "game_version.h"
#include "heap.h"
#include "narc.h"
#include "string_gf.h"

#define KEY_START  596947
#define KEY_INC    18749

static u16 sDiamondPearlUnifiedTextBankLookup[] = {
	TEXT_BANK_DP_SAVE_DATA_READ_ERROR,
	TEXT_BANK_DP_SAVE_DATA_WRITE_ERROR,
	TEXT_BANK_DP_MAIN_MENU_ALERTS,
	TEXT_BANK_DP_POKEMON_STORAGE_SYSTEM,
	TEXT_BANK_DP_BOX_MESSAGES,
	TEXT_BANK_DP_NATURE_NAMES,
	TEXT_BANK_DP_CONTEST_EFFECTS,
	TEXT_BANK_DP_STATUS_CONDITION_NAMES,
	TEXT_BANK_DP_MENU_ENTRIES,
	TEXT_BANK_DP_BATTLE_STRINGS,
	TEXT_BANK_DP_ITEM_DESCRIPTIONS,
	TEXT_BANK_DP_ITEM_NAMES,
	TEXT_BANK_DP_BAG_POCKET_NAMES,
	TEXT_BANK_DP_BAG_POCKET_NAMES_WITH_ICONS,
	TEXT_BANK_DP_SAVE_CORRUPTED,
	TEXT_BANK_DP_SPECIES_NAME,
	TEXT_BANK_DP_NAMING_SCREEN,
	TEXT_BANK_DP_GENERIC_NAMES,
	TEXT_BANK_DP_LOCATION_NAMES,
	TEXT_BANK_DP_MYSTERY_GIFT_EVENT_NAMES,
	TEXT_BANK_DP_SPECIAL_MET_LOCATION_NAMES,
	TEXT_BANK_DP_POKEMON_SUMMARY_SCREEN,
	TEXT_BANK_DP_RIBBONS,
	TEXT_BANK_DP_MAIN_MENU_OPTIONS,
	TEXT_BANK_DP_POKEMON_STAT_NAMES,
	TEXT_BANK_DP_COUNTERPART_NAMES,
	TEXT_BANK_DP_FLAVOR_NAMES,
	TEXT_BANK_DP_ABILITY_NAMES,
	TEXT_BANK_DP_ABILITY_DESCRIPTIONS,
	TEXT_BANK_DP_POKEMON_TYPE_NAMES,
	TEXT_BANK_DP_MOVE_DESCRIPTIONS,
	TEXT_BANK_DP_MOVE_NAMES,
	TEXT_BANK_DP_MONTH_NAMES
};

static u16 sPlatinumUnifiedTextBankLookup[] = {
	TEXT_BANK_PT_SAVE_DATA_READ_ERROR,
	TEXT_BANK_PT_SAVE_DATA_WRITE_ERROR,
	TEXT_BANK_PT_MAIN_MENU_ALERTS,
	TEXT_BANK_PT_POKEMON_STORAGE_SYSTEM,
	TEXT_BANK_PT_BOX_MESSAGES,
	TEXT_BANK_PT_NATURE_NAMES,
	TEXT_BANK_PT_CONTEST_EFFECTS,
	TEXT_BANK_PT_STATUS_CONDITION_NAMES,
	TEXT_BANK_PT_MENU_ENTRIES,
	TEXT_BANK_PT_BATTLE_STRINGS,
	TEXT_BANK_PT_ITEM_DESCRIPTIONS,
	TEXT_BANK_PT_ITEM_NAMES,
	TEXT_BANK_PT_BAG_POCKET_NAMES,
	TEXT_BANK_PT_BAG_POCKET_NAMES_WITH_ICONS,
	TEXT_BANK_PT_SAVE_CORRUPTED,
	TEXT_BANK_PT_SPECIES_NAME,
	TEXT_BANK_PT_NAMING_SCREEN,
	TEXT_BANK_PT_GENERIC_NAMES,
	TEXT_BANK_PT_LOCATION_NAMES,
	TEXT_BANK_PT_MYSTERY_GIFT_EVENT_NAMES,
	TEXT_BANK_PT_SPECIAL_MET_LOCATION_NAMES,
	TEXT_BANK_PT_POKEMON_SUMMARY_SCREEN,
	TEXT_BANK_PT_RIBBONS,
	TEXT_BANK_PT_MAIN_MENU_OPTIONS,
	TEXT_BANK_PT_POKEMON_STAT_NAMES,
	TEXT_BANK_PT_COUNTERPART_NAMES,
	TEXT_BANK_PT_FLAVOR_NAMES,
	TEXT_BANK_PT_ABILITY_NAMES,
	TEXT_BANK_PT_ABILITY_DESCRIPTIONS,
	TEXT_BANK_PT_POKEMON_TYPE_NAMES,
	TEXT_BANK_PT_MOVE_DESCRIPTIONS,
	TEXT_BANK_PT_MOVE_NAMES,
	TEXT_BANK_PT_MONTH_NAMES
};

static void MemCopyEntry(charcode_t* dst, const charcode_t* src, const MessageBankEntry* entry);


static inline int EntryOffset(u32 bankIndex) {
	return sizeof(MessageBank) + sizeof(MessageBankEntry) * bankIndex;
}


static inline void DecodeEntry(MessageBankEntry* entry, u32 entryID, u32 seed) {
	seed = (seed * 765 * (entryID + 1)) & 0xFFFF;
	seed |= seed << 16;
	
	entry->offset ^= seed;
	entry->length ^= seed;
}


static inline void DecodeString(charcode_t* str, u32 len, u32 entryID, u16 seed) {
	seed = (entryID + 1) * KEY_START;
	
	while (len--) {
		*str ^= seed;
		str++;
		seed += KEY_INC;
	}
}


static inline u8* EntryOffsetAddress(const MessageBank* bank, u32 bankIndex) {
	return (u8*)bank + bankIndex;
}


MessageBank* MessageBank_Load(NarcID narcID, u32 bankID, HeapID heapID) {
	return NARC_AllocAndReadWholeMemberByIndexPair(narcID, bankID, heapID);
}


void MessageBank_Free(MessageBank* bank) {
	Heap_Free(bank);
}


void MessageBank_Get(const MessageBank* bank, u32 entryID, charcode_t* dst) {
	if (entryID < bank->count) {
		MessageBankEntry entry = bank->entries[entryID];
		DecodeEntry(&entry, entryID, bank->seed);
		
		const charcode_t* str = (charcode_t*)EntryOffsetAddress(bank, entry.offset);
		MemCopyEntry(dst, str, &entry);
		DecodeString(dst, entry.length, entryID, bank->seed);
		return;
	}
	
	GF_ASSERT(FALSE);
}


void MessageBank_GetFromNARC(NarcID narcID, u32 bankID, u32 entryID, HeapID heapID, charcode_t* dst) {
	NARC* narc = NARC_ctor(narcID, heapID);
	if (narc != NULL) {
		MessageBank bank;
		NARC_ReadFromMember(narc, bankID, 0, sizeof(MessageBank), &bank);
		
		MessageBankEntry entry;
		NARC_ReadFromMember(narc, bankID, EntryOffset(entryID), sizeof(MessageBankEntry), &entry);
		
		DecodeEntry(&entry, entryID, bank.seed);
		
		u32 size = entry.length * sizeof(charcode_t);
		NARC_ReadFromMember(narc, bankID, entry.offset, size, dst);
		
		DecodeString(dst, entry.length, entryID, bank.seed);
		NARC_dtor(narc);
	}
}


static void MemCopyEntry(charcode_t* dst, const charcode_t* src, const MessageBankEntry* entry) {
	MI_CpuCopy16(src, dst, entry->length * sizeof(charcode_t));
}


void MessageBank_GetString(const MessageBank* bank, u32 entryID, String* string) {
	if (entryID < bank->count) {
		MessageBankEntry entry = bank->entries[entryID];
		DecodeEntry(&entry, entryID, bank->seed);
		
		u32 size = entry.length * sizeof(charcode_t);
		charcode_t* cstr = Heap_AllocAtEnd(HEAP_ID_SYSTEM, size);
		if (cstr != NULL) {
			MI_CpuCopy16(EntryOffsetAddress(bank, entry.offset), cstr, size);
			DecodeString(cstr, entry.length, entryID, bank->seed);
			
			String_CopyNumChars(string, cstr, entry.length);
			
			Heap_Free(cstr);
		}
		
		return;
	}
	
	GF_ASSERT(FALSE);
	String_Clear(string);
}


String* MessageBank_GetNewString(const MessageBank* bank, u32 entryID, HeapID heapID) {
	if (entryID < bank->count) {
		MessageBankEntry entry = bank->entries[entryID];
		DecodeEntry(&entry, entryID, bank->seed);
		
		u32 size = entry.length * sizeof(u16);
		charcode_t* cstr = Heap_AllocAtEnd(heapID, size);
		if (cstr != NULL) {
			MI_CpuCopy16(EntryOffsetAddress(bank, entry.offset), cstr, size);
			DecodeString(cstr, entry.length, entryID, bank->seed);
			
			String* string = String_Init(entry.length, heapID);
			if (string) {
				String_CopyNumChars(string, cstr, entry.length);
			}
			
			Heap_Free(cstr);
			return string;
		}
	
		return NULL;
	}
	
	GF_ASSERT(FALSE);
	return String_Init(4, heapID);
}


void MessageBank_GetStringFromHandle(NARC* narc, u32 bankID, u32 entryID, HeapID heapID, String* string) {
	MessageBank bank;
	NARC_ReadFromMember(narc, bankID, 0, sizeof(MessageBank), &bank);
	
	if (entryID < bank.count) {
		MessageBankEntry entry;
		NARC_ReadFromMember(narc, bankID, EntryOffset(entryID), sizeof(MessageBankEntry), &entry);
		DecodeEntry(&entry, entryID, bank.seed);
		
		u32 size = entry.length * sizeof(charcode_t);
		charcode_t* cstr = Heap_AllocAtEnd(heapID, size);
		if (cstr != NULL) {
			NARC_ReadFromMember(narc, bankID, entry.offset, size, cstr);
			DecodeString(cstr, entry.length, entryID, bank.seed);
			
			String_CopyNumChars(string, cstr, entry.length);
			
			Heap_Free(cstr);
		}
		
		return;
	}
	
	GF_ASSERT(FALSE);
	String_Clear(string);
}


String* MessageBank_GetNewStringFromHandle(NARC* narc, u32 bankID, u32 entryID, HeapID heapID) {
	MessageBank bank;
	NARC_ReadFromMember(narc, bankID, 0, sizeof(MessageBank), &bank);
	
	if (entryID < bank.count) {
		MessageBankEntry entry;
		NARC_ReadFromMember(narc, bankID, EntryOffset(entryID), sizeof(MessageBankEntry), &entry);
		DecodeEntry(&entry, entryID, bank.seed);
		
		String* string = String_Init(entry.length, heapID);
		if (string != NULL) {
			u32 size = entry.length * sizeof(charcode_t);
			charcode_t* cstr = Heap_AllocAtEnd(heapID, size);
			if (cstr != NULL) {
				NARC_ReadFromMember(narc, bankID, entry.offset, size, cstr);
				DecodeString(cstr, entry.length, entryID, bank.seed);
				
				String_CopyNumChars(string, cstr, entry.length);
				
				Heap_Free(cstr);
			}
		}
	
		return string;
	}
	
	GF_ASSERT(FALSE);
	return String_Init(4, heapID);
}


u32 MessageBank_EntryCount(const MessageBank* bank) {
	return bank->count;
}


u32 MessageBank_NARCEntryCount(NarcID narcID, u32 bankID) {
	MessageBank bank;
	NARC_ReadFromMemberByIndexPair(&bank, narcID, bankID, 0, sizeof(MessageBank));
	
	return bank.count;
}


MessageLoader* MessageLoader_Init(MessageLoaderMode mode, NarcID narcID, u32 bankID, HeapID heapID) {
	MessageLoader* loader = Heap_AllocAtEnd(heapID, sizeof(MessageLoader));
	if (loader != NULL) {
		if (bankID >= TEXT_BANK_UNIFIED_START) {
			GF_ASSERT(bankID > TEXT_BANK_UNIFIED_START && bankID < TEXT_BANK_UNIFIED_MAX);
			
			bankID -= TEXT_BANK_UNIFIED_SAVE_DATA_READ_ERROR;
			if (gIsDiamondPearl) {
				bankID = sDiamondPearlUnifiedTextBankLookup[bankID];
			} else {
				bankID = sPlatinumUnifiedTextBankLookup[bankID];
			}
		}
		
		if (mode == MSG_LOADER_PRELOAD_ENTIRE_BANK) {
			loader->bank = MessageBank_Load(narcID, bankID, heapID);
			if (loader->bank == NULL) {
				Heap_Free(loader);
				return NULL;
			}
		} else {
			loader->narc = NARC_ctor(narcID, heapID);
		}
		
		loader->mode = mode;
		loader->narcID = narcID;
		loader->bankID = bankID;
		loader->heapID = heapID;
	}
	
	return loader;
}


void MessageLoader_Free(MessageLoader* loader) {
	if (loader != NULL) {
		switch (loader->mode) {
			case MSG_LOADER_PRELOAD_ENTIRE_BANK:
				MessageBank_Free(loader->bank);
				break;
			
			case MSG_LOADER_LOAD_ON_DEMAND:
				NARC_dtor(loader->narc);
				break;
		}
		
		Heap_Free(loader);
	}
}


void MessageLoader_GetString(const MessageLoader* loader, u32 entryID, String* string) {
	switch (loader->mode) {
		case MSG_LOADER_PRELOAD_ENTIRE_BANK:
			MessageBank_GetString(loader->bank, entryID, string);
			break;
		
		case MSG_LOADER_LOAD_ON_DEMAND:
			MessageBank_GetStringFromHandle(loader->narc, loader->bankID, entryID, loader->heapID, string);
			break;
	}
}


String* MessageLoader_GetNewString(const MessageLoader* loader, u32 entryID) {
	switch (loader->mode) {
		case MSG_LOADER_PRELOAD_ENTIRE_BANK:
			return MessageBank_GetNewString(loader->bank, entryID, loader->heapID);
		
		case MSG_LOADER_LOAD_ON_DEMAND:
			return MessageBank_GetNewStringFromHandle(loader->narc, loader->bankID, entryID, loader->heapID);
	}
	
	return NULL;
}


u32 MessageLoader_MessageCount(const MessageLoader* loader) {
	switch (loader->mode) {
		case MSG_LOADER_PRELOAD_ENTIRE_BANK:
			return MessageBank_EntryCount(loader->bank);
		
		case MSG_LOADER_LOAD_ON_DEMAND:
			return MessageBank_NARCEntryCount(loader->narcID, loader->bankID);
	}
	
	return 0;
}


void MessageLoader_Get(const MessageLoader* loader, u32 entryID, charcode_t* dst) {
	switch (loader->mode) {
		case MSG_LOADER_PRELOAD_ENTIRE_BANK:
			MessageBank_Get(loader->bank, entryID, dst);
			break;
		
		case MSG_LOADER_LOAD_ON_DEMAND:
			MessageBank_GetFromNARC(loader->narcID, loader->bankID, entryID, loader->heapID, dst);
			break;
	}
}


void MessageLoader_GetSpeciesName(u32 species, HeapID heapID, charcode_t* dst) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	MessageLoader* loader = MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, TEXT_BANK_UNIFIED_SPECIES_NAME, heapID);
	MessageLoader_Get(loader, species, dst);
	MessageLoader_Free(loader);
}
