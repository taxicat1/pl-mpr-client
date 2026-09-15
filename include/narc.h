#ifndef MPR_NARC_H
#define MPR_NARC_H

#include "constants/heap.h"
#include "constants/narc.h"

/*
 * The NARC struct wraps NitroSDK type FSFile and manages the important
 * offsets to the FATB and the FIMG chunks within the file.
 *
 * FATB defines the regions within the FIMG to which the data are allocated.
 */
typedef struct {
	FSFile  file;
	u32     fatbStart;
	u32     fimgStart;
	u16     numFiles;
} NARC;

/*
 * Reads the entire content of an archive member into an existing buffer
 *
 * @param dest:           Pointer to destination buffer, should be large enough to hold the data
 * @param narcID:         Index of NARC to read
 * @param memebrIndex:    Index of FAT member within the NARC
 */
void NARC_ReadWholeMemberByIndexPair(void* dest, NarcID narcID, int memberIndex);

/*
 * Creates a new buffer large enough to hold the content of the archive member, then reads its data.
 * The AllocAtEnd variant is preferred for very short-lived buffers.
 *
 * @param narcID:         Index of NARC to read
 * @param memberIndex:    Index of FAT member within the NARC
 * @param heapID:         ID of the heap to alloc from
 *
 * @returns: Pointer to the allocated buffer which contains the data that was read.
 */
void *NARC_AllocAndReadWholeMemberByIndexPair(NarcID narcID, int memberIndex, HeapID heapID);
void *NARC_AllocAtEndAndReadWholeMemberByIndexPair(NarcID narcID, int memberIndex, HeapID heapID);

/*
 * Reads a portion of an archive member into an existing buffer
 *
 * @param dest:           Pointer to destination buffer, should be large enough to hold the data
 * @param narcID:         Index of NARC to read
 * @param memberIndex:    Index of FAT member within the NARC
 * @param offset:         Byte offset to start reading, relative to start of member, up to the end of member
 * @param bytesToRead:    Number of bytes to read, up to the total bytesToRead of member minus offset
 */
void NARC_ReadFromMemberByIndexPair(void* dest, NarcID narcID, int memberIndex, int offset, int bytesToRead);

/*
 * Gets the total number of archive members
 *
 * @param narcID:      Index of NARC to read
 *
 * @returns: Number of archive members
 */
u16 NARC_GetFileCountByIndex(NarcID narcID, BOOL unused);

/*
 * Gets the size of a NARC member. Useful when managing the read buffer yourself and the NARC has variable
 * sized members.
 *
 * @param narcID:         Index of NARC to read
 * @param memberIndex:    Index of FAT member within the NARC
 *
 * @returns: Size in bytes of the member
 */
u32 NARC_GetMemberSizeByIndexPair(NarcID narcID, int memberIndex);

/*
 * Constructs a new NARC which contains an open FSFile to the corresponding archive.
 * Useful to reduce overhead when reading from the same NARC multiple times.
 *
 * @param narcID:       Index of NARC to open
 * @param heapID:       ID of the heap to alloc from
 *
 * @returns: Pointer to the newly-allocated NARC
 */
NARC* NARC_ctor(NarcID narcID, HeapID heapID);

/*
 * Closes the wrapped FSFile and returns the NARC allocation to the heap from whence it came.
 *
 * @param narc:    Pointer to the NARC
 */
void NARC_dtor(NARC* narc);

/*
 * Reads a portion of an archive member into an existing buffer
 *
 * @param narc:           Pointer to the NARC
 * @param memberIndex:    Index of FAT member within the NARC
 * @param offset:         Byte offset to start reading, relative to start of member, up to the end of member
 * @param bytesToRead:    Number of bytes to read, up to the total bytesToRead of member minus offset
 * @param dest:           Pointer to destination buffer, should be large enough to hold the data
 */
void NARC_ReadFromMember(NARC* narc, int memberIndex, u32 offset, u32 bytesToRead, void* dest);

/*
 * Sets future NARCs to use file paths for the selected game version
 *
 * @param gameVersion:    game version to use
 */
void NARC_SetVersion(u8 gameVersion);

#endif /* MPR_NARC_H */
