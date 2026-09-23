#include <nitro.h>
#include <string.h>

#include "mpr_client/mpr_comm.h"

#include "mpr_client/mpr_client_common.h"
#include "mpr_client/mpr_ggid.h"

#include "assert.h"
#include "game_version.h"
#include "pokedex.h"
#include "pc_boxes.h"
#include "save_player.h"
#include "trainer_info.h"
#include "wireless_manager.h"  // WH_*
#include "constants/charcode.h"

typedef void (*DataCommCallback)(u16 port, void* dataP, u32 dataLen);

typedef struct {
	void*             dataP;
	u32               dataLen;
	u32               dataTransmitted;
	DataCommCallback  callback;
} CommPortData;

typedef struct {
	u8 boxMons[BOX_DATA_SIZE]  ATTRIBUTE_ALIGN(32);
} PCBoxBuffer;

// TODO this seems hacky to group these in a struct. However the second buffer will
//      not be linked after the PC box buffer without doing this
typedef struct {
	PCBoxBuffer  pcBoxes[3];
	u16          unk_3000[16] ATTRIBUTE_ALIGN(4);
} ClientConfigUnk;

// TODO this is also hacky to group these in a struct. However it does not even seem
//      possible to link smaller data after larger data without it?
typedef struct {
	u16  unk_00[70];  // ranch name buffer
	u16  unk_8C[16];  // ?
} UnkClientCodeBuffer;

typedef enum {
	COMM_STATE_INVALID = 0,
	COMM_STATE_IDLE,
	COMM_STATE_PARENT_SEARCH,
	COMM_STATE_PARENT_CONNECT,
	COMM_STATE_CONNECTED
} ParentCommState;

#define SEND_BUFFER_SIZE  (480)  //< Typo? Seems like this is meant to be 0x480 (the WH client buffer size)
#define TEXT_BUFFER_SIZE  (0x400)
#define MAIN_BUFFER_SIZE  (0x20000)

static void ClientSendDataCallback(u16 port, BOOL success);
static void SendDataToParent(u16 port, void* dataP, u32 dataLen, DataCommCallback callback);
static void ParentRecvDataCallback(u16 port, u16 aid, u16* data, u16 length);
static void RecvDataFromParent(u16 port, void* dataP, u32 dataLen, DataCommCallback callback);
static u32 ParentRecvGetDataTransmitted(u16 port);
static void ParentScanCallback(WMBssDesc* wmBssDesc);
static void ParentRecvPort14Callback(u16 port, u16 aid, u16* data, u16 length);
static void ClearRecvBufferDebugFlag(void);
static void ClientSendResponseCallback(u16 port, BOOL success);
static void ShowPCBoxCallback(u16 port, void* dataP, u32 dataLen);
static BOOL CheckBoxDownloadDone(void);
static void UpdateMonCallback(u16 port, void* dataP, u32 dataLen);
static void ParentRecvPort13Callback(u16 port, u16 aid, u16* data, u16 length);

// Very sensitive to order.
static const BoxPokemon* sUpdatedMon = NULL;
static int sFilterNature = -1;
static WMBssDesc sWMBssDesc  ATTRIBUTE_ALIGN(32);
static u32 sDAT_021376D0 = 0;
static u32 sClientDisplayConfig = 0xFFFFFFFF; // Bitmask of event unlocks
static u8 sCommErrorFlag = FALSE;
static int sPrevClientState = 35;
static int sFilterType = -1;
static const SaveData* sSaveData = NULL;
static int sFilterSpecies = -1;
static int sPeekRanchBoxID = -1;
static vu32 sSharedDataBufferLen = 0;
static int sClientRequestedState = 2;
static s32 sWithdrawRanchMonIndex = -1;
static int sFilterMark = -1;
static ParentCommState sCommState = COMM_STATE_IDLE;
static int sUpdatedMonIndex = -1;
static u8 sClientSubMenuFlag = FALSE;
static int sPeekRanchBoxPrevID = -1;
static int sFocusedMonIndex = -1;
static int sFilterAbility = -1;
static s32 sRanchDepositSpaceRemaining = -1;
static s32 sDepositMonIndex = -1;
static s32 sCurrPCBoxIndex = -1;
static s32 sDAT_0206AA38 = -1;
static ParentCommState sPrevCommState = COMM_STATE_PARENT_SEARCH;
static s32 sFilterMove = -1;
static s32 sFilterSortOrder = -1;
static u8 sRecvBufferDebugFlag = FALSE;
static u8 sParentRecvToSharedBufferFlag = FALSE;
static u8 sDAT_021376C4 = 0;
static u8 sClientSaveComplete = FALSE;
static u8 sParentSubMenuFlag = FALSE;
static u8 sRecvBufferNewFlag = FALSE;
static u32 sDAT_021376D4 = 0;
static void* sPeekRanchBoxOutPtr = NULL;
static CommPortData sParentRecvComm[8];
static CommPortData sClientSendComm[8];
static char sParentRecvBuffer[TEXT_BUFFER_SIZE]  ATTRIBUTE_ALIGN(32);
static ClientConfigUnk sClientUnk;
static u8 sSharedDataBuffer[MAIN_BUFFER_SIZE]  ATTRIBUTE_ALIGN(32);
static u32 sClientState = 0;
static u8 sParentSaveComplete = FALSE;

