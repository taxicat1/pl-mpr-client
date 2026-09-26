#include <nitro.h>
#include <string.h>

#include "narc.h"

#include "assert.h"
#include "game_version.h"
#include "heap.h"

#include "fs/narc_dp_fname.dat" // sNarcDPPaths
#include "fs/narc_pt_fname.dat" // sNarcPtPaths

char** sNarcCurrentPaths = sNarcDPPaths;

#define NARC_LIST_TERMINATOR  ((NarcID)(-1))

typedef struct {
	NarcID  narc;
	char*   path;
} NarcPathReplacement;

static NarcPathReplacement sNonEnglishNarcsDP[] = {
	{ NARC_INDEX_DP_BATTLE__GRAPHIC__B_PLIST_GRA,        "resource/XXX/b_plist/b_plist_gra.narc"             },
	{ NARC_INDEX_DP_GRAPHIC__BAG_GRA,                    "resource/XXX/bag/bag_gra.narc"                     },
	{ NARC_INDEX_DP_BATTLE__GRAPHIC__BATT_OBJ,           "resource/XXX/battle_graphic/batt_obj.narc"         },
	{ NARC_INDEX_DP_GRAPHIC__BOX,                        "resource/XXX/box/box.narc"                         },
	{ NARC_INDEX_DP_CONTEST__GRAPHIC__CONTEST_BG,        "resource/XXX/contest_graphic/contest_bg.narc"      },
	{ NARC_INDEX_DP_CONTEST__GRAPHIC__CONTEST_OBJ,       "resource/XXX/contest_graphic/contest_obj.narc"     },
	{ NARC_INDEX_DP_GRAPHIC__IMAGECLIP,                  "resource/XXX/dress_up_graphic/imageclip.narc"      },
	{ NARC_INDEX_DP_GRAPHIC__MYSTERY,                    "resource/XXX/mysterycard/mystery.narc"             },
	{ NARC_INDEX_DP_DATA__NAMEIN,                        "resource/XXX/nameinput/namein.narc"                },
	{ NARC_INDEX_DP_GRAPHIC__NUTMIXER,                   "resource/XXX/nutmixer/nutmixer.narc"               },
	{ NARC_INDEX_DP_DEMO__TITLE__OP_DEMO,                "resource/XXX/opening_demo/op_demo.narc"            },
	{ NARC_INDEX_DP_GRAPHIC__PST_GRA,                    "resource/XXX/p_status/pst_gra.narc"                },
	{ NARC_INDEX_DP_GRAPHIC__PLIST_GRA,                  "resource/XXX/pokelist/plist_gra.narc"              },
	{ NARC_INDEX_DP_FIELDDATA__POKEMON_TRADE__FLD_TRADE, "resource/XXX/pokemon_trade/fld_trade.narc"         },
	{ NARC_INDEX_DP_GRAPHIC__POKETCH,                    "resource/XXX/poketch/poketch.narc"                 },
	{ NARC_INDEX_DP_DATA__SLOT,                          "resource/XXX/slot/slot.narc"                       },
	{ NARC_INDEX_DP_DEMO__TITLE__TITLEDEMO,              "resource/XXX/title/titledemo.narc"                 },
	{ NARC_INDEX_DP_GRAPHIC__TOUCH_SUBWINDOW,            "resource/XXX/touch_subwindow/touch_subwindow.narc" },
	{ NARC_INDEX_DP_GRAPHIC__TRAINER_CASE,               "resource/XXX/trainer_case/trainer_case.narc"       },
	{ NARC_INDEX_DP_BATTLE__GRAPHIC__VS_DEMO_GRA,        "resource/XXX/vs_demo/vs_demo_gra.narc"             },
	
	// Already in resource/eng/, only need to format the region code without supplying a new path
	{ NARC_INDEX_DP_GRAPHIC__ZUKAN, NULL },
	
	{ NARC_LIST_TERMINATOR }
};

