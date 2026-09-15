#include <nitro.h>
#include <string.h>

#include "image_clips.h"

#include "assert.h"

#define PHOTO_EMPTY_MAGIC  0x1234

static void BackdropFlags_SetCount(u32* flags, u8 count, u8 backdropID);


static void BackdropFlags_SetCount(u32* flags, u8 count, u8 backdropID) {
	GF_ASSERT(count <= BACKDROP_COUNT);
	
	u8 word = backdropID / 4;
	u8 byte = backdropID % 4;
	
	byte <<= 3;
	
	flags[word] &= ~(0xFF << byte);
	flags[word] |= (count << byte);
}


static inline void DressUpPhoto_InitInternal(DressUpPhoto* photo) {
	memset(photo, 0, sizeof(DressUpPhoto));
	photo->integrity = PHOTO_EMPTY_MAGIC;
}


static inline void UnkImageClipsSub_InitInternal(UnkImageClipsSub* sub) {
	memset(sub, 0, sizeof(UnkImageClipsSub));
	sub->integrity = PHOTO_EMPTY_MAGIC;
}


static inline void FashionCase_Init(FashionCase* fashionCase) {
	memset(fashionCase, 0, sizeof(FashionCase));
	for (int i = 0; i < BACKDROP_COUNT; i++) {
		BackdropFlags_SetCount(fashionCase->backdropFlags, BACKDROP_COUNT, i);
	}
}


void ImageClips_Init(ImageClips* imageClips) {
	int i;
	for (i = 0; i < SAVED_PHOTOS_COUNT; i++) {
		DressUpPhoto_InitInternal(&imageClips->savedPhotos[i]);
	}
	for (i = 0; i < IC_UNKSUBSTRUCT_COUNT; i++) {
		UnkImageClipsSub_InitInternal(&imageClips->unk_4C8[i]);
	}
	
	FashionCase_Init(&imageClips->fashionCase);
}


int ImageClips_GetSaveSize(void) {
	return sizeof(ImageClips);
}
