#include <nitro.h>

#include "save_table.h"

#include "common.h"
#include "system_data.h"
#include "save_player.h"
#include "party.h"
#include "bag.h"
#include "vars_flags.h"
#include "poketch.h"
#include "field_overworld_state.h"
#include "pokedex.h"
#include "daycare_save.h"
#include "pal_pad.h"
#include "savedata_misc.h"
#include "underground.h"
#include "battle_regulation.h"
#include "image_clips.h"
#include "mail.h"
#include "poffin.h"
#include "record_mixed_rng.h"
#include "journal.h"
#include "trainer_case_save_data.h"
#include "game_records.h"
#include "seal_case.h"
#include "chatot_cry.h"
#include "battle_frontier_save.h"
#include "special_ribbons.h"
#include "special_encounter.h"
#include "global_trade.h"
#include "tv_broadcast.h"
#include "rankings.h"
#include "wifi_list.h"
#include "wifi_history_save_data.h"
#include "mystery_gift.h"
#include "pal_park_transfers.h"
#include "link_contest_records.h"
#include "easy_chat_words.h"
#include "email.h"
#include "wifi_questions.h"
#include "pc_boxes.h"

typedef int (*SaveEntrySizeFunc)(void);
typedef void (*SaveEntryInitFunc)(void*);

