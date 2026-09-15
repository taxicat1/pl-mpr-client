#ifndef MPR_APPLICATIONS_NAMING_SCREEN_H
#define MPR_APPLICATIONS_NAMING_SCREEN_H

#include "game_options.h"
#include "overlay_manager.h"
#include "pc_boxes.h"
#include "string_gf.h"

typedef enum {
	NAMING_SCREEN_TYPE_PLAYER,
	NAMING_SCREEN_TYPE_POKEMON,
	NAMING_SCREEN_TYPE_BOX,
	NAMING_SCREEN_TYPE_RIVAL,
	NAMING_SCREEN_TYPE_FRIEND_CODE,
	NAMING_SCREEN_TYPE_GROUP,
	NAMING_SCREEN_TYPE_SHAYMIN_TABLET,
	NAMING_SCREEN_TYPE_PAL_PAD
} NamingScreenType;

typedef enum {
	NAMING_SCREEN_CODE_OK,
	NAMING_SCREEN_CODE_NO_INPUT
} NamingScreenReturnCode;

typedef struct {
	NamingScreenType        type;
	int                     playerGenderOrMonSpecies;
	int                     monForm;
	int                     maxChars;
	int                     monGender;
	NamingScreenReturnCode  returnCode;
	String*                 textInputStr;
	charcode_t              nameInputRaw[20];
	int                     battleMsgID;
	PCBoxes*                pcBoxes;
	Options*                options;
} NamingScreenArgs;

extern const ApplicationManagerTemplate gNamingScreenAppTemplate;

NamingScreenArgs* NamingScreenArgs_Init(
	HeapID heapID,
	NamingScreenType type,
	int playerGenderOrMonSpecies,
	int maxChars,
	Options* options);
void NamingScreenArgs_Free(NamingScreenArgs* args);

#endif /* MPR_APPLICATIONS_NAMING_SCREEN_H */
