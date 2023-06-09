/*
 * include/lib/string.h
 * include/prgm/string.h
 * include/kernel/string.h
 */

#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

size_t strlen (const char *);
char * strcpy (char *,       const char *);
char * strncpy(char *,       const char *, size_t);
int    strcmp (const char *, const char *);
int    strncmp(const char *, const char *, size_t);
void * memcpy (void *,       const void *, size_t);
void * memset (void *,       int,          size_t);
int    memcmp (const void *, const void *, size_t);

#ifndef FROSK_KERNEL
/* Not available to the kernel */
char * strdup (const char *);
char * strndup(const char *, size_t);
#endif

#endif
