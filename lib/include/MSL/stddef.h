#ifndef _STDDEF_H
#define _STDDEF_H

/* Stub */

#ifndef __std
#define __std(x)  (x)
#endif

#define _MSL_OFFSETOF(type, member)  ((__std(size_t)) &(((type *) 0)->member))
#define offsetof(type, member)       _MSL_OFFSETOF(type, member)

#endif /* _STDDEF_H */