static NarcPathReplacement sNonEnglishNarcsPt[] = {
	{ NARC_INDEX_PL_BATTLE__GRAPHIC__PL_B_PLIST_GRA,     "resource/XXX/b_plist/pl_b_plist_gra.narc"          },
	{ NARC_INDEX_PL_GRAPHIC__PL_BAG_GRA,                 "resource/XXX/bag/pl_bag_gra.narc"                  },
	{ NARC_INDEX_PL_BATTLE__GRAPHIC__PL_BATT_OBJ,        "resource/XXX/battle_graphic/pl_batt_obj.narc"      },
	{ NARC_INDEX_PL_GRAPHIC__BOX,                        "resource/XXX/box/box.narc"                         },
	{ NARC_INDEX_PL_CONTEST__GRAPHIC__CONTEST_BG,        "resource/XXX/contest_graphic/contest_bg.narc"      },
	{ NARC_INDEX_PL_CONTEST__GRAPHIC__CONTEST_OBJ,       "resource/XXX/contest_graphic/contest_obj.narc"     },
	{ NARC_INDEX_PL_GRAPHIC__IMAGECLIP,                  "resource/XXX/dress_up_graphic/imageclip.narc"      },
	{ NARC_INDEX_PL_GRAPHIC__MYSTERY,                    "resource/XXX/mysterycard/mystery.narc"             },
	{ NARC_INDEX_PL_DATA__NAMEIN,                        "resource/XXX/nameinput/namein.narc"                },
	{ NARC_INDEX_PL_GRAPHIC__NUTMIXER,                   "resource/XXX/nutmixer/nutmixer.narc"               },
	{ NARC_INDEX_PL_DEMO__TITLE__OP_DEMO,                "resource/XXX/opening_demo/op_demo.narc"            },
	{ NARC_INDEX_PL_GRAPHIC__PL_PST_GRA,                 "resource/XXX/p_status/pl_pst_gra.narc"             },
	{ NARC_INDEX_PL_GRAPHIC__PL_PLIST_GRA,               "resource/XXX/pokelist/pl_plist_gra.narc"           },
	{ NARC_INDEX_PL_FIELDDATA__POKEMON_TRADE__FLD_TRADE, "resource/XXX/pokemon_trade/fld_trade.narc"         },
	{ NARC_INDEX_PL_GRAPHIC__POKETCH,                    "resource/XXX/poketch/poketch.narc"                 },
	{ NARC_INDEX_PL_DATA__SLOT,                          "resource/XXX/slot/slot.narc"                       },
	{ NARC_INDEX_PL_DEMO__TITLE__TITLEDEMO,              "resource/XXX/title/titledemo.narc"                 },
	{ NARC_INDEX_PL_GRAPHIC__TOUCH_SUBWINDOW,            "resource/XXX/touch_subwindow/touch_subwindow.narc" },
	{ NARC_INDEX_PL_GRAPHIC__TRAINER_CASE,               "resource/XXX/trainer_case/trainer_case.narc"       },
	{ NARC_INDEX_PL_BATTLE__GRAPHIC__VS_DEMO_GRA,        "resource/XXX/vs_demo/vs_demo_gra.narc"             },
	
	// Already in resource/eng/, only need to format the region code without supplying a new path
	{ NARC_INDEX_PL_ARC__BATT_REC_GRA,               NULL },
	{ NARC_INDEX_PL_FRONTIER__GRAPHIC__FRONTIER_BG,  NULL },
	{ NARC_INDEX_PL_FRONTIER__GRAPHIC__FRONTIER_OBJ, NULL },
	{ NARC_INDEX_PL_ARC__PMS_AIKOTOBA,               NULL },
	{ NARC_INDEX_PL_GRAPHIC__SCRATCH,                NULL },
	{ NARC_INDEX_PL_GRAPHIC__WLMNGM_TOOL,            NULL },
	{ NARC_INDEX_PL_GRAPHIC__ZUKAN,                  NULL },
	
	{ NARC_LIST_TERMINATOR }
};

