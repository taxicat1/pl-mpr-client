#ifndef MPR_TV_BROADCAST_H
#define MPR_TV_BROADCAST_H

/* Unimplemented */
#define TV_BROADCAST_SIZE_DP  0xB88
#define TV_BROADCAST_SIZE_PT  0xBC8

typedef struct {
	u8 dat[TV_BROADCAST_SIZE_DP];
} TVBroadcastDP;

typedef struct {
	u8 dat[TV_BROADCAST_SIZE_PT];
} TVBroadcastPt;

typedef void TVBroadcastCommon;

int TVBroadcast_GetSaveSize(void);
void TVBroadcast_Init(TVBroadcastCommon* tvBroadcast);

#endif /* MPR_TV_BROADCAST_H */
