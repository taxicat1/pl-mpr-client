#ifndef MPR_MET_LOCATION_UTIL_H
#define MPR_MET_LOCATION_UTIL_H

typedef enum {
	MET_LOCATION_TYPE_NORMAL = 0,
	MET_LOCATION_TYPE_SPECIAL,
	MET_LOCATION_TYPE_MYSTERY_GIFT,
	MET_LOCATION_TYPE_MAX
} MetLocationType;

MetLocationType MetLoc_GetBankType(u32 metLoc);
int MetLoc_GetBankEntry(u32 metLoc);
u32 MetLoc_GetByTypeAndEntry(MetLocationType type, u32 entry);

#endif /* MPR_MET_LOCATION_UTIL_H */
