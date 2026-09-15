#ifndef MPR_EMAIL_H
#define MPR_EMAIL_H

#include <nitro/types.h>

#define EMAIL_MAX_LENGTH  50

typedef struct {
    char  emailStr[EMAIL_MAX_LENGTH + 1];
    u8    padding_33[1];
    int   unk_34;
    u16   unk_38;
    u16   unk_3A;
    u32   unk_3C;
} EmailSave;

int Email_GetSaveSize(void);
void Email_Init(EmailSave* email);

#endif /* MPR_EMAIL_H */
