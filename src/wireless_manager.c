#include <nitro.h>
#include <nitro/cht.h>

#include "wireless_manager.h"

#include "common.h"

#define WH_DMA_NO           2
#define WH_DS_PORT          13
#define WH_DATA_PRIO        WM_PRIORITY_NORMAL
#define WH_MP_FREQUENCY     1
#define WH_BITMAP_EMPTY     1

typedef void (*WHTraceFunc)(const char*, ...);
typedef BOOL (*WHJudgeAcceptFunc)(WMStartParentCallback*);
typedef u16 (*WHParentWEPKeyGeneratorFunc)(u16*, const WMParentParam*);
typedef u16 (*WHChildWEPKeyGeneratorFunc)(u16*, const WMBssDesc*);

static const char* WH_GetWMErrCodeName(int errorCode);
static const char* WH_GetWMStateCodeName(u16 stateId);
static void WH_ChangeSysState(int state);
static void WH_SetError(int errorCode);
static BOOL WH_StateInEndParentKeyShare(void);
static BOOL WH_StateInEndParentMP(void);
static void WH_StateOutEndParentMP(void* wmCallback);
static BOOL WH_StateInEndParent(void);
static void WH_StateOutEndParent(void* wmCallback);
static BOOL WH_StateInStartScan(void);
static void WH_StateOutStartScan(void* callback);
static BOOL WH_StateInEndScan(void);
static void WH_StateOutEndScan(void* callback);
static BOOL WH_StateInSetChildWEPKey(void);
static void WH_StateOutSetChildWEPKey(void* callback);
static BOOL WH_StateInStartChild(void);
static void WH_StateOutStartChild(void* callback);
static BOOL WH_StateInStartChildMP(void);
static void WH_StateOutStartChildMP(void* callback);
static BOOL WH_StateInStartChildKeyShare(void);
static BOOL WH_StateInEndChildKeyShare(void);
static BOOL WH_StateInEndChildMP(void);
static void WH_StateOutEndChildMP(void* callback);
static BOOL WH_StateInEndChild(void);
static void WH_StateOutEndChild(void* callback);
static BOOL WH_StateInReset(void);
static void WH_StateOutReset(void* callback);
static BOOL WH_StateInSetMPData(u16 port, const void* data, u16 datasize, WHSendCallbackFunc callback);
static void WH_StateOutSetMPData(void* callback);
static void WH_PortReceiveCallback(void* callback);
static void WH_StateOutEnd(void* callback);
static void WH_IndicateHandler(void* callback);
static BOOL WH_StateInInitialize(void);
static void WH_StateOutInitialize(void* callback);

static WMParentParam                sParentParam  ATTRIBUTE_ALIGN(32);
static int                          sSysState = WH_SYSSTATE_STOP;
static int                          sConnectMode;
static WHReceiverFunc               sReceiverFuncs[16];
static WHJudgeAcceptFunc            sJudgeAcceptFunc;
static u16                          sMyAid;
static u16                          sConnectBitmap;
static int                          sErrCode;
static u32                          sRand;              /* Unused */
static u16                          sChannel;           /* Unused */
static u16                          sChannelBusyRatio;  /* Unused */
static u16                          sChannelBitmap;     /* Unused */
static u8                           sWmBuffer[WM_SYSTEM_BUF_SIZE]           ATTRIBUTE_ALIGN(32);
static u8                           sSendBuffer[WH_CHILD_SEND_BUFFER_SIZE]  ATTRIBUTE_ALIGN(32);
static u8                           sRecvBuffer[WH_CHILD_RECV_BUFFER_SIZE]  ATTRIBUTE_ALIGN(32);
static s32                          sSendBufferSize;
static s32                          sRecvBufferSize;
static WMBssDesc                    sBssDesc    ATTRIBUTE_ALIGN(32);
static WMScanParam                  sScanParam  ATTRIBUTE_ALIGN(32);
static WHStartScanCallbackFunc      sScanCallback;
static u16                          sChannelIndex;
static u16                          sAutoConnectFlag;
static BOOL                         sPictoCatchFlag = FALSE;
static u8                           sConnectionSsid[WM_SIZE_CHILD_SSID];
static WHParentWEPKeyGeneratorFunc  sParentWEPKeyGenerator = NULL;  /* Unused */
static WHChildWEPKeyGeneratorFunc   sChildWEPKeyGenerator = NULL;
static u16                          sWEPKey[20 / sizeof(u16)]  ATTRIBUTE_ALIGN(32);
static WMDataSharingInfo            sDSInfo       ATTRIBUTE_ALIGN(32);
static WMDataSet                    sDataSet      ATTRIBUTE_ALIGN(32);  /* Unused */
static WMKeySetBuf                  sWMKeySetBuf  ATTRIBUTE_ALIGN(32);
static WHTraceFunc                  sWHTraceFunc = NULL; // Intended to be set to a printf() function type for debugging

