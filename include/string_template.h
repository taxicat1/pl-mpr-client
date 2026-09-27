#ifndef MPR_STRING_TEMPLATE_H
#define MPR_STRING_TEMPLATE_H

#include "constants/abilities.h"
#include "constants/pokemon_data_params.h"
#include "heap.h"
#include "struct_defs/pokemon.h"
#include "pc_boxes.h"
#include "string_gf.h"

#define STRING_TEMPLATE_DEFAULT_MAX_ARGS    8
#define STRING_TEMPLATE_DEFAULT_MAX_LENGTH  32

typedef struct {
	u8  unused_00;
	u8  unused_01;
	u8  unused_02;
	u8  unused_03;
} StringTemplateArgHeader;

typedef struct {
	StringTemplateArgHeader  header;
	String*                  string;
} StringTemplateArg;

typedef struct {
	u32                 maxArgs;
	HeapID              heapID;
	StringTemplateArg*  args;
	String*             templateBuf;
} StringTemplate;

StringTemplate* StringTemplate_Default(HeapID heapID);
StringTemplate* StringTemplate_New(u32 maxArgs, u32 maxLen, HeapID heapID);
void StringTemplate_Free(StringTemplate* stringTemplate);
void StringTemplate_SetString(StringTemplate* stringTemplate, u32 idx, const String* argVal, u32 unused_gender, BOOL unused, u32 language);
void StringTemplate_SetSpeciesName(StringTemplate* stringTemplate, u32 idx, BoxPokemon* boxMon);
void StringTemplate_SetNickname(StringTemplate* stringTemplate, u32 idx, BoxPokemon* boxMon);
void StringTemplate_SetOTName(StringTemplate* stringTemplate, u32 idx, BoxPokemon* boxMon);
void StringTemplate_SetNumber(StringTemplate* stringTemplate, u32 idx, int num, u32 maxDigits, PaddingMode paddingMode, CharsetMode charsetMode);
void StringTemplate_SetAbilityName(StringTemplate* stringTemplate, u32 idx, Ability ability);
void StringTemplate_SetItemName(StringTemplate* stringTemplate, u32 idx, u32 item);
void StringTemplate_SetPCBoxName(StringTemplate* stringTemplate, u32 idx, const PCBoxes* pcBoxes, u32 boxIdx);
void StringTemplate_SetMetLocationName(StringTemplate* stringTemplate, u32 idx, u32 location);
void StringTemplate_SetMonthName(StringTemplate* stringTemplate, u32 idx, u32 month);
void StringTemplate_Format(const StringTemplate* stringTemplate, String* dst, const String* fmtString);

#endif /* MPR_STRING_TEMPLATE_H */
