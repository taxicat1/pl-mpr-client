#include <nitro.h>

#include "string_template.h"

#include "assert.h"
#include "game_version.h"
#include "message.h"
#include "heap.h"
#include "string_gf.h"
#include "charcode.h"
#include "pc_boxes.h"
#include "pokemon.h"
#include "met_location_util.h"

#include "constants/charcode.h"
#include "constants/abilities.h"
#include "constants/pokemon_data_params.h"
#include "constants/text_banks.h"

#include "fs/msgdata/location_names.h"
#include "fs/msgdata/special_met_location_names.h"
#include "fs/msgdata/mystery_gift_event_names.h"

static void InitStringTemplateArgHeader(StringTemplateArgHeader* header);
static void SetStringTemplateArg(StringTemplate* template, u32 idx, const String* argVal, const StringTemplateArgHeader* newHeader);


static inline MessageLoader* InitMessageLoader(u32 bankID, HeapID heapID) {
	NarcID narc;
	if (gIsDiamondPearl) {
		narc = NARC_INDEX_DP_MSGDATA__MSG;
	} else {
		narc = NARC_INDEX_PL_MSGDATA__PL_MSG;
	}
	return MessageLoader_Init(MSG_LOADER_LOAD_ON_DEMAND, narc, bankID, heapID);
}


static inline void SetArgFromArchive(StringTemplate* template, u32 idx, u32 argVal, u32 bankID) {
	MessageLoader* loader = InitMessageLoader(bankID, template->heapID);
	if (loader) {
		MessageLoader_GetString(loader, argVal, template->templateBuf);
		SetStringTemplateArg(template, idx, template->templateBuf, NULL);
		MessageLoader_Free(loader);
	}
}


StringTemplate* StringTemplate_Default(HeapID heapID) {
	return StringTemplate_New(STRING_TEMPLATE_DEFAULT_MAX_ARGS, STRING_TEMPLATE_DEFAULT_MAX_LENGTH, heapID);
}


StringTemplate* StringTemplate_New(u32 maxArgs, u32 maxLen, HeapID heapID) {
	StringTemplate* template = NULL;
	
	GF_ASSERT(maxArgs);
	GF_ASSERT(maxLen);
	
	StringTemplate* tmp = Heap_AllocAtEnd(heapID, sizeof(StringTemplate));
	if (tmp == NULL) {
		goto cleanup;
	}
	
	tmp->maxArgs = maxArgs;
	tmp->heapID = heapID;
	tmp->templateBuf = String_Init(maxLen, heapID);
	if (tmp->templateBuf == NULL) {
		goto cleanup;
	}
	
	tmp->args = Heap_AllocAtEnd(heapID, sizeof(StringTemplateArg) * maxArgs);
	if (tmp->args == NULL) {
		goto cleanup;
	}
	
	u32 arg;
	for (arg = 0; arg < maxArgs; arg++) {
		InitStringTemplateArgHeader(&tmp->args[arg].header);
		tmp->args[arg].string = String_Init(maxLen, heapID);
		
		if (tmp->args[arg].string == NULL) {
			break;
		}
	}
	
	if (arg == maxArgs) {
		return tmp;
	}
	
cleanup:
	if (template) {
		StringTemplate_Free(template);
	}
	
	return NULL;
}


void StringTemplate_Free(StringTemplate* template) {
	GF_ASSERT(template->maxArgs);
	
	if (template->args) {
		for (u32 i = 0; i < template->maxArgs; i++) {
			if (template->args[i].string) {
				String_Free(template->args[i].string);
			} else {
				break;
			}
		}
	
		Heap_Free(template->args);
	}
	
	if (template->templateBuf) {
		String_Free(template->templateBuf);
	}
	
	template->maxArgs = 0;
	Heap_Free(template);
}


static void InitStringTemplateArgHeader(StringTemplateArgHeader* header) {
	return;
}


static void SetStringTemplateArg(StringTemplate* template, u32 idx, const String* argVal, const StringTemplateArgHeader* newHeader) {
	GF_ASSERT(idx < template->maxArgs);
	
	if (idx < template->maxArgs) {
		if (newHeader != NULL) {
			template->args[idx].header = *newHeader;
		}
	
		String_Copy(template->args[idx].string, argVal);
	}
}


void StringTemplate_SetString(StringTemplate* template, u32 idx, const String* argVal, u32 unusedGender, BOOL unused, u32 unusedLanguage) {
	#pragma unused(unusedGender)
	#pragma unused(unused)
	#pragma unused(unusedLanguage)
	SetStringTemplateArg(template, idx, argVal, NULL);
}


void StringTemplate_SetSpeciesName(StringTemplate* template, u32 idx, BoxPokemon* boxMon) {
	u32 bank;
	if (gIsDiamondPearl) {
		bank = TEXT_BANK_DP_SPECIES_NAME;
	} else {
		bank = TEXT_BANK_PT_SPECIES_NAME;
	}
	MessageLoader* loader = InitMessageLoader(bank, template->heapID);
	u32 species = BoxPokemon_GetValue(boxMon, MON_DATA_SPECIES, NULL);
	
	MessageLoader_GetString(loader, species, template->templateBuf);
	SetStringTemplateArg(template, idx, template->templateBuf, NULL);
	MessageLoader_Free(loader);
}


void StringTemplate_SetNickname(StringTemplate* template, u32 idx, BoxPokemon* boxMon) {
	BoxPokemon_GetValue(boxMon, MON_DATA_NICKNAME_STRING, template->templateBuf);
	SetStringTemplateArg(template, idx, template->templateBuf, NULL);
}