static char sClientSendBuffer[TEXT_BUFFER_SIZE]  ATTRIBUTE_ALIGN(32);
static u8 sClientSendSuccessfulFlag = FALSE;

static u8 sSubMenuDebugFlag = FALSE;

static UnkClientCodeBuffer sDAT_0206AA94  ATTRIBUTE_ALIGN(4) = {
	{ CHAR_EOS },
	{ 0x0000, 0x0101, 0x0202, 0x0303, 0x0404, 0x0505, 0x0606, 0x0707, 0x0808, 0x0909, 0x0A0A, 0x0B0B, 0x0C0C, 0x0D0D, 0x0E0E, 0x0F0F }
};

static void ClientSendDataCallback(u16 port, BOOL success) {
	#pragma unused(success)
	
	CommPortData* comm = &sClientSendComm[port - 8];
	
	u32 dataLen         = comm->dataLen;
	u32 dataTransmitted = comm->dataTransmitted;
	
	if (dataLen - dataTransmitted <= SEND_BUFFER_SIZE) {
		if (comm->callback != NULL) {
			comm->callback(port, comm->dataP, comm->dataLen);
		}
	} else {
		u8* dataP = (u8*)comm->dataP;
		
		dataTransmitted += SEND_BUFFER_SIZE;
		comm->dataTransmitted = dataTransmitted;
		dataP += dataTransmitted;
		
		u32 length = dataLen - dataTransmitted;
		if (length > SEND_BUFFER_SIZE) {
			length = SEND_BUFFER_SIZE;
		}
		
		WH_SendData(port, dataP, length, ClientSendDataCallback);
	}
}


static void SendDataToParent(u16 port, void* dataP, u32 dataLen, DataCommCallback callback) {
	CommPortData* comm = &sClientSendComm[port - 8];
	
	comm->dataP           = dataP;
	comm->dataLen         = dataLen;
	comm->dataTransmitted = 0;
	comm->callback        = callback;
	
	if (dataLen > SEND_BUFFER_SIZE) {
		dataLen = SEND_BUFFER_SIZE;
	}
	
	WH_SendData(port, dataP, dataLen, ClientSendDataCallback);
}


static void ParentRecvDataCallback(u16 port, u16 aid, u16* data, u16 length) {
	#pragma unused(aid)
	
	CommPortData* comm = &sParentRecvComm[port - 8];
	
	u32 dataLen         = comm->dataLen;
	u32 dataTransmitted = comm->dataTransmitted;
	u32 newTransmit     = length;
	
	u32 dataLenRemaining = dataLen - dataTransmitted;
	if (dataLenRemaining == 0) {
		return;
	}
	
	u8* dataP = (u8*)comm->dataP;
	
	if (newTransmit > dataLenRemaining) {
		newTransmit = dataLenRemaining;
	}
	
	MI_CpuCopy8(data, dataP + dataTransmitted, newTransmit);
	comm->dataTransmitted = dataTransmitted + newTransmit;
	if (comm->dataTransmitted == comm->dataLen) {
		if (comm->callback != NULL) {
			comm->callback(port, comm->dataP, comm->dataLen);
		}
	}
}