#define WH_ASSERT(cond)  ((cond) || (OS_Terminate(), 0))  // ", 0" required to match in places

#define WH_TRACE(...) do {          \
    if (sWHTraceFunc) {             \
        sWHTraceFunc(__VA_ARGS__);  \
    }                               \
} while (FALSE)


static const char* WH_GetWMErrCodeName(int errorCode) {
	static const char* errorStrings[] = {
		"WM_ERRCODE_SUCCESS",
		"WM_ERRCODE_FAILED",
		"WM_ERRCODE_OPERATING",
		"WM_ERRCODE_ILLEGAL_STATE",
		"WM_ERRCODE_WM_DISABLE",
		"WM_ERRCODE_NO_DATASET",
		"WM_ERRCODE_INVALID_PARAM",
		"WM_ERRCODE_NO_CHILD",
		"WM_ERRCODE_FIFO_ERROR",
		"WM_ERRCODE_TIMEOUT",
		"WM_ERRCODE_SEND_QUEUE_FULL",
		"WM_ERRCODE_NO_ENTRY",
		"WM_ERRCODE_OVER_MAX_ENTRY",
		"WM_ERRCODE_INVALID_POLLBITMAP",
		"WM_ERRCODE_NO_DATA",
		"WM_ERRCODE_SEND_FAILED",
		"WM_ERRCODE_DCF_TEST",
		"WM_ERRCODE_WL_INVALID_PARAM",
		"WM_ERRCODE_WL_LENGTH_ERR",
		"WM_ERRCODE_FLASH_ERROR",
		"WH_ERRCODE_DISCONNECTED",
		"WH_ERRCODE_PARENT_NOT_FOUND",
		"WH_ERRCODE_NO_RADIO"
	};
	
	if (errorCode >= 0 && errorCode < NELEMS(errorStrings)) {
		return errorStrings[errorCode];
	} else {
		return "N/A";
	}
}


static const char* WH_GetWMStateCodeName(u16 stateId) {
	static const char* stateStrings[] = {
		"WM_STATECODE_PARENT_START",
		"N/A",
		"WM_STATECODE_BEACON_SENT",
		"WM_STATECODE_SCAN_START",
		"WM_STATECODE_PARENT_NOT_FOUND",
		"WM_STATECODE_PARENT_FOUND",
		"WM_STATECODE_CONNECT_START",
		"WM_STATECODE_CONNECTED",
		"WM_STATECODE_BEACON_LOST",
		"WM_STATECODE_DISCONNECTED",
		"WM_STATECODE_MP_START",
		"WM_STATECODE_MPEND_IND",
		"WM_STATECODE_MP_IND",
		"WM_STATECODE_MPACK_IND",
		"WM_STATECODE_DCF_START",
		"WM_STATECODE_DCF_IND",
		"WM_STATECODE_BEACON_RECV",
		"WM_STATECODE_DISASSOCIATE",
		"WM_STATECODE_REASSOCIATE",
		"WM_STATECODE_AUTHENTICATE",
		"WM_STATECODE_PORT_SEND",
		"WM_STATECODE_PORT_RECV",
		"WM_STATECODE_FIFO_ERROR",
		"WM_STATECODE_INFORMATION",
		"WM_STATECODE_UNKNOWN",
		"WM_STATECODE_PORT_INIT",
		"WM_STATECODE_DISCONNECTED_FROM_MYSELF"
	};
	
	if (stateId >= 0 && stateId < NELEMS(stateStrings)) {
		return stateStrings[stateId];
	} else {
		return "N/A";
	}
}