static const char* NarcIDToFilePath(NarcID narcID);
static void ReadFromNarcMemberByPathAndIndex(void* dest, const char* path, int memberIndex, int offset, int bytesToRead);
static void* AllocAndReadFromNarcMemberByPathAndIndex(const char* path, int memberIndex, HeapID heapID, int offset, int bytesToRead, BOOL allocAtEnd);
static void NarcPathFormatLanguage(char* path);


static const char* NarcIDToFilePath(NarcID narcID) {
	return sNarcCurrentPaths[narcID];
}


static void ReadFromNarcMemberByPathAndIndex(void* dest, const char* path, int memberIndex, int offset, int bytesToRead) {
	FSFile file;
	u32 btafStart = 0;
	u32 btnfStart = 0;
	u32 gmifStart = 0;
	u32 chunkSize = 0;
	u32 fileStart = 0;
	u32 fileEnd = 0;
	u16 fileCount = 0;
	
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	
	FS_SeekFile(&file, 12, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 2);
	
	btnfStart = btafStart;
	
	FS_SeekFile(&file, btnfStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 4);
	FS_ReadFile(&file, &fileCount, 2);
	
	GF_ASSERT(fileCount > memberIndex);
	
	gmifStart = btnfStart + btafStart;
	
	FS_SeekFile(&file, gmifStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 4);
	
	chunkSize = gmifStart + btafStart;
	
	FS_SeekFile(&file, btnfStart + 12 + memberIndex * 8, FS_SEEK_SET);
	FS_ReadFile(&file, &fileStart, 4);
	FS_ReadFile(&file, &fileEnd, 4);
	FS_SeekFile(&file, chunkSize + 8 + fileStart + offset, FS_SEEK_SET);
	
	if (bytesToRead != 0) {
		btafStart = bytesToRead;
	} else {
		btafStart = fileEnd - fileStart;
	}
	
	GF_ASSERT(btafStart != 0);
	
	FS_ReadFile(&file, dest, btafStart);
	FS_CloseFile(&file);
}


static void* AllocAndReadFromNarcMemberByPathAndIndex(const char* path, int memberIndex, HeapID heapID, int offset, int bytesToRead, BOOL allocAtEnd) {
	FSFile file;
	u32 btafStart = 0;
	u32 btnfStart = 0;
	u32 gmifStart = 0;
	u32 chunkSize = 0;
	u32 fileStart = 0;
	u32 fileEnd = 0;
	void* dest;
	u16 fileCount = 0;
	
	FS_InitFile(&file);
	FS_OpenFile(&file, path);
	
	FS_SeekFile(&file, 12, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 2);
	
	btnfStart = btafStart;
	
	FS_SeekFile(&file, btnfStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 4);
	FS_ReadFile(&file, &fileCount, 2);
	
	GF_ASSERT(fileCount > memberIndex);
	
	gmifStart = btnfStart + btafStart;
	
	FS_SeekFile(&file, gmifStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 4);
	
	chunkSize = gmifStart + btafStart;
	
	FS_SeekFile(&file, btnfStart + 12 + memberIndex * 8, FS_SEEK_SET);
	FS_ReadFile(&file, &fileStart, 4);
	FS_ReadFile(&file, &fileEnd, 4);
	FS_SeekFile(&file, chunkSize + 8 + fileStart + offset, FS_SEEK_SET);
	
	if (bytesToRead != 0) {
		btafStart = bytesToRead;
	} else {
		btafStart = fileEnd - fileStart;
	}
	
	GF_ASSERT(btafStart != 0);
	
	if (allocAtEnd == FALSE) {
		dest = Heap_Alloc(heapID, btafStart);
	} else {
		dest = Heap_AllocAtEnd(heapID, btafStart);
	}
	
	FS_ReadFile(&file, dest, btafStart);
	FS_CloseFile(&file);
	
	return dest;
}