static void RecvDataFromParent(u16 port, void* dataP, u32 dataLen, DataCommCallback callback) {
	CommPortData* comm = &sParentRecvComm[port - 8];
	
	comm->dataP           = dataP;
	comm->dataLen         = dataLen;
	comm->dataTransmitted = 0;
	comm->callback        = callback;
	
	WH_SetReceiver(port, ParentRecvDataCallback);
}


static u32 ParentRecvGetDataTransmitted(u16 port) {
	CommPortData* comm = &sParentRecvComm[port - 8];
	return comm->dataTransmitted;
}


static void ParentScanCallback(WMBssDesc* wmBssDesc) {
	if (wmBssDesc->gameInfo.ggid != MPR_GGID_JA)  {
		return;
	}
	
	sWMBssDesc.length            = wmBssDesc->length;
	sWMBssDesc.rssi              = wmBssDesc->rssi;
	sWMBssDesc.bssid             = wmBssDesc->bssid;
	sWMBssDesc.ssidLength        = wmBssDesc->ssidLength;
	sWMBssDesc.ssid              = wmBssDesc->ssid;
	sWMBssDesc.capaInfo          = wmBssDesc->capaInfo;
	sWMBssDesc.rateSet           = wmBssDesc->rateSet;
	sWMBssDesc.beaconPeriod      = wmBssDesc->beaconPeriod;
	sWMBssDesc.dtimPeriod        = wmBssDesc->dtimPeriod;
	sWMBssDesc.channel           = wmBssDesc->channel;
	sWMBssDesc.cfpPeriod         = wmBssDesc->cfpPeriod;
	sWMBssDesc.cfpMaxDuration    = wmBssDesc->cfpMaxDuration;
	sWMBssDesc.gameInfoLength    = wmBssDesc->gameInfoLength;
	sWMBssDesc.otherElementCount = wmBssDesc->otherElementCount;
	sWMBssDesc.gameInfo          = wmBssDesc->gameInfo;
	
	sCommState = COMM_STATE_PARENT_CONNECT;
}


static void ParentRecvPort14Callback(u16 port, u16 aid, u16* data, u16 length) {
	#pragma unused(port)
	#pragma unused(aid)
	
	if (data == NULL) {
		return;
	}
	
	if (sParentRecvToSharedBufferFlag) { // Flag is never set
		if (sSharedDataBufferLen + length <= MAIN_BUFFER_SIZE) {
			MI_CpuCopy8(data, sSharedDataBuffer + sSharedDataBufferLen, length);
		}
		
		sSharedDataBufferLen += length;
	} else {
		// Unreachable?
		MI_CpuCopy8(data, sParentRecvBuffer, length);
		sParentRecvBuffer[length] = '\0';
		
		sRecvBufferNewFlag = TRUE;
		sRecvBufferDebugFlag = TRUE;
	}
}


static void ClearRecvBufferDebugFlag(void) {
	// Flag is unused
	if (sRecvBufferDebugFlag) {
		sRecvBufferDebugFlag = FALSE;
	}
}


static void ClientSendResponseCallback(u16 port, BOOL success) {
	#pragma unused(port)
	
	if (success) {
		sClientSendSuccessfulFlag = TRUE;
	}
}


void MPRComm_Init(SaveData* saveData) {
	sDAT_0206AA94.unk_00[0] = CHAR_EOS;
	sSaveData = saveData;
	
	WH_Initialize();
	WH_SetGgid(MPR_GGID_JA);
	
	sCommState = COMM_STATE_PARENT_SEARCH;
	sCommErrorFlag = FALSE;
}


void MPRComm_Reset(void) {
	if (sCommState == COMM_STATE_IDLE) {
		return;
	}
	
	sCommState = COMM_STATE_IDLE;
	WH_Finalize();
}


void MPRComm_SetIdle(void) {
	if (sCommState != COMM_STATE_IDLE) {
		sCommState = COMM_STATE_IDLE;
	}
}


void MPRComm_0203C1B4(void) {
	sClientRequestedState = 11;
}


void MPRComm_0203C1C8(void) {
	sClientRequestedState = 16;
}


void MPRComm_0203C1DC(void) {
	sClientRequestedState = 22;
}


void MPRComm_0203C1F0(void) { // Called from the save application to initiate a save
	sClientRequestedState = 28;
}


