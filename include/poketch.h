#ifndef MPR_POKETCH_H
#define MPR_POKETCH_H

#include "pokemon.h"
#include "savedata.h"

#define POKETCH_MAPMARKER_COUNT     6
#define POKETCH_POKEMONHISTORY_MAX  12
#define POKETCH_REGISTRY_SIZE       32
#define POKETCH_DOTART_SIZE_BYTES   120

typedef enum {
	POKETCH_APPID_DIGITALWATCH      =  0,
	POKETCH_APPID_CALCULATOR        =  1,
	POKETCH_APPID_MEMOPAD           =  2,
	POKETCH_APPID_PEDOMETER         =  3,
	POKETCH_APPID_PARTYSTATUS       =  4,
	POKETCH_APPID_FRIENDSHIPCHECKER =  5,
	POKETCH_APPID_DOWSINGMACHINE    =  6,
	POKETCH_APPID_BERRYSEARCHER     =  7,
	POKETCH_APPID_DAYCARECHECKER    =  8,
	POKETCH_APPID_POKEMONHISTORY    =  9,
	POKETCH_APPID_COUNTER           = 10,
	POKETCH_APPID_ANALOGWATCH       = 11,
	POKETCH_APPID_MARKINGMAP        = 12,
	POKETCH_APPID_LINKSEARCHER      = 13,
	POKETCH_APPID_COINTOSS          = 14,
	POKETCH_APPID_MOVETESTER        = 15,
	POKETCH_APPID_CALENDAR          = 16,
	POKETCH_APPID_DOTART            = 17,
	POKETCH_APPID_ROULETTE          = 18,
	POKETCH_APPID_TRAINERCOUNTER    = 19,
	POKETCH_APPID_KITCHENTIMER      = 20,
	POKETCH_APPID_COLORCHANGER      = 21,
	POKETCH_APPID_MATCHUPCHECKER    = 22,
	POKETCH_APPID_STOPWATCH         = 23,
	POKETCH_APPID_ALARMCLOCK        = 24,
	POKETCH_APPID_MAX               = 25,
	POKETCH_APPID_NONE              = -1,
} PoketchAppID;

typedef struct {
	u8  x;
	u8  y;
} PoketchMapPosition;

typedef struct {
	u16  species;
	u16  icon;
} PoketchPokemonHistoryEntryDP;

typedef struct {
    u8   poketchEnabled         : 1;
    u8   pedometerEnabled       : 1;
    u8   dotArtModifiedByPlayer : 1;
    u8   screenColor            : 3;
    u8                          : 2;
	
    s8   appCount;
    s8   appIndex;
    u8   appRegistry[POKETCH_REGISTRY_SIZE];
    u32  stepCount;
	
    u16  alarmSet    : 1;
    u16  alarmHour   : 5;
    u16  alarmMinute : 6;
    u16              : 4;
	
    u8   dotArtData[POKETCH_DOTART_SIZE_BYTES];
    u32  calendarMarkBitmap;
    u8   calendarMonth;
	
    PoketchMapPosition markMapPositions[POKETCH_MAPMARKER_COUNT];
	
    PoketchPokemonHistoryEntryDP pokemonHistoryQueue[POKETCH_POKEMONHISTORY_MAX];
} PoketchDP;

typedef struct {
	u16  species;
	u16  icon;
	u32  form;    //<! New with Pt
} PoketchPokemonHistoryEntryPt;

typedef struct {
    u8   poketchEnabled         : 1;
    u8   pedometerEnabled       : 1;
    u8   dotArtModifiedByPlayer : 1;
    u8   screenColor            : 3;
    u8                          : 2;
	
    s8   appCount;
    s8   appIndex;
    u8   appRegistry[POKETCH_REGISTRY_SIZE];
    u32  stepCount;
	
    u16  alarmSet    : 1;
    u16  alarmHour   : 5;
    u16  alarmMinute : 6;
    u16              : 4;
	
    u8   dotArtData[POKETCH_DOTART_SIZE_BYTES];
    u32  calendarMarkBitmap;
    u8   calendarMonth;
	
    PoketchMapPosition markMapPositions[POKETCH_MAPMARKER_COUNT];
	
    PoketchPokemonHistoryEntryPt pokemonHistoryQueue[POKETCH_POKEMONHISTORY_MAX];
} PoketchPt;

typedef struct {
    u8   poketchEnabled         : 1;
    u8   pedometerEnabled       : 1;
    u8   dotArtModifiedByPlayer : 1;
    u8   screenColor            : 3;
    u8                          : 2;
	
    s8   appCount;
    s8   appIndex;
    u8   appRegistry[POKETCH_REGISTRY_SIZE];
    u32  stepCount;
	
    u16  alarmSet    : 1;
    u16  alarmHour   : 5;
    u16  alarmMinute : 6;
    u16              : 4;
	
    u8   dotArtData[POKETCH_DOTART_SIZE_BYTES];
    u32  calendarMarkBitmap;
    u8   calendarMonth;
	
    PoketchMapPosition markMapPositions[POKETCH_MAPMARKER_COUNT];
} PoketchCommon;

int Poketch_GetSaveSize(void);
void Poketch_Init(PoketchCommon* poketch);
BOOL Poketch_RegisterApp(PoketchCommon* poketch, PoketchAppID appID);

#endif /* MPR_POKETCH_H */
