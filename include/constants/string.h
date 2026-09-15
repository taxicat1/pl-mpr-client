#ifndef MPR_CONSTANTS_STRING_H
#define MPR_CONSTANTS_STRING_H

#define MON_NAME_LEN          10
#define TRAINER_NAME_LEN      7
#define TABLET_NAME_LEN       10
#define MOVE_NAME_LEN         16
#define UNION_GROUP_NAME_LEN  7
#define BOX_NAME_LEN          8

typedef enum {
	PADDING_MODE_NONE = 0,  // Do not pad the string. Effectively left-justification.
	PADDING_MODE_SPACES,    // Right-justify the string, padding with spaces.
	PADDING_MODE_ZEROES     // Right-justify the string, padding with zeroes.
} PaddingMode;

#endif /* MPR_CONSTANTS_STRING_H */
