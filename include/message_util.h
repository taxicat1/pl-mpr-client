#ifndef MPR_MESSAGE_UTIL_H
#define MPR_MESSAGE_UTIL_H

#include "message.h"
#include "heap.h"
#include "string_gf.h"
#include "string_template.h"

#define EXPANDED_STRING_SIZE 1024

String* MessageUtil_ExpandedString(StringTemplate* template, MessageLoader* loader, u32 entryID, HeapID heapID);
String* MessageUtil_SpeciesName(u32 species, HeapID heapID);

#endif /* MPR_MESSAGE_UTIL_H */