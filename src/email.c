#include <nitro.h>
#include <string.h>

#include "email.h"


int Email_GetSaveSize(void) {
	return sizeof(EmailSave);
}


void Email_Init(EmailSave* email) {
	MI_CpuClear8(email, sizeof(EmailSave));
	memset(email->emailStr, 0, EMAIL_MAX_LENGTH + 1);
	email->unk_34 = 1;
}
