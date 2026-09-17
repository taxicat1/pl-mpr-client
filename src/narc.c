#include <nitro.h>
#include <string.h>

#include "narc.h"

#include "assert.h"
#include "constants/versions.h"
#include "heap.h"

#include "fs/narc_dp_fname.dat" // sNarcDPPaths
#include "fs/narc_pt_fname.dat" // sNarcPtPaths

const char** sNarcCurrentPaths = sNarcDPPaths;

static const char* NarcIDToFilePath(NarcID narcID);
static BOOL VersionTolerantOpenFile(FSFile* file, const char* path);
static void ReadFromNarcMemberByPathAndIndex(void* dest, const char* path, int memberIndex, int offset, int bytesToRead);
static void* AllocAndReadFromNarcMemberByPathAndIndex(const char* path, int memberIndex, HeapID heapID, int offset, int bytesToRead, BOOL allocAtEnd);


static const char* NarcIDToFilePath(NarcID narcID) {
	switch (narcID) {
		case NARC_INDEX_DP_POKETOOL__PERSONAL__PERSONAL:
		case NARC_INDEX_DP_POKETOOL__POKEGRA__POKEGRA:
		case NARC_INDEX_DP_GRAPHIC__BAG_GRA:
		case NARC_INDEX_DP_POKETOOL__ICONGRA__POKE_ICON:
		case NARC_INDEX_DP_GRAPHIC__PLIST_GRA:
		case NARC_INDEX_DP_MSGDATA__MSG:
		case NARC_INDEX_DP_BATTLE__GRAPHIC__BATT_OBJ:
		case NARC_INDEX_DP_POKETOOL__POKEANM__POKEANM:
		case NARC_INDEX_DP_POKETOOL__POKEGRA__OTHERPOKE:
		case NARC_INDEX_PL_POKETOOL__POKE_EDIT__PL_POKE_DATA:
			return sNarcCurrentPaths[narcID];
		
		default:
			return sNarcDPPaths[narcID];
	}
}


static BOOL VersionTolerantOpenFile(FSFile* file, const char* path) {
	if (FS_OpenFile(file, path)) {
		return TRUE;
	}
	
	if (strcmp(path, "poketool/personal/personal.narc") == 0) {
		return FS_OpenFile(file, "poketool/personal_pearl/personal.narc");
	}
	
	if (strcmp(path, "graphic/zukan.narc") != 0) {
		return FALSE;
	}
	
	return FS_OpenFile(file, "resource/eng/zukan/zukan.narc");
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
	VersionTolerantOpenFile(&file, path);
	if (!FS_IsFile(&file)) {
		// ?
	}
	
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
	VersionTolerantOpenFile(&file, path);
	if (!FS_IsFile(&file)) {
		// ?
	}
	
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
	VersionTolerantOpenFile(&file, NarcIDToFilePath(narcID));
	if (!FS_IsFile(&file)) {
		// ?
	}
	
	FS_SeekFile(&file, 12, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 2);
	
	btnfStart = btafStart;
	
	FS_SeekFile(&file, btnfStart + 4, FS_SEEK_SET);
	FS_ReadFile(&file, &btafStart, 4);
	FS_ReadFile(&file, &fileCount, 2);
	
	// Bug: file is never closed!
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
	VersionTolerantOpenFile(&file, NarcIDToFilePath(narcID));
	if (!FS_IsFile(&file)) {
		// ?
	}
	
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
	// BUG: file is never closed
	
	return chunkSize;
}


NARC* NARC_ctor(NarcID narcID, HeapID heapID) {
	NARC* narc = Heap_Alloc(heapID, sizeof(NARC));
	if (narc != NULL) {
		u32 btnfStart;
		u32 chunkSize;
		
		narc->fatbStart = 0;
		
		FS_InitFile(&narc->file);
		VersionTolerantOpenFile(&narc->file, NarcIDToFilePath(narcID));
		if (!FS_IsFile(&narc->file)) {
			// ?
		}
		
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


void NARC_SetVersion(u8 gameVersion) {
	switch (gameVersion) {
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
}
