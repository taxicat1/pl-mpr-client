#include <nitro.h>

#include "mail.h"

#include "heap.h"


void Mail_Reset(Mail* mail) {
	// Unimplemented
	return;
}


Mail* Mail_New(HeapID heapID) {
	Mail* mail = Heap_AllocAtEnd(heapID, sizeof(Mail));
	Mail_Reset(mail);
	return mail;
}


void Mail_Copy(Mail* src, Mail* dest) {
	MI_CpuCopy8(src, dest, sizeof(Mail));
}


int Mailbox_GetSaveSize(void) {
	return sizeof(Mailbox);
}


void Mailbox_Init(Mailbox* mailbox) {
	for (int i = 0; i < MAILBOX_SIZE; i++) {
		Mail_Reset(&mailbox->mail[i]);
	}
}
