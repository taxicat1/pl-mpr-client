#ifndef MPR_SEAL_CASE_H
#define MPR_SEAL_CASE_H

#include <nitro/types.h>

#include "struct_defs/seal_case.h"

int SealCase_GetSaveSize(void);
void SealCase_Init(SealCase* sealCase);
void BallCapsule_Copy(const BallCapsule* src, BallCapsule* dst);

#endif /* MPR_SEAL_CASE_H */
