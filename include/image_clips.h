#ifndef MPR_IMAGE_CLIPS_H
#define MPR_IMAGE_CLIPS_H

#include "easy_chat_sentence.h"
#include "constants/string.h"

#define BACKDROP_COUNT         18
#define PHOTO_ACCESSORY_COUNT  10
#define SAVED_PHOTOS_COUNT     11
#define IC_UNKSUBSTRUCT_COUNT  5

typedef struct {
	u32  personality;
	u32  otID;
	u16  species;
	u16  nickname[MON_NAME_LEN + 1];
	u16  trainerName[TRAINER_NAME_LEN + 1];
	s8   priority;
	u8   xPos;
	u8   yPos;
	u8   form;
	u8   trainerGender;
} PhotoPokemon;

typedef struct {
	u8  unk_00;
	u8  xPos;
	u8  yPos;
	s8  priority;
} PhotoAccessory;

typedef struct {
	u32              integrity;
	PhotoPokemon     photoMon;
	u32              unk_3C;
	EasyChatSentence title;
	PhotoAccessory   accessories[PHOTO_ACCESSORY_COUNT];
	u8               unk_70;
	u8               language;
} DressUpPhoto;

typedef struct {
	u32             integrity;
	u32             unk_04;
	PhotoPokemon    photoMon;
	u32             unk_40;
	PhotoAccessory  accessories[20];
	u8              unk_94;
} UnkImageClipsSub;

typedef struct {
	u32  nonUniqueAccessoryFlags[8];
	u32  uniqueAccessoryFlags[2];
	u32  backdropFlags[6];
} FashionCase;

typedef struct {
	DressUpPhoto      savedPhotos[SAVED_PHOTOS_COUNT];
	UnkImageClipsSub  unk_4C8[IC_UNKSUBSTRUCT_COUNT];
	FashionCase       fashionCase;
} ImageClips;

void ImageClips_Init(ImageClips* imageClips);
int ImageClips_GetSaveSize(void);

#endif /* MPR_IMAGE_CLIPS_H */
