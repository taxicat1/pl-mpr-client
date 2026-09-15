#include <nitro.h>
#include <string.h>

#include "seal_case.h"


int SealCase_GetSaveSize(void) {
	return sizeof(SealCase);
}


void SealCase_Init(SealCase* sealCase) {
	MI_CpuFill8(sealCase, 0, SealCase_GetSaveSize());
}


void BallCapsule_Copy(const BallCapsule* src, BallCapsule* dst) {
	MI_CpuCopy8(src, dst, sizeof(BallCapsule));
}
