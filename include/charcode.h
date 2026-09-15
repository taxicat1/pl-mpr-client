#ifndef MPR_CHARCODE_H
#define MPR_CHARCODE_H

#include <nitro/types.h>

typedef u16 charcode_t;

const charcode_t* CharCode_SkipFormatArg(const charcode_t* cstr);
u32 CharCode_FormatArgType(const charcode_t* cstr);
BOOL CharCode_IsFormatArg(const charcode_t* cstr);
u32 CharCode_FormatArgParam(const charcode_t* cstr, u32 paramIdx);

#endif /* MPR_CHARCODE_H */
