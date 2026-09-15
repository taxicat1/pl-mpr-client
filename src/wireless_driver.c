#include <nitro.h>

#include "wireless_driver.h"

#include "system.h"

typedef enum {
	WIRELESS_DRIVER_STATUS_DISCONNECTED = 0,
	WIRELESS_DRIVER_STATUS_CONNECTING,
	WIRELESS_DRIVER_STATUS_CONNECTED,
} WirelessDriverState;

static void WirelessDriver_InitCallback(void* unused, WVRResult result);

// Unused BSS
void Dummy_Unused_02133AC8(void);
void Dummy_Unused_02133AC8(void) {
	static struct {
		u8 data[4];
	} unused_02133AC8[6];
}

static volatile WirelessDriverState sWirelessDriverStatus;

// Unused BSS
void Dummy_Unused_02133AC0(void);
void Dummy_Unused_02133AC0(void) {
	static struct {
		u8 data[4];
	} unused_02133AC0;
}


static void WirelessDriver_InitCallback(void* unused, WVRResult result) {
	if (result != WVR_RESULT_SUCCESS) {
		OS_Terminate();
	}
	
	sWirelessDriverStatus = WIRELESS_DRIVER_STATUS_CONNECTED;
}


void WirelessDriver_Init(void) {
    SleepLock(4);
    sWirelessDriverStatus = WIRELESS_DRIVER_STATUS_CONNECTING;
	
	WVRResult result = WVR_StartUpAsync(GX_VRAM_ARM7_128_D, WirelessDriver_InitCallback, NULL);
	
	if (result == WVR_RESULT_DISABLE) {
		// ? Debug remnant?
		// WVR_RESULT_DISABLE: Indicates that the ARM7-side component is not supported in the WVR library.
		sWirelessDriverStatus = WIRELESS_DRIVER_STATUS_CONNECTED;
	} else if (result != WVR_RESULT_OPERATING) {
        OS_Terminate();
    }
}


BOOL WirelessDriver_IsReady(void) {
	return (sWirelessDriverStatus == WIRELESS_DRIVER_STATUS_CONNECTED);
}