void NARC_ReadWholeMemberByIndexPair(void* dest, NarcID narcID, int memberIndex) {
	ReadFromNarcMemberByPathAndIndex(dest, NarcIDToFilePath(narcID), memberIndex, 0, 0);
}


void* NARC_AllocAndReadWholeMemberByIndexPair(NarcID narcID, int memberIndex, HeapID heapID) {
	return AllocAndReadFromNarcMemberByPathAndIndex(NarcIDToFilePath(narcID), memberIndex, heapID, 0, 0, FALSE);
}


void* NARC_AllocAtEndAndReadWholeMemberByIndexPair(NarcID narcID, int memberIndex, HeapID heapID) {
	return AllocAndReadFromNarcMemberByPathAndIndex(NarcIDToFilePath(narcID), memberIndex, heapID, 0, 0, TRUE);
}


void NARC_ReadFromMemberByIndexPair(void* dest, NarcID narcID, int memberIndex, int offset, int bytesToRead) {
	ReadFromNarcMemberByPathAndIndex(dest, NarcIDToFilePath(narcID), memberIndex, offset, bytesToRead);
}


u16 NARC_GetFileCountByIndex(NarcID narcID, BOOL unused) {
	#pragma unused(unused)
	
	FSFile file;
	u32 btafStart = 0;
	u32 btnfStart = 0;
	u16 fileCount = 0;
	
	FS_InitFile(&file);
	FS_OpenFile(&file, NarcIDToFilePath(narcID));
	
	FS_SeekFile(&file, 12, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 2);
	
	btnfStart = btafStart;
	
	FS_SeekFile(&file, btnfStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 4);
	FS_ReadFile(&file, &fileCount, 2);
	
	FS_CloseFile(&file);
	return fileCount;
}


u32 NARC_GetMemberSizeByIndexPair(NarcID narcID, int memberIndex) {
	FSFile file;
	u32 chunkSize = 0;
	u32 btafStart = 0;
	u32 btnfStart = 0;
	u32 gmifStart = 0;
	u32 fileStart = 0;
	u32 fileEnd = 0;
	u16 fileCount = 0;
	
	FS_InitFile(&file);
	FS_OpenFile(&file, NarcIDToFilePath(narcID));
	
	FS_SeekFile(&file, 12, FS_SEEK_SET);
	FS_ReadFile(&file, &chunkSize, 2);
	
	btafStart = chunkSize;
	
	FS_SeekFile(&file, btafStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &chunkSize, 4);
	FS_ReadFile(&file, &fileCount, 2);
	
	GF_ASSERT(fileCount > memberIndex);
	
	btnfStart = btafStart + chunkSize;
	
	FS_SeekFile(&file, btnfStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &chunkSize, 4);
	
	gmifStart = btnfStart + chunkSize;
	
	FS_SeekFile(&file, btafStart + 12 + memberIndex * 8, FS_SEEK_SET);
	FS_ReadFile(&file, &fileStart, 4);
	FS_ReadFile(&file, &fileEnd, 4);
	FS_SeekFile(&file, gmifStart + 8 + fileStart, FS_SEEK_SET);
	
	chunkSize = fileEnd - fileStart;
	
	GF_ASSERT(chunkSize != 0);
	
	FS_CloseFile(&file);
	
	return chunkSize;
}


NARC* NARC_ctor(NarcID narcID, HeapID heapID) {
	NARC* narc = Heap_Alloc(heapID, sizeof(NARC));
	if (narc != NULL) {
		u32 btnfStart;
		u32 chunkSize;
		
		narc->fatbStart = 0;
		
		FS_InitFile(&narc->file);
		FS_OpenFile(&narc->file, NarcIDToFilePath(narcID));
		
		FS_SeekFile(&narc->file, 12, FS_SEEK_SET);
		FS_ReadFile(&narc->file, &narc->fatbStart, 2);
		FS_SeekFile(&narc->file, narc->fatbStart + 4, FS_SEEK_SET);
		FS_ReadFile(&narc->file, &chunkSize, 4);
		FS_ReadFile(&narc->file, &narc->numFiles, 2);
		
		btnfStart = narc->fatbStart + chunkSize;
		
		FS_SeekFile(&narc->file, btnfStart + 4, FS_SEEK_SET);
		FS_ReadFile(&narc->file, &chunkSize, 4);
		
		narc->fimgStart = btnfStart + chunkSize;
	}
	
	return narc;
}


