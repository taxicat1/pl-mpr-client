#ifndef MPR_STRUCT_DEF_CHATOT_CRY_H
#define MPR_STRUCT_DEF_CHATOT_CRY_H

#define CHATOT_CRY_SIZE  1000

typedef struct {
	BOOL  valid;
	s8    audioData[CHATOT_CRY_SIZE];
} ChatotCry;

#endif /* MPR_STRUCT_DEF_CHATOT_CRY_H */
