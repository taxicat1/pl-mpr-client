#ifndef MPR_MPR_CLIENT_MPR_COMM_H
#define MPR_MPR_CLIENT_MPR_COMM_H

#include <nitro/types.h>

#include "pokemon.h"
#include "savedata.h"
#include "charcode.h"

#define MPR_CONFIG_SHOW_SPECIES_SHAYMIN    (1 << 0)
#define MPR_CONFIG_SHOW_SPECIES_ARCEUS     (1 << 1)
#define MPR_CONFIG_SHOW_MOVE_JUDGMENT      (1 << 2)
#define MPR_CONFIG_SHOW_MOVE_SEED_FLARE    (1 << 3)
#define MPR_CONFIG_SHOW_ABILITY_MULTITYPE  (1 << 4)

void MPRComm_Init(SaveData* saveData);
void MPRComm_Reset(void);
void MPRComm_SetIdle(void);
void MPRComm_0203C1B4(void);
void MPRComm_0203C1C8(void);
void MPRComm_0203C1DC(void);
void MPRComm_0203C1F0(void);
void MPRComm_0203C204(void);
BOOL MPRComm_0203C218(void);
charcode_t* MPRComm_GetRanchName(void);
u8* MPRComm_GetBoxBuffer(int boxID);
BOOL MPRComm_ParentInDepositMode(void);
int MPRComm_GetRanchDepositSpace(void);
void MPRComm_DepositMon(u32 monIndex, u32 arg1);
BOOL MPRComm_0203C2B8(void);
void MPRComm_ShowPCBox(const BoxPokemon* boxMons, s32 pcBoxIndex);
BOOL MPRComm_IsAwaitingShowPCBox(void);
BOOL MPRComm_ParentInWithdrawMode(void);
s32 MPRComm_0203C33C(void);
u32 MPRComm_GetDisplayConfig(void);
void MPRComm_SetSortAndFilterParams(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6);
void MPRComm_0203C3A4(int arg0, int arg1, void* arg2);
BOOL MPRComm_0203C404(void);
void MPRComm_0203C420(int arg0, int arg1);
BOOL MPRComm_0203C444(void);
void MPRComm_SetFocusedMonIndex(int focusedMonIndex);
void MPRComm_UpdateMon(int monIndex, const BoxPokemon* mon);
BOOL MPRComm_IsMonUpdateComplete(void);
BOOL MPRComm_ParentInSavingMode(void);
BOOL MPRComm_ParentInSavingEndMode(void);
void MPRComm_SetSavingFlag(void);
BOOL MPRComm_0203C520(void);
BOOL MPRComm_IsConnected(void);
u8 MPRComm_GetErrorFlag(void);
void MPRComm_TickSystem(void);
void MPRComm_SendKeyInput(u16 input);
void MPRComm_ClientEnterSubMenu(void);
u8 MPRComm_ParentSyncSubMenu(void);
void MPRComm_ClientLeaveSubMenu(void);

#endif /* MPR_MPR_CLIENT_MPR_COMM_H */