void MPRComm_0203C204(void) { // Called from the menu application when the player has no save file
	sClientRequestedState = 32;
}


BOOL MPRComm_0203C218(void) {
	return (sClientState == 0);
}


charcode_t* MPRComm_GetRanchName(void) {
	return (charcode_t*)sDAT_0206AA94.unk_00;
}


u8* MPRComm_GetBoxBuffer(int boxID) {
	return sClientUnk.pcBoxes[boxID].boxMons;
}


BOOL MPRComm_ParentInDepositMode(void) { // Called to verify if the wii is in deposit mode when the DS clicks the button
	return (sClientState == 18);
}


static void ShowPCBoxCallback(u16 port, void* dataP, u32 dataLen) {
	#pragma unused(port)
	#pragma unused(dataP)
	#pragma unused(dataLen)
	
	sCurrPCBoxIndex = -1;
}


int MPRComm_GetRanchDepositSpace(void) {
	return sRanchDepositSpaceRemaining;
}


void MPRComm_DepositMon(u32 monIndex, u32 arg1) {
	sDepositMonIndex = monIndex;
	sDAT_021376D4 = arg1;
	sRanchDepositSpaceRemaining--;
}


BOOL MPRComm_0203C2B8(void) {
	return (sDepositMonIndex < 0);
}


void MPRComm_ShowPCBox(const BoxPokemon* boxMons, s32 pcBoxIndex) {
	memcpy(sSharedDataBuffer, boxMons, BOX_DATA_SIZE);
	sCurrPCBoxIndex = pcBoxIndex;
}


BOOL MPRComm_IsAwaitingShowPCBox(void) {
	return (sCurrPCBoxIndex < 0);
}


BOOL MPRComm_ParentInWithdrawMode(void) { // Called to verify if the Wii is in withdraw mode when the DS clicks the button
	return (sClientState == 24);
}


s32 MPRComm_0203C33C(void) {
	return sDAT_0206AA38;
}


u32 MPRComm_GetDisplayConfig(void) {
	// Bit 1 : show Shaymin in species names
	// Bit 2 : show Arceus in species names
	// Bit 3 : show Judgment in move names
	// Bit 4 : show Seed Flare in move names
	// Bit 5 : show Multitype in ability names
	return sClientDisplayConfig;
}


void MPRComm_SetSortAndFilterParams(int sortOrder, int filterSpecies, int filterType, int filterMove, int filterAbility, int filterNature, int filterMark) {
	sParentRecvComm[12 - 8].dataTransmitted = 0;
	
	sFilterSortOrder = sortOrder;
	sFilterSpecies   = filterSpecies;
	sFilterType      = filterType;
	sFilterMove      = filterMove;
	sFilterAbility   = filterAbility;
	sFilterNature    = filterNature;
	sFilterMark      = filterMark;
}


void MPRComm_SetPeekRanchBox(int currBoxID, int prevBoxID, void* outBoxMons) {
	sParentRecvComm[12 - 8].dataTransmitted = 0;
	
	sPeekRanchBoxID = currBoxID;
	sPeekRanchBoxPrevID = prevBoxID;
	sPeekRanchBoxOutPtr = outBoxMons;
}


static BOOL CheckBoxDownloadDone(void) {
	if (ParentRecvGetDataTransmitted(12) < BOX_DATA_SIZE) {
		return FALSE;
	}
	
	sPeekRanchBoxID = -1;
	sPeekRanchBoxPrevID = -1;
	
	return TRUE;
}


BOOL MPRComm_IsNotPeekingRanchBox(void) {
	return (sPeekRanchBoxID < 0);
}


void MPRComm_0203C420(int arg0, int arg1) {
	sParentRecvComm[12 - 8].dataTransmitted = 0;
	
	sWithdrawRanchMonIndex = arg0;
	sDAT_021376D0 = arg1;
}


BOOL MPRComm_IsWithdrawComplete(void) {
	if (sWithdrawRanchMonIndex >= 0) {
		return FALSE;
	}
	
	sWithdrawRanchMonIndex = -1;
	return TRUE;
}


void MPRComm_SetFocusedMonIndex(int focusedMonIndex) {
	sFocusedMonIndex = focusedMonIndex;
}


