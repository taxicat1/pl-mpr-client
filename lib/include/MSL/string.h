#ifndef _STRING_H
#define _STRING_H

/* Stub */

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void*, const void*, size_t);
void* memset(void*, int, size_t);
int memcmp(const void*, const void*, size_t);
size_t strlen(const char *);
int strcmp(const char*, const char*);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H */