const SaveTableEntry gSaveTable[] = {
	{
		SAVE_TABLE_ENTRY_SYSTEM,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)SystemData_GetSaveSize,
		(SaveEntryInitFunc)SystemData_Init
	},
	{
		SAVE_TABLE_ENTRY_PLAYER,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Player_GetSaveSize,
		(SaveEntryInitFunc)Player_Init
	},
	{
		SAVE_TABLE_ENTRY_PARTY,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Party_GetSaveSize,
		(SaveEntryInitFunc)Party_Init
	},
	{
		SAVE_TABLE_ENTRY_BAG,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Bag_GetSaveSize,
		(SaveEntryInitFunc)Bag_Init
	},
	{
		SAVE_TABLE_ENTRY_VARS_FLAGS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)VarsFlags_GetSaveSize,
		(SaveEntryInitFunc)VarsFlags_Init
	},
	{
		SAVE_TABLE_ENTRY_POKETCH,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Poketch_GetSaveSize,
		(SaveEntryInitFunc)Poketch_Init
	},
	{
		SAVE_TABLE_ENTRY_FIELD_PLAYER_STATE,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)FieldOverworldState_GetSaveSize,
		(SaveEntryInitFunc)FieldOverworldState_Init
	},
	{
		SAVE_TABLE_ENTRY_POKEDEX,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Pokedex_GetSaveSize,
		(SaveEntryInitFunc)Pokedex_Init
	},
	{
		SAVE_TABLE_ENTRY_DAYCARE,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Daycare_GetSaveSize,
		(SaveEntryInitFunc)Daycare_Init
	},
	{
		SAVE_TABLE_ENTRY_PAL_PAD,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)PalPad_GetSaveSize,
		(SaveEntryInitFunc)PalPad_Init
	},
	{
		SAVE_TABLE_ENTRY_MISC,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)MiscSaveBlock_GetSaveSize,
		(SaveEntryInitFunc)MiscSaveBlock_Init
	},
	{
		SAVE_TABLE_ENTRY_FIELD_OVERWORLD_STATE,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)FieldOverworldSave_GetSaveSize,
		(SaveEntryInitFunc)FieldOverworldSave_Init
	},
	{
		SAVE_TABLE_ENTRY_UNDERGROUND,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Underground_GetSaveSize,
		(SaveEntryInitFunc)Underground_Init
	},
	{
		SAVE_TABLE_ENTRY_REGULATION_BATTLES,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)BattleRegulation_GetSaveSize,
		(SaveEntryInitFunc)BattleRegulation_Init
	},
	{
		SAVE_TABLE_ENTRY_IMAGE_CLIPS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)ImageClips_GetSaveSize,
		(SaveEntryInitFunc)ImageClips_Init
	},
	{
		SAVE_TABLE_ENTRY_MAILBOX,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Mailbox_GetSaveSize,
		(SaveEntryInitFunc)Mailbox_Init
	},
	{
		SAVE_TABLE_ENTRY_POFFINS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)PoffinCase_GetSaveSize,
		(SaveEntryInitFunc)PoffinCase_Init
	},
	{
		SAVE_TABLE_ENTRY_RECORD_MIXED_RNG,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)RecordMixedRNG_GetSaveSize,
		(SaveEntryInitFunc)RecordMixedRNG_Init
	},
	{
		SAVE_TABLE_ENTRY_JOURNAL,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Journal_GetSaveSize,
		(SaveEntryInitFunc)Journal_Init
	},
	{
		SAVE_TABLE_ENTRY_TRAINER_CASE,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)TrainerCaseSaveData_GetSaveSize,
		(SaveEntryInitFunc)TrainerCaseSaveData_Init
	},
	{
		SAVE_TABLE_ENTRY_GAME_RECORDS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)GameRecords_GetSaveSize,
		(SaveEntryInitFunc)GameRecords_Init
	},
	{
		SAVE_TABLE_ENTRY_SEAL_CASE,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)SealCase_GetSaveSize,
		(SaveEntryInitFunc)SealCase_Init
	},
	{
		SAVE_TABLE_ENTRY_CHATOT,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)ChatotCry_GetSaveSize,
		(SaveEntryInitFunc)ChatotCry_Init
	},
	{
		SAVE_TABLE_ENTRY_FRONTIER,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)BattleFrontier_GetSaveSize,
		(SaveEntryInitFunc)BattleFrontier_Init
	},
	{
		SAVE_TABLE_ENTRY_RIBBONS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)SpecialRibbons_GetSaveSize,
		(SaveEntryInitFunc)SpecialRibbons_Init
	},
	{
		SAVE_TABLE_ENTRY_ENCOUNTERS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)SpecialEncounter_GetSaveSize,
		(SaveEntryInitFunc)SpecialEncounter_Init
	},
	{
		SAVE_TABLE_ENTRY_GLOBAL_TRADE,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)GlobalTrade_GetSaveSize,
		(SaveEntryInitFunc)GlobalTrade_Init
	},
	{
		SAVE_TABLE_ENTRY_TV_BROADCAST,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)TVBroadcast_GetSaveSize,
		(SaveEntryInitFunc)TVBroadcast_Init
	},
	{
		SAVE_TABLE_ENTRY_RANKINGS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Rankings_GetSaveSize,
		(SaveEntryInitFunc)Rankings_Init
	},
	{
		SAVE_TABLE_ENTRY_WIFI_LIST,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)WiFiList_GetSaveSize,
		(SaveEntryInitFunc)WiFiList_Init
	},
	{
		SAVE_TABLE_ENTRY_WIFI_HISTORY,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)WiFiHistory_GetSaveSize,
		(SaveEntryInitFunc)WiFiHistory_Init
	},
	{
		SAVE_TABLE_ENTRY_MYSTERY_GIFT,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)MysteryGift_GetSaveSize,
		(SaveEntryInitFunc)MysteryGift_Init
	},
	{
		SAVE_TABLE_ENTRY_PAL_PARK_TRANSFER,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)MigratedPokemon_GetSaveSize,
		(SaveEntryInitFunc)MigratedPokemon_Init
	},
	{
		SAVE_TABLE_ENTRY_LINK_CONTEST_RECORDS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)LinkContestRecords_GetSaveSize,
		(SaveEntryInitFunc)LinkContestRecords_Init
	},
	{
		SAVE_TABLE_ENTRY_UNLOCKED_EASY_CHAT_WORDS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)UnlockedEasyChatWords_GetSaveSize,
		(SaveEntryInitFunc)UnlockedEasyChatWords_Init
	},
	{
		SAVE_TABLE_ENTRY_EMAIL,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)Email_GetSaveSize,
		(SaveEntryInitFunc)Email_Init
	},
	{
		SAVE_TABLE_ENTRY_WIFI_QUESTIONS,
		SAVE_BLOCK_ID_NORMAL,
		(SaveEntrySizeFunc)WiFiQuestion_GetSaveSize,
		(SaveEntryInitFunc)WiFiQuestion_Init
	},
	{
		SAVE_TABLE_ENTRY_PC_BOXES,
		SAVE_BLOCK_ID_BOXES,
		(SaveEntrySizeFunc)PCBoxes_GetSaveSize,
		(SaveEntryInitFunc)PCBoxes_Init
	}
};

const int gSaveTableSize = NELEMS(gSaveTable);


PCBoxes* SaveData_GetPCBoxes(SaveData* saveData) {
	return SaveData_GetSaveTable(saveData, SAVE_TABLE_ENTRY_PC_BOXES);
}