static void UpdateMonCallback(u16 port, void* dataP, u32 dataLen) {
	#pragma unused(port)
	#pragma unused(dataP)
	#pragma unused(dataLen)
	
	sUpdatedMonIndex = -1;
	sUpdatedMon = NULL;
}


void MPRComm_UpdateMon(int monIndex, const BoxPokemon* mon) {
	sUpdatedMonIndex = monIndex;
	sUpdatedMon = mon;
}


BOOL MPRComm_IsMonUpdateComplete(void) {
	return (sUpdatedMon == NULL);
}


BOOL MPRComm_ParentInSavingMode(void) {
	// Switch required to match. Likely debug prints for other states
	switch (sClientState) {
		case 29:
			return TRUE;
		
		default:
			return FALSE;
	}
}


BOOL MPRComm_ParentInSavingEndMode(void) {
	// Switch required to match. Likely debug prints for other states
	switch (sClientState) {
		case 31:
			return TRUE;
		
		default:
			return FALSE;
	}
}


void MPRComm_SetSavingFlag(void) {
	sClientSaveComplete = TRUE;
}


BOOL MPRComm_0203C520(void) {
	// Switch required to match. Likely debug prints for other states
	switch (sClientState) {
		case 34:
			return TRUE;
		
		default:
			return FALSE;
	}
}


BOOL MPRComm_IsConnected(void) {
	return (sCommState == COMM_STATE_CONNECTED);
}


u8 MPRComm_GetErrorFlag(void) {
	return sCommErrorFlag;
}