static void WH_ChangeSysState(int state) {
	static const char* stateNames[] = {
		"WH_SYSSTATE_STOP",
		"WH_SYSSTATE_IDLE",
		"WH_SYSSTATE_SCANNING",
		"WH_SYSSTATE_BUSY",
		"WH_SYSSTATE_CONNECTED",
		"WH_SYSSTATE_DATASHARING",
		"WH_SYSSTATE_KEYSHARING",
		"WH_SYSSTATE_MEASURECHANNEL",
		"WH_SYSSTATE_CONNECT_FAIL",
		"WH_SYSSTATE_ERROR"
	};
	
	WH_TRACE("%s -> ", stateNames[sSysState]);
	sSysState = state;
	WH_TRACE("%s\n", stateNames[sSysState]);
}


// Deadstripped function that used this string, which was reused later (-str reuse) affecting the order
void Dummy_StringOrder_0206A688(void);
void Dummy_StringOrder_0206A688(void) {
	WH_TRACE("unknown indicate, state = %d\n", 0);
}


static void WH_SetError(int errorCode) {
	if (sSysState == WH_SYSSTATE_ERROR || sSysState == WH_SYSSTATE_FATAL) {
		return;
	}
	
	sErrCode = errorCode;
}


static BOOL WH_StateInEndParentKeyShare(void) {
	WMErrCode errorCode = WM_EndKeySharing(&sWMKeySetBuf);
	if (errorCode != WM_ERRCODE_SUCCESS) {
		WH_SetError(errorCode);
		return FALSE;
	}
	
	if (!WH_StateInEndParentMP()) {
		WH_TRACE("WH_StateInEndParentMP failed\n");
		WH_Reset();
		return FALSE;
	}
	
	return TRUE;
}


