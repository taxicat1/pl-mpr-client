#ifndef _STDARG_H
#define _STDARG_H

/* Stub */

typedef char* va_list;

#define va_start(list, arg)                        \
    do {                                           \
        list = (va_list)(((u32)&(arg) & ~3) + 4);  \
    } while(0)

#define va_end(list)  ((void)0)

#endif /* _STDARG_H */