void MPRComm_TickSystem(void) {
	int state = WH_GetSystemState();
	
	if (sCommState != sPrevCommState) {
		sPrevCommState = sCommState;
	}
	
	switch (state) {
		case WH_SYSSTATE_IDLE:
			if (sCommState == COMM_STATE_IDLE) {
				WH_End();
			} else if (sCommState == COMM_STATE_PARENT_SEARCH) {
				static const u8 sWildcardMacAddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
				
				const u8* macAddr = sWildcardMacAddr;
				
				const MBParentBssDesc* parent = MB_GetMultiBootParentBssDesc();
				if (parent != NULL) {
					macAddr = (const u8*)parent->bssid;
				}
				
				WH_StartScan(ParentScanCallback, macAddr, 0);
			
			} else if (sCommState == COMM_STATE_PARENT_CONNECT) {
				sDAT_0206AA94.unk_00[0] = CHAR_EOS;
				
				sRecvBufferNewFlag = FALSE;
				sClientSendSuccessfulFlag = FALSE;
				sParentRecvToSharedBufferFlag = FALSE;
				
				WH_SetReceiver(14, ParentRecvPort14Callback);
				WH_SetReceiver(13, ParentRecvPort13Callback);
				
				WH_ChildConnect(1, &sWMBssDesc);
			}
			break;
		
		case WH_SYSSTATE_SCANNING:
			if (sCommState == COMM_STATE_PARENT_CONNECT) {
				WH_EndScan();
			}
			break;
		
		case WH_SYSSTATE_BUSY:
			break;
		
		case WH_SYSSTATE_CONNECTED:
			if (sPrevClientState != sClientState) {
				sPrevClientState = sClientState;
			}
			
			if (sCommState == COMM_STATE_PARENT_CONNECT) {
				sCommState = COMM_STATE_CONNECTED;
				sClientState = 0;
			}
			
			int responseLen = 0;
			
			ClearRecvBufferDebugFlag();
			
			if (sRecvBufferNewFlag) {
				char ranchNameBuf[256 + 1];
				if (STD_TSScanf(sParentRecvBuffer, "GET MYSTATUS %256s", ranchNameBuf) == 1) {
					int len = strlen(ranchNameBuf) / 4;
					
					sRecvBufferNewFlag = FALSE;
					
					for (int i = 0; i < len; i++) {
						u16 charCode;
						STD_TSScanf(ranchNameBuf + (i * 4), "%04X", &charCode);
						sDAT_0206AA94.unk_00[i] = charCode;
					}
					sDAT_0206AA94.unk_00[len] = CHAR_EOS;
					
					u8 gameCode = 0;
					TrainerInfo* trainerInfo = SaveData_GetTrainerInfo((SaveData*)sSaveData);
					MI_CpuCopy8(trainerInfo, sSharedDataBuffer, TrainerInfo_GetSize());
					
					switch (gGameVersion) {
						case VERSION_PEARL:
							gameCode = 1;
							break;
						
						case VERSION_PLATINUM:
							gameCode = 2;
							break;
					}
					TrainerInfo_SetGameCode((TrainerInfo*)sSharedDataBuffer, gameCode);
					
					int trainerInfoSize = TrainerInfo_GetSize();
					Pokedex_CopyAsDP(SaveData_GetPokedex((SaveData*)sSaveData), (PokedexDP*)(sSharedDataBuffer + trainerInfoSize));
					SendDataToParent(12, sSharedDataBuffer, Pokedex_GetSaveSizeDP() + trainerInfoSize, NULL);
				
				} else if (STD_CompareString(sParentRecvBuffer, "GET MODE") == 0) {
					if (sClientState == 0) {
						sClientState = 2;
					}
					
					sRecvBufferNewFlag = FALSE;
					
					if (sClientRequestedState != 2) {
						sClientState = sClientRequestedState;
						sClientRequestedState = 2;
					} else {
						// BUG:? This seems like it should be writing to sClientSendBuffer
						STD_CopyString(sParentRecvBuffer, "MODE IDLE");
						responseLen = 9;
					}
				
				} else if (STD_CompareString(sParentRecvBuffer, "GET BOXDATA") == 0) {
					sRecvBufferNewFlag = FALSE;
					sDAT_021376C4 = 0;
				
				} else if (STD_CompareString(sParentRecvBuffer, "SET BOX START") == 0) {
					STD_CopyString(sClientSendBuffer, "OK");
					
					sRecvBufferNewFlag = FALSE;
					sSharedDataBufferLen = 0;
					responseLen = 2;
					
					RecvDataFromParent(12, sPeekRanchBoxOutPtr, BOX_DATA_SIZE, NULL);
					
					sClientState = 10;
				
				} else if (STD_CompareString(sParentRecvBuffer, "SYNCSAVE WII") == 0) {
					sRecvBufferNewFlag = FALSE;
					sParentSaveComplete = TRUE;
				}
			}
			
			switch (sClientState) {
				case 11:
					STD_CopyString(sClientSendBuffer, "MODE TOP");
					sClientState = 2;
					responseLen = 8;
					break;
				
				case 12:
					STD_CopyString(sClientSendBuffer, "MODE TRIAL");
					sClientState = 13;
					responseLen = 10;
					break;
				
				case 14:
					sClientState = 2;
					break;
				
				case 15:
					sClientState = 2;
					break;
				
				case 16:
					STD_CopyString(sClientSendBuffer, "MODE DEPOSIT");
					sClientState = 17;
					responseLen = 12;
					break;
				
				case 17:
					if (sRecvBufferNewFlag) {
						int params;
						if (STD_TSScanf(sParentRecvBuffer, "SET PARAMS %d", &params) == 1) {
							GF_ASSERT(params >= 0);
							sRanchDepositSpaceRemaining = params;
							
							STD_CopyString(sClientSendBuffer, "OK");
							sClientState = 18;
							sRecvBufferNewFlag = FALSE;
							responseLen = 2;
						}
					}
					break;
				
				case 18:
					if (sClientRequestedState != 2) {
						sDepositMonIndex = -1;
						sRanchDepositSpaceRemaining = -1;
						sCurrPCBoxIndex = -1;
						sFocusedMonIndex = -1;
						
						STD_CopyString(sClientSendBuffer, "MODE TOP");
						sClientState = 2;
						responseLen = 8;
					
					} else if (sDepositMonIndex >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "DEPOSIT %d %d", sDepositMonIndex, sDAT_021376D4);
						sClientState = 19;
					
					} else if (sCurrPCBoxIndex >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "SHOW BOX %d", sCurrPCBoxIndex);
						SendDataToParent(12, sSharedDataBuffer, BOX_DATA_SIZE, ShowPCBoxCallback);
						sClientState = 20;
					
					} else if (sFocusedMonIndex >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "FOCUS %d", sFocusedMonIndex);
						sFocusedMonIndex = -1;
						sClientState = 19;
					
					} else if (sUpdatedMon != NULL) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "UPDATE %d", sUpdatedMonIndex);
						MI_CpuCopy8(sUpdatedMon, sSharedDataBuffer, sizeof(BoxPokemon));
						SendDataToParent(12, sSharedDataBuffer, sizeof(BoxPokemon), UpdateMonCallback);
						sClientState = 21;
					}
					
					break;
				
				case 19:
					if (sRecvBufferNewFlag) {
						sDepositMonIndex = -1;
						sClientState = 18;
						sRecvBufferNewFlag = FALSE;
					}
					break;
				
				case 20:
					if (sCurrPCBoxIndex < 0) {
						sClientState = 18;
					}
					break;
				
				case 21:
					if (sUpdatedMon == NULL) {
						sClientState = 18;
					}
					break;
				
				case 22:
					sClientDisplayConfig = 0xFFFFFFFF;
					STD_CopyString(sClientSendBuffer, "MODE WITHDRAW");
					sClientState = 25;
					responseLen = 13;
					break;
				
				case 23:
					if (sRecvBufferNewFlag) {
						if (STD_CompareString(sParentRecvBuffer, "GET WITHDRAW") == 0) {
							sClientState = 24;
							sRecvBufferNewFlag = FALSE;
						}
					}
					break;
				
				case 24:
					if (sClientRequestedState != 2) {
						sFilterSortOrder = -1;
						sFilterSpecies   = -1;
						sFilterType      = -1;
						sFilterMove      = -1;
						sFilterAbility   = -1;
						sFilterNature    = -1;
						sFilterMark      = -1;
						
						sPeekRanchBoxID = -1;
						sPeekRanchBoxPrevID = -1;
						sDAT_0206AA38 = -1;
						sWithdrawRanchMonIndex = -1;
						sFocusedMonIndex = -1;
						
						STD_CopyString(sClientSendBuffer, "MODE TOP");
						sClientState = 2;
						responseLen = 8;
					
					} else if (sFilterSortOrder >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "PARAMS %d %d %d %d %d %d %d",
							sFilterSortOrder,
							sFilterSpecies,
							sFilterType,
							sFilterMove,
							sFilterAbility,
							sFilterNature,
							sFilterMark
						);
						
						sFilterSortOrder = -1;
						sFilterSpecies   = -1;
						sFilterType      = -1;
						sFilterMove      = -1;
						sFilterAbility   = -1;
						sFilterNature    = -1;
						sFilterMark      = -1;
						
						sClientState = 25;
					
					} else if (sPeekRanchBoxID >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "PEEK %d %d", sPeekRanchBoxID, sPeekRanchBoxPrevID);
						sClientState = 26;
					
					} else if (sWithdrawRanchMonIndex >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "WITHDRAW %d %d", sWithdrawRanchMonIndex, sDAT_021376D0);
						sWithdrawRanchMonIndex = -1;
						sClientState = 23;
					
					} else if (sFocusedMonIndex >= 0) {
						responseLen = STD_TSNPrintf(sClientSendBuffer, 0x201, "FOCUS %d", sFocusedMonIndex);
						sFocusedMonIndex = -1;
						sClientState = 23;
					}
					break;
				
				case 25:
					if (sRecvBufferNewFlag) {
						int param1, param2;
						if (STD_TSScanf(sParentRecvBuffer, "SET PARAMS %d %d", &param1, &param2) == 2) {
							GF_ASSERT(param1 >= 0 && param1 <= 2970);
							
							sDAT_0206AA38 = param1;
							if (sClientDisplayConfig == 0xFFFFFFFF) {
								sClientDisplayConfig = param2;
							}
							
							STD_CopyString(sClientSendBuffer, "OK");
							sClientState = 23;
							sRecvBufferNewFlag = FALSE;
							responseLen = 2;
						}
					}
					break;
				
				case 27:
					if (MPRComm_IsWithdrawComplete()) {
						sClientState = 23;
					}
					break;
				
				case 10:
					if (CheckBoxDownloadDone()) {
						STD_CopyString(sClientSendBuffer, "OK");
						sClientState = 23;
						responseLen = 2;
					}
					break;
				
				case 28:
					sParentSaveComplete = FALSE;
					sClientSaveComplete = FALSE;
					STD_CopyString(sClientSendBuffer, "MODE REPORT");
					sClientState = 29;
					responseLen = 11;
					break;
				
				case 29:
					if (sClientRequestedState == 32) {
						sClientState = 32;
					}
					
					if (sClientSaveComplete) {
						STD_CopyString(sClientSendBuffer, "SYNCSAVE DS");
						sClientState = 30;
						responseLen = 11;
					}
					
					break;
				
				case 30:
					if (sParentSaveComplete) {
						STD_CopyString(sClientSendBuffer, "SYNCSAVE FINALIZE");
						sClientState = 31;
						responseLen = 17;
					}
					break;
				
				case 31:
					if (sClientRequestedState != 2) {
						sClientState = 2;
					}
					break;
				
				case 32:
					STD_CopyString(sClientSendBuffer, "MODE EXIT");
					sClientState = 33;
					responseLen = 9;
					break;
				
				case 33:
					if (sRecvBufferNewFlag) {
						sClientState = 34;
						sRecvBufferNewFlag = FALSE;
					}
					break;
				
				case 0:
				case 34:
					break;
			}
			
			if (responseLen > 0) {
				responseLen = (responseLen + 2) & ~1;
				sRecvBufferNewFlag = FALSE;
				sClientSendSuccessfulFlag = FALSE;
				WH_SendData(14, sClientSendBuffer, responseLen, ClientSendResponseCallback);
			}
			
			break;
		
		case WH_SYSSTATE_ERROR:
			if (sCommState == COMM_STATE_PARENT_SEARCH) {
				break;
			}
			
			sCommErrorFlag = TRUE;
			
			if (sCommState != COMM_STATE_IDLE) {
				sCommState = COMM_STATE_PARENT_SEARCH;
				break;
			}
			
			WH_Finalize();
			break;
		
		case WH_SYSSTATE_CONNECT_FAIL:
			WH_Reset();
			sCommState = COMM_STATE_PARENT_SEARCH;
			break;
		
		default:
		case WH_SYSSTATE_STOP:
		case WH_SYSSTATE_DATASHARING:
		case WH_SYSSTATE_KEYSHARING:
		case WH_SYSSTATE_MEASURECHANNEL:
			WH_Reset();
			sCommState = COMM_STATE_PARENT_SEARCH;
			break;
	}
}


