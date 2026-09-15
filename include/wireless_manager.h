#ifndef MPR_WIRELESS_MANAGER_H
#define MPR_WIRELESS_MANAGER_H

#include <nitro/wm.h>

enum WirelessManagerState {
	WH_SYSSTATE_STOP,
	WH_SYSSTATE_IDLE,
	WH_SYSSTATE_SCANNING,
	WH_SYSSTATE_BUSY,
	WH_SYSSTATE_CONNECTED,
	WH_SYSSTATE_DATASHARING,
	WH_SYSSTATE_KEYSHARING,
	WH_SYSSTATE_MEASURECHANNEL,
	WH_SYSSTATE_CONNECT_FAIL,
	WH_SYSSTATE_ERROR,
	WH_SYSSTATE_FATAL,
	WH_SYSSTATE_NUM
};

enum WirelessConnectionType {
	WH_CONNECTMODE_MP_PARENT,
	WH_CONNECTMODE_MP_CHILD,
	WH_CONNECTMODE_KS_PARENT,
	WH_CONNECTMODE_KS_CHILD,
	WH_CONNECTMODE_DS_PARENT,
	WH_CONNECTMODE_DS_CHILD,
	WH_CONNECTMODE_NUM
};

// Extension of the NitroSDK WMErrCode enum
enum ExtendedWMErrCode {
	WH_ERRCODE_DISCONNECTED = WM_ERRCODE_MAX, // =20
	WH_ERRCODE_PARENT_NOT_FOUND,
	WH_ERRCODE_NO_RADIO,
	WH_ERRCODE_LOST_PARENT,
	WH_ERRCODE_NOMORE_CHANNEL,
	WH_ERRCODE_MAX
};

typedef void (*WHStartScanCallbackFunc)(WMBssDesc*);
typedef void (*WHSendCallbackFunc)(u16, BOOL);
typedef void (*WHReceiverFunc)(u16, u16, u16*, u16);

#define WH_DS_DATA_SIZE     128
#define WH_CHILD_MAX        1

// TODO fix macros that programmatically determine buffer size
//#define WH_PARENT_MAX_SIZE  (WH_DS_DATA_SIZE * (1 + WH_CHILD_MAX) + WM_SIZE_DS_PARENT_HEADER)
//#define WH_CHILD_MAX_SIZE   WH_DS_DATA_SIZE

//#define WH_PARENT_RECV_BUFFER_SIZE   WM_SIZE_MP_PARENT_RECEIVE_BUFFER(WH_CHILD_MAX_SIZE, WH_CHILD_MAX, FALSE)
//#define WH_PARENT_SEND_BUFFER_SIZE   WM_SIZE_MP_PARENT_SEND_BUFFER(WH_PARENT_MAX_SIZE, FALSE)

#define WH_CHILD_RECV_BUFFER_SIZE    (0x480) //WM_SIZE_MP_CHILD_RECEIVE_BUFFER(WH_PARENT_MAX_SIZE, FALSE)
#define WH_CHILD_SEND_BUFFER_SIZE    (0x220) //WM_SIZE_MP_CHILD_SEND_BUFFER(WH_CHILD_MAX_SIZE, FALSE)

BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8* macAddr, u16 channel);
BOOL WH_EndScan(void);
void WH_SetGgid(u32 ggid);
int WH_GetSystemState(void);
BOOL WH_Initialize(void);
BOOL WH_ChildConnect(int mode, WMBssDesc* bssDesc);
void WH_SetReceiver(u16 port, WHReceiverFunc proc);
BOOL WH_SendData(u16 port, const void* data, u16 size, WHSendCallbackFunc callback);
void WH_Reset(void);
void WH_Finalize(void);
BOOL WH_End(void);

#endif /* MPR_WIRELESS_MANAGER_H */