void NARC_dtor(NARC* narc) {
	FS_CloseFile(&narc->file);
	Heap_Free(narc);
}


void NARC_ReadFromMember(NARC* narc, int memberIndex, u32 offset, u32 bytesToRead, void* dest) {
	u32 fileStart;
	
	GF_ASSERT((u32)narc->numFiles > memberIndex);
	
	FS_SeekFile(&narc->file, narc->fatbStart + 12 + memberIndex * 8, FS_SEEK_SET);
	FS_ReadFile(&narc->file, &fileStart, 4);
	FS_SeekFile(&narc->file, narc->fimgStart + 8 + fileStart + offset, FS_SEEK_SET);
	FS_ReadFile(&narc->file, dest, bytesToRead);
}


static void NarcPathFormatLanguage(char* path) {
	char a, b, c;
	
	switch (gGameLanguage) {
		case LANGUAGE_ENGLISH:
			a = 'e', b = 'n', c = 'g';
			break;
		
		case LANGUAGE_GERMAN:
			a = 'g', b = 'e', c = 'r';
			break;
		
		case LANGUAGE_ITALIAN:
			a = 'i', b = 't', c = 'a';
			break;
		
		case LANGUAGE_SPANISH:
			a = 's', b = 'p', c = 'a';
			break;
		
		case LANGUAGE_FRENCH:
			a = 'f', b = 'r', c = 'a';
			break;
		
		default:
			return;
	}
	
	path[9]  = a;
	path[10] = b;
	path[11] = c;
}


void NARC_SetVersion(void) {
	static BOOL narcVersionSet = FALSE;
	
	if (!narcVersionSet) {
		switch (gGameVersion) {
			case VERSION_DIAMOND:
				sNarcDPPaths[NARC_INDEX_DP_POKETOOL__PERSONAL__PERSONAL] = "poketool/personal/personal.narc";
				sNarcCurrentPaths = sNarcDPPaths;
				break;
			
			case VERSION_PEARL:
				sNarcDPPaths[NARC_INDEX_DP_POKETOOL__PERSONAL__PERSONAL] = "poketool/personal_pearl/personal.narc";
				sNarcCurrentPaths = sNarcDPPaths;
				break;
			
			case VERSION_PLATINUM:
				sNarcCurrentPaths = sNarcPtPaths;
				break;
		}
		
		NarcPathReplacement* narcList = NULL;
		
		switch (gGameLanguage) {
			case LANGUAGE_ENGLISH:
				// Default
				break;
			
			case LANGUAGE_GERMAN:
			case LANGUAGE_ITALIAN:
			case LANGUAGE_SPANISH:
			case LANGUAGE_FRENCH:
				switch (gGameVersion) {
					case VERSION_DIAMOND:
					case VERSION_PEARL:
						narcList = sNonEnglishNarcsDP;
						break;
					
					case VERSION_PLATINUM:
						narcList = sNonEnglishNarcsPt;
						break;
					
					default:
						break;
				}
				break;
			
			default:
				break;
		}
		
		if (narcList != NULL) {
			for (int i = 0; narcList[i].narc != NARC_LIST_TERMINATOR; i++) {
				if (narcList[i].path != NULL) {
					sNarcCurrentPaths[narcList[i].narc] = narcList[i].path;
				}
				
				NarcPathFormatLanguage(sNarcCurrentPaths[narcList[i].narc]);
			}
		}
	}
	
	narcVersionSet = TRUE;
}
