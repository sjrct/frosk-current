//
// include/lib/string.h
// include/prgm/string.h
// include/kernel/string.h
//

#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

size_t strlen (const char *);
char * strcpy (char *,       const char *);
int    strcmp (const char *, const char *);
int    strncmp(const char *, const char *, size_t);
void * memcpy (void *,       const void *, size_t);
int    memcmp (const void *, const void *, size_t);

#endif
