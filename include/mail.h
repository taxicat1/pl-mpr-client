#ifndef MPR_MAIL_H
#define MPR_MAIL_H

#include "constants/string.h"

#include "charcode.h"
#include "easy_chat_sentence.h"
#include "savedata.h"

#define MAIL_MAX_SENTENCES  3
#define MAIL_MAX_ICONS      3

#define MAILBOX_SIZE  20

typedef union {
	struct {
		u16  spriteIndex : 12;
		u16  palIndex    : 4;
	} asStruct;
	u16 asValue;
} MailIconData;

typedef struct {
	u32               trainerID;
	u8                trainerGender;
	u8                language;
	u8                gameVersion;
	u8                mailType;
	charcode_t        trainerName[TRAINER_NAME_LEN + 1];
	MailIconData      iconData[MAIL_MAX_ICONS];
	u16               platExclusiveFormIcons;
	EasyChatSentence  sentences[MAIL_MAX_SENTENCES];
} Mail;

typedef struct Mailbox {
	Mail mail[MAILBOX_SIZE];
} Mailbox;

void Mail_Reset(Mail* mail);
Mail* Mail_New(HeapID heapID);
void Mail_Copy(Mail* src, Mail* dest);
int Mailbox_GetSaveSize(void);
void Mailbox_Init(Mailbox* mailbox);

#endif /* MPR_MAIL_H */