void StringTemplate_SetOTName(StringTemplate* template, u32 idx, BoxPokemon* boxMon) {
	BoxPokemon_GetValue(boxMon, MON_DATA_OT_NAME_STRING, template->templateBuf);
	SetStringTemplateArg(template, idx, template->templateBuf, NULL);
}


void StringTemplate_SetNumber(StringTemplate* template, u32 idx, int num, u32 maxDigits, PaddingMode paddingMode, CharsetMode charsetMode) {
	String_FormatInt(template->templateBuf, num, maxDigits, paddingMode, charsetMode);
	SetStringTemplateArg(template, idx, template->templateBuf, NULL);
}


void StringTemplate_SetAbilityName(StringTemplate* template, u32 idx, Ability ability) {
	// BUG: use unified access
	u32 bank;
	if (gIsDiamondPearl) {
		bank = TEXT_BANK_DP_ABILITY_NAMES;
	} else {
		bank = TEXT_BANK_PT_ABILITY_NAMES;
	}
	SetArgFromArchive(template, idx, ability, bank);
}


void StringTemplate_SetItemName(StringTemplate* template, u32 idx, u32 item) {
	// BUG: use unified access
	u32 bank;
	if (gIsDiamondPearl) {
		bank = TEXT_BANK_DP_ITEM_NAMES;
	} else {
		bank = TEXT_BANK_PT_ITEM_NAMES;
	}
	SetArgFromArchive(template, idx, item, bank);
}


void StringTemplate_SetPCBoxName(StringTemplate* template, u32 idx, const PCBoxes* pcBoxes, u32 boxIdx) {
	PCBoxes_BufferBoxName(pcBoxes, boxIdx, template->templateBuf);
	SetStringTemplateArg(template, idx, template->templateBuf, NULL);
}


void StringTemplate_SetMetLocationName(StringTemplate* strTemplate, u32 idx, u32 location) {
	// BUG: use unified access
	static const u16 MetLocationBanksDP[] = {
		TEXT_BANK_DP_LOCATION_NAMES,
		TEXT_BANK_DP_SPECIAL_MET_LOCATION_NAMES,
		TEXT_BANK_DP_MYSTERY_GIFT_EVENT_NAMES
	};
	
	static const u16 MetLocationBanksPt[] = {
		TEXT_BANK_PT_LOCATION_NAMES,
		TEXT_BANK_PT_SPECIAL_MET_LOCATION_NAMES,
		TEXT_BANK_PT_MYSTERY_GIFT_EVENT_NAMES
	};
	
	int metLocationType = MetLoc_GetBankType(location);
	int metLocationEntry = MetLoc_GetBankEntry(location);
	u32 bank;
	if (gIsDiamondPearl) {
		bank = MetLocationBanksDP[metLocationType];
	} else {
		bank = MetLocationBanksPt[metLocationType];
	}
	MessageLoader* loader = InitMessageLoader(bank, strTemplate->heapID);
	
	if (loader) {
		if (metLocationEntry < MessageLoader_MessageCount(loader) && 
			(gIsDiamondPearl || (!(metLocationType == 0 && metLocationEntry == 0)))
		) {
			MessageLoader_GetString(loader, metLocationEntry, strTemplate->templateBuf);
			SetStringTemplateArg(strTemplate, idx, strTemplate->templateBuf, NULL);
			MessageLoader_Free(loader);
		} else {
			MessageLoader_Free(loader);
			
			if (gIsDiamondPearl) {
				// BUG: use table immediately above
				loader = InitMessageLoader(TEXT_BANK_DP_LOCATION_NAMES, strTemplate->heapID);
				if (loader) {
					MessageLoader_GetString(loader, LOCATION_NAME_MysteryZone, strTemplate->templateBuf);
					SetStringTemplateArg(strTemplate, idx, strTemplate->templateBuf, NULL);
					MessageLoader_Free(loader);
				}
			} else {
				// BUG: use table immediately above
				u32 bankID, msgID;
				if (metLocationType == 0 && metLocationEntry == 0) {
					bankID = TEXT_BANK_PT_SPECIAL_MET_LOCATION_NAMES;
					msgID = SPECIAL_METLOC_NAME_MysteryZone;
				} else {
					bankID = TEXT_BANK_PT_MYSTERY_GIFT_EVENT_NAMES;
					msgID = MYSTERY_GIFT_EVENT_NAME_FarawayPlace;
				}
				
				SetArgFromArchive(strTemplate, idx, msgID, bankID);
			}
		}
	}
}


void StringTemplate_Format(const StringTemplate* template, String* dst, const String* fmtString) {
	const charcode_t* c = String_GetData(fmtString);
	String_Clear(dst);
	
	while (*c != CHAR_EOS) {
		if (*c == CHAR_FORMAT_ARG) {
			if (CharCode_IsFormatArg(c)) {
				u32 idx = CharCode_FormatArgParam(c, 0);
				GF_ASSERT(idx < template->maxArgs);
				
				String_Concat(dst, template->args[idx].string);
				c = CharCode_SkipFormatArg(c);
			} else {
				const charcode_t* tmp = c;
				c = CharCode_SkipFormatArg(c);
				
				while (tmp < c) {
					String_AppendChar(dst, *tmp++);
				}
			}
		} else {
			String_AppendChar(dst, *c++);
		}
	}
}