void MPRComm_SendKeyInput(u16 input) {
	if (sCommState == COMM_STATE_CONNECTED) {
		u16* dst = sClientUnk.unk_3000;
		*dst = input;
		WH_SendData(15, dst, sizeof(u16), NULL);
	}
}


static void ParentRecvPort13Callback(u16 port, u16 aid, u16* data, u16 length) {
	#pragma unused(port)
	#pragma unused(aid)
	#pragma unused(length)
	
	switch (*(u8*)data) {
		case 0:
			sSubMenuDebugFlag = TRUE;
			break;
		
		case 1:
			sParentSubMenuFlag = TRUE;
			break;
		
		case 2:
			sSubMenuDebugFlag = FALSE;
			
			if (sClientSubMenuFlag) {
				WH_SendData(13, &sDAT_0206AA94.unk_8C[0], sizeof(u16), NULL);
			}
			
			break;
	}
}


void MPRComm_ClientEnterSubMenu(void) {
	sClientSubMenuFlag = TRUE;
	
	if (!MPRComm_IsConnected()) {
		sParentSubMenuFlag = TRUE;
	} else {
		WH_SendData(13, &sDAT_0206AA94.unk_8C[0], sizeof(u16), NULL);
	}
}


u8 MPRComm_ParentSyncSubMenu(void) { // Must return true for the DS side to advance from the menu to the PC boxes or saving screen. if this takes too long, "Waiting for the Wii.." is displayed
	if (!MPRComm_IsConnected() && sClientSubMenuFlag) {
		sParentSubMenuFlag = TRUE;
	}
	
	return sParentSubMenuFlag;
}


void MPRComm_ClientLeaveSubMenu(void) { // Called when player returns to the menu after leaving the deposit/withdraw mode
	sParentSubMenuFlag = FALSE;
	sClientSubMenuFlag = FALSE;
	
	if (MPRComm_IsConnected()) {
		WH_SendData(13, &sDAT_0206AA94.unk_8C[1], sizeof(u16), NULL);
	}
}