static BOOL WH_StateInEndParentMP(void) {
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	WMErrCode errorCode = WM_EndMP(WH_StateOutEndParentMP);
	if (errorCode != WM_ERRCODE_OPERATING) {
		WH_SetError(errorCode);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutEndParentMP(void* wmCallback) {
	WMCallback* callback = (WMCallback*)wmCallback;
	
	if (callback->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(callback->errcode);
		WH_Reset();
		return;
	}
	
	if (!WH_StateInEndParent()) {
		WH_TRACE("WH_StateInEndParent failed\n");
		WH_Reset();
		return;
	}
}


// Deadstripped function that used this string, which was reused later (-str reuse) affecting the order
void Dummy_StringOrder_0206A6E4(void);
void Dummy_StringOrder_0206A6E4(void) {
	WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
	WH_TRACE("send buffer size = %d\n", sSendBufferSize);
}


static BOOL WH_StateInEndParent(void) {
	WMErrCode errorCode = WM_EndParent(WH_StateOutEndParent);
	
	if (errorCode != WM_ERRCODE_OPERATING) {
		WH_SetError(errorCode);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutEndParent(void* wmCallback) {
	WMCallback* callback = (WMCallback*)wmCallback;
	
	if (callback->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(callback->errcode);
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_IDLE);
}


BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8* macAddr, u16 channel) {
	WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);
	
	WH_ChangeSysState(WH_SYSSTATE_SCANNING);
	
	sScanCallback = callback;
	sChannelIndex = channel;
	sScanParam.channel = 0;
	sAutoConnectFlag = FALSE;
	
	*(u16*)(&sScanParam.bssid[4]) = *(u16*)(macAddr + 4);
	*(u16*)(&sScanParam.bssid[2]) = *(u16*)(macAddr + 2);
	*(u16*)(&sScanParam.bssid[0]) = *(u16*)(macAddr);
	
	if (!WH_StateInStartScan()) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return FALSE;
	}
	
	return TRUE;
}


BOOL WH_StateInStartScan(void) {
	WH_ASSERT(sSysState == WH_SYSSTATE_SCANNING);
	
	u16 channelBit = WM_GetAllowedChannel();
	
	if (channelBit == 0x8000) {
		WH_SetError(WM_ERRCODE_ILLEGAL_STATE);
		return FALSE;
	}
	
	if (channelBit == 0x0000) {
		WH_SetError(WH_ERRCODE_NO_RADIO);
		return FALSE;
	}
	
	if (sChannelIndex == 0) {
		while (TRUE) {
			sScanParam.channel++;
			if (sScanParam.channel > 16) {
				sScanParam.channel = 1;
			}
			
			if (channelBit & (0x0001 << (sScanParam.channel - 1))) {
				break;
			}
		}
	} else {
		sScanParam.channel = sChannelIndex;
	}
	
	sScanParam.maxChannelTime = WM_GetDispersionScanPeriod();
	sScanParam.scanBuf = &sBssDesc;
	
	WMErrCode result = WM_StartScan(WH_StateOutStartScan, &sScanParam);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutStartScan(void* callback) {
	WMstartScanCallback* cb = (WMstartScanCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return;
	}
	
	if (sSysState != WH_SYSSTATE_SCANNING) {
		sAutoConnectFlag = FALSE;
		if (!WH_StateInEndScan()) {
			WH_ChangeSysState(WH_SYSSTATE_ERROR);
		}
		return;
	}
	
	switch (cb->state) {
		case WM_STATECODE_SCAN_START:
			return;
		
		case WM_STATECODE_PARENT_NOT_FOUND:
			break;
		
		case WM_STATECODE_PARENT_FOUND:
			DC_InvalidateRange(&sBssDesc, sizeof(WMbssDesc));
			
			if (sPictoCatchFlag && CHT_IsPictochatParent(&sBssDesc)) {
				WH_TRACE("pictochat parent find\n");
				if (sScanCallback) {
					sScanCallback(&sBssDesc);
				}
				break;
			}
			
			if (!WM_IsValidGameInfo(&cb->gameInfo, cb->gameInfoLength) || cb->gameInfo.ggid != sParentParam.ggid) {
				// Mismatching parent GGID
				break;
			}
			
			if ((cb->gameInfo.gameNameCount_attribute & (WM_ATTR_FLAG_ENTRY | WM_ATTR_FLAG_MB)) != WM_ATTR_FLAG_ENTRY) {
				WH_TRACE("not recieve entry\n");
				break;
			}
			
			WH_TRACE("parent find\n");
			
			if (sScanCallback) {
				sScanCallback(&sBssDesc);
			}
			
			if (sAutoConnectFlag) {
				if (!WH_StateInEndScan()) {
					WH_ChangeSysState(WH_SYSSTATE_ERROR);
				}
				return;
			}
			
			break;
	}
	
	if (!WH_StateInStartScan()) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
	}
}


BOOL WH_EndScan(void) {
	if (sSysState != WH_SYSSTATE_SCANNING) {
		return FALSE;
	}
	
	sAutoConnectFlag = FALSE;
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	return TRUE;
}


static BOOL WH_StateInEndScan(void) {
	WMErrCode result = WM_EndScan(WH_StateOutEndScan);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutEndScan(void* callback) {
	WMCallback* cb = (WMCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_IDLE);
	
	if (!sAutoConnectFlag) {
		return;
	}
	
	if (sChildWEPKeyGenerator) {
		if (!WH_StateInSetChildWEPKey()) {
			WH_ChangeSysState(WH_SYSSTATE_ERROR);
		}
	} else if (!WH_StateInStartChild()) {
		WH_TRACE("WH_StateOutEndScan : startchild failed\n");
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
	}
}


static BOOL WH_StateInSetChildWEPKey(void) {
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	u16 wepmode = sChildWEPKeyGenerator(sWEPKey, &sBssDesc);
	WMErrCode result = WM_SetWEPKey(WH_StateOutSetChildWEPKey, wepmode, sWEPKey);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutSetChildWEPKey(void* callback) {
	WMCallback* cb = (WMCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return;
	}
	
	if (!WH_StateInStartChild()) {
		WH_TRACE("WH_StateOutSetChildWEPKey : startchild failed\n");
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
	}
}


static BOOL WH_StateInStartChild(void) {
	if (sSysState == WH_SYSSTATE_CONNECTED || sSysState == WH_SYSSTATE_KEYSHARING || sSysState == WH_SYSSTATE_DATASHARING) {
		WH_TRACE("WH_StateInStartChild : already connected?\n");
		return TRUE;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	WMErrCode result = WM_StartConnectEx(
		WH_StateOutStartChild,
		&sBssDesc,
		sConnectionSsid,
		TRUE,
		(sChildWEPKeyGenerator ? WM_AUTHMODE_SHARED_KEY : WM_AUTHMODE_OPEN_SYSTEM)
	);
	
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutStartChild(void* callback) {
	WMStartConnectCallback* cb = (WMStartConnectCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);

		if (cb->errcode == WM_ERRCODE_OVER_MAX_ENTRY) {
			WH_ChangeSysState(WH_SYSSTATE_ERROR);
			return;
		} else if (cb->errcode == WM_ERRCODE_NO_ENTRY) {
			WH_ChangeSysState(WH_SYSSTATE_ERROR);
			return;
		} else if (cb->errcode == WM_ERRCODE_FAILED) {
			WH_ChangeSysState(WH_SYSSTATE_CONNECT_FAIL);
			return;
		} else {
			WH_ChangeSysState(WH_SYSSTATE_ERROR);
			return;
		}
	}
	
	if (cb->state == WM_STATECODE_BEACON_LOST) {
		return;
	}
	
	if (cb->state == WM_STATECODE_CONNECTED) {
		WH_TRACE("Connect to Parent\n");
		WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
		if (!WH_StateInStartChildMP()) {
			WH_TRACE("WH_StateInStartChildMP failed\n");
			WH_ChangeSysState(WH_SYSSTATE_BUSY);
			return;
		}
		
		sMyAid = cb->aid;
		return;
	} else if (cb->state == WM_STATECODE_CONNECT_START) {
		return;
	} else if (cb->state == WM_STATECODE_DISCONNECTED) {
		WH_TRACE("Disconnected from Parent\n");
		WH_SetError(WH_ERRCODE_DISCONNECTED);
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return;
	} else if (cb->state == WM_STATECODE_DISCONNECTED_FROM_MYSELF) {
		return;
	}
	
	WH_TRACE("unknown state %d, %s\n", cb->state, WH_GetWMStateCodeName(cb->state));
	WH_ChangeSysState(WH_SYSSTATE_ERROR);
}


static BOOL WH_StateInStartChildMP(void) {
	WMErrCode result = WM_StartMP(
		WH_StateOutStartChildMP,
		(u16*)sRecvBuffer,
		(u16)sRecvBufferSize,
		(u16*)sSendBuffer,
		(u16)sSendBufferSize,
		WH_MP_FREQUENCY
	);
	
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutStartChildMP(void* callback) {
	WMStartMPCallback* cb = (WMStartMPCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		if (cb->errcode == WM_ERRCODE_SEND_FAILED) {
			return;
		} else if (cb->errcode == WM_ERRCODE_TIMEOUT) {
			return;
		} else if (cb->errcode == WM_ERRCODE_INVALID_POLLBITMAP) {
			return;
		}
		
		WH_SetError(cb->errcode);
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return;
	}
	
	switch (cb->state) {
		case WM_STATECODE_MP_START:
			if (sConnectMode == WH_CONNECTMODE_KS_CHILD) {
				if (sSysState == WH_SYSSTATE_KEYSHARING) {
					return;
				}
				
				if (sSysState == WH_SYSSTATE_CONNECTED) {
					if (!WH_StateInStartChildKeyShare()) {
						WH_TRACE("WH_StateInStartChildKeyShare failed\n");
						WH_Finalize();
					}
					return;
				}
			
			} else if (sConnectMode == WH_CONNECTMODE_DS_CHILD) {
				WMErrCode result;
				u16 aidBitmap = (1 << (WH_CHILD_MAX + 1)) - 1;
				result = WM_StartDataSharing(&sDSInfo, WH_DS_PORT, aidBitmap, WH_DS_DATA_SIZE, TRUE);
				if (result != WM_ERRCODE_SUCCESS) {
					WH_SetError(result);
					WH_Finalize();
					return;
				}
				
				WH_TRACE("WH_StateOutStartChildMP : WM_StartDataSharing OK\n");
				WH_ChangeSysState(WH_SYSSTATE_DATASHARING);
				return;
			}
			
			WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
			break;
		
		case WM_STATECODE_MP_IND:
			break;
		
		case WM_STATECODE_MPACK_IND:
			break;
		
		case WM_STATECODE_MPEND_IND:
		default:
			WH_TRACE("unknown indicate, state = %d\n", cb->state);
			break;
	}
}


static BOOL WH_StateInStartChildKeyShare(void) {
	if (sSysState == WH_SYSSTATE_KEYSHARING) {
		return TRUE;
	}
	
	if (sSysState != WH_SYSSTATE_CONNECTED) {
		return FALSE;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_KEYSHARING);
	WMErrCode result = WM_StartKeySharing(&sWMKeySetBuf, WH_DS_PORT);
	
	if (result != WM_ERRCODE_SUCCESS) {
		WH_SetError(result);
		return FALSE;
	}
	
	return TRUE;
}


static BOOL WH_StateInEndChildKeyShare(void) {
	if (sSysState != WH_SYSSTATE_KEYSHARING) {
		return FALSE;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	WMErrCode result = WM_EndKeySharing(&sWMKeySetBuf);
	
	if (result != WM_ERRCODE_SUCCESS) {
		WH_SetError(result);
		return FALSE;
	}
	
	if (!WH_StateInEndChildMP()) {
		return FALSE;
	}
	
	return TRUE;
}


static BOOL WH_StateInEndChildMP(void) {
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	WMErrCode result = WM_EndMP(WH_StateOutEndChildMP);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		return FALSE;
	}
	return TRUE;
}


static void WH_StateOutEndChildMP(void* callback) {
	WMCallback* cb = (WMCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
		WH_Finalize();
		return;
	}
	
	if (!WH_StateInEndChild()) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
	}
}


static BOOL WH_StateInEndChild(void) {
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	WMErrCode result = WM_Disconnect(WH_StateOutEndChild, 0);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		WH_Reset();
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutEndChild(void* callback) {
	WMCallback* cb = (WMCallback*)callback;
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_IDLE);
}


static BOOL WH_StateInReset(void) {
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	WMErrCode result = WM_Reset(WH_StateOutReset);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutReset(void* callback) {
	WMCallback *cb = (WMCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		WH_SetError(cb->errcode);
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_IDLE);
}


static BOOL WH_StateInSetMPData(u16 port, const void* data, u16 datasize, WHSendCallbackFunc callback) {
	DC_FlushRange(sSendBuffer, (u32)sSendBufferSize);
	WMErrCode result = WM_SetMPDataToPortEx(WH_StateOutSetMPData, (void*)callback, data, datasize, 0xFFFF, port, WH_DATA_PRIO);
	
	if (result != WM_ERRCODE_OPERATING) {
		WH_TRACE("WH_StateInSetMPPortData failed - %s\n", WH_GetWMErrCodeName(result));
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutSetMPData(void* callback) {
	WMPortSendCallback* cb = (WMPortSendCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS && cb->errcode != WM_ERRCODE_SEND_FAILED) {
		WH_SetError(cb->errcode);
		return;
	}
	
	if (cb->arg) {
		WHSendCallbackFunc callback = (WHSendCallbackFunc)cb->arg;
		callback(cb->port, (cb->errcode == WM_ERRCODE_SUCCESS));
	}
}


static void WH_PortReceiveCallback(void* callback) {
	WMPortRecvCallback* cb = (WMPortRecvCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
	} else if (sReceiverFuncs[cb->port] != NULL && cb->state == WM_STATECODE_PORT_RECV) {
		sReceiverFuncs[cb->port](cb->port, cb->aid, cb->data, cb->length);
	}
}


static void WH_StateOutEnd(void* callback) {
	WMCallback* cb = (WMCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_ChangeSysState(WH_SYSSTATE_FATAL);
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_STOP);
}


void WH_SetGgid(u32 ggid) {
	sParentParam.ggid = ggid;
}


int WH_GetSystemState(void) {
	return sSysState;
}


BOOL WH_Initialize(void) {
	sRecvBufferSize = 0;
	sSendBufferSize = 0;
	
	for (u16 i = 0; i < 16; i++) {
		sReceiverFuncs[i] = NULL;
	}
	
	sMyAid = 0;
	sConnectBitmap = WH_BITMAP_EMPTY;
	sErrCode = WM_ERRCODE_SUCCESS;
	
	sParentParam.userGameInfo = NULL;
	sParentParam.userGameInfoLength = 0;
	
	MI_CpuClear8(sConnectionSsid, sizeof(sConnectionSsid));
	sJudgeAcceptFunc = NULL;
	
	if (!WH_StateInInitialize()) {
		return FALSE;
	}
	
	return TRUE;
}


static void WH_IndicateHandler(void* callback) {
	WMindCallback* cb = (WMindCallback*)callback;
	
	if (cb->errcode == WM_ERRCODE_FIFO_ERROR) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		WH_ASSERT(FALSE);
	}
}


static BOOL WH_StateInInitialize(void) {
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	WMErrCode result = WM_Initialize(&sWmBuffer, WH_StateOutInitialize, WH_DMA_NO);
	if (result != WM_ERRCODE_OPERATING) {
		WH_SetError(result);
		WH_ChangeSysState(WH_SYSSTATE_FATAL);
		return FALSE;
	}
	
	return TRUE;
}


static void WH_StateOutInitialize(void* callback) {
	WMCallback* cb = (WMCallback*)callback;
	
	if (cb->errcode != WM_ERRCODE_SUCCESS) {
		WH_SetError(cb->errcode);
		WH_ChangeSysState(WH_SYSSTATE_FATAL);
		return;
	}
	
	WMErrCode result = WM_SetIndCallback(WH_IndicateHandler);
	if (result != WM_ERRCODE_SUCCESS) {
		WH_SetError(result);
		WH_ChangeSysState(WH_SYSSTATE_FATAL);
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_IDLE);
}


BOOL WH_ChildConnect(int mode, WMBssDesc* bssDesc) {
	WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);
	
	sRecvBufferSize = WH_CHILD_RECV_BUFFER_SIZE;
	sSendBufferSize = WH_CHILD_SEND_BUFFER_SIZE;
	
	WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
	WH_TRACE("send buffer size = %d\n", sSendBufferSize);
	
	sConnectMode = mode;
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	switch (mode) {
		case WH_CONNECTMODE_MP_CHILD:
		case WH_CONNECTMODE_KS_CHILD:
		case WH_CONNECTMODE_DS_CHILD:
			MI_CpuCopy8(bssDesc, &sBssDesc, sizeof(WMBssDesc));
			DC_FlushRange(&sBssDesc, sizeof(WMBssDesc));
			DC_WaitWriteBufferEmpty();
			if (sChildWEPKeyGenerator) {
				return WH_StateInSetChildWEPKey();
			} else {
				return WH_StateInStartChild();
			}
			
			break;
	}
	
	WH_TRACE("unknown connect mode %d\n", mode);
	return FALSE;
}


void WH_SetReceiver(u16 port, WHReceiverFunc proc) {
	WH_ASSERT(port < 16);
	
	sReceiverFuncs[port] = proc;
	
	if (WM_SetPortCallback(port, WH_PortReceiveCallback, NULL) != WM_ERRCODE_SUCCESS) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		WH_TRACE("WM not Initialized\n");
	}
}


BOOL WH_SendData(u16 port, const void* data, u16 size, WHSendCallbackFunc callback) {
	WH_ASSERT(port < 16);
	return WH_StateInSetMPData(port, data, size, callback);
}


void WH_Reset(void) {
	if (sSysState == WH_SYSSTATE_DATASHARING) {
		WMErrCode result = WM_EndDataSharing(&sDSInfo);
		if (result != WM_ERRCODE_SUCCESS) {
			WH_SetError(result);
		}
	}
	
	if (!WH_StateInReset()) {
		WH_ChangeSysState(WH_SYSSTATE_FATAL);
	}
}


void WH_Finalize(void) {
	if (sSysState == WH_SYSSTATE_IDLE) {
		WH_TRACE("already WH_SYSSTATE_IDLE\n");
		return;
	}
	
	WH_TRACE("WH_Finalize, state = %d\n", sSysState);
	
	if (sSysState == WH_SYSSTATE_SCANNING) {
		if (!WH_EndScan()) {
			WH_Reset();
		}
		return;
	}
	
	if (sSysState != WH_SYSSTATE_KEYSHARING && sSysState != WH_SYSSTATE_DATASHARING && sSysState != WH_SYSSTATE_CONNECTED) {
		WH_ChangeSysState(WH_SYSSTATE_BUSY);
		WH_Reset();
		return;
	}
	
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	
	switch (sConnectMode) {
		case WH_CONNECTMODE_KS_CHILD:
			if (!WH_StateInEndChildKeyShare()) {
				WH_Reset();
			}
			break;
		
		case WH_CONNECTMODE_DS_CHILD:
			if (WM_EndDataSharing(&sDSInfo) != WM_ERRCODE_SUCCESS) {
				WH_Reset();
				break;
			}
			// Fallthrough
		
		case WH_CONNECTMODE_MP_CHILD:
			if (!WH_StateInEndChildMP()) {
				WH_Reset();
			}
			break;
		
		case WH_CONNECTMODE_KS_PARENT:
			if (!WH_StateInEndParentKeyShare()) {
				WH_Reset();
			}
			break;
		
		case WH_CONNECTMODE_DS_PARENT:
			if (WM_EndDataSharing(&sDSInfo) != WM_ERRCODE_SUCCESS) {
				WH_Reset();
				break;
			}
			// Fallthrough
		
		case WH_CONNECTMODE_MP_PARENT:
			if (!WH_StateInEndParentMP()) {
				WH_Reset();
			}
			break;
	}
}


BOOL WH_End(void) {
	WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);
	
	WH_ChangeSysState(WH_SYSSTATE_BUSY);
	if (WM_End(WH_StateOutEnd) != WM_ERRCODE_OPERATING) {
		WH_ChangeSysState(WH_SYSSTATE_ERROR);
		return FALSE;
	}
	
	return TRUE;
}
