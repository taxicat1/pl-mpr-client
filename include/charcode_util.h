#ifndef MPR_CHARCODE_UTIL_H
#define MPR_CHARCODE_UTIL_H

#include "charcode.h"
#include "string_gf.h"

charcode_t* CharCode_Copy(charcode_t* dst, const charcode_t* src);
u32 CharCode_Length(const charcode_t* str);
BOOL CharCode_Compare(const charcode_t* str1, const charcode_t* str2);
charcode_t* CharCode_FillWith(charcode_t* str, charcode_t fill, u32 num);
charcode_t* CharCode_FillWithEOS(charcode_t* str, u32 num);
charcode_t* CharCode_FromInt(charcode_t* str, s32 i, PaddingMode paddingMode, u32 digits);

#endif /* MPR_CHARCODE_UTIL_H */
