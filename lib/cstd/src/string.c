/*
 * lib/cstd/src/string.c
 * kernel/src/cstd/string.c
 */

#include <string.h>

size_t strlen(const char * a)
{
	size_t i;
	for (i = 0; a[i] != 0; i++);
	return i;
}

size_t strnlen(const char *a, size_t n)
{
	size_t i;
	for (i = 0; a[i] != 0 && i < n; i++);
	return i;
}

char * strcpy(char * d0, const char * s)
{
    char *d = d0;
	while (*s != 0) {
		*d = *s;
		d++;
		s++;
	}
	*d = 0;
	return d0;
}

char * strncpy(char * d, const char * s, size_t max)
{
    size_t i;
    for (i = 0; i < max && s[i]; i++) {
        d[i] = s[i];
    }
    return d;
}

int strcmp(const char * a, const char * b)
{
	while (*a != 0 && *b != 0) {
		if (*a != *b) {
			return ((*a > *b) << 1) - 1;
		}

		a++;
		b++;
	}

	return *a < *b ? -1 : *a > *b;
}

int strncmp(const char * a, const char * b, size_t size)
{
	size_t i;

	for (i = 0; i < size; i++) {
		if (a[i] != b[i]) {
			return ((a[i] > b[i]) << 1) - 1;
		}

		if (*a == 0) break;
	}

	return 0;
}

void * memcpy(void * vdest, const void * vsrc, size_t size)
{
	size_t i;
	char * dest = vdest;
	const char * src  = vsrc;

	for (i = 0; i < size; i++) {
		dest[i] = src[i];
	}

	return vdest;
}

void *memset(void *vptr, int value, size_t num) {
    size_t i;
    char *ptr = vptr;

    for (i = 0; i < num; i++) ptr[i] = value;

    return vptr;
}

int memcmp(const void * va, const void * vb, size_t size)
{
	size_t i;
	const char * a = va;
	const char * b = vb;

	for (i = 0; i < size; i++) {
		if (a[i] != b[i]) {
			return ((a[i] > b[i]) << 1) - 1;
		}
	}

	return 0;
}

#ifndef FROSK_KERNEL
#include <stdlib.h>

char * strdup(const char *src) {
    size_t len = strlen(src) + 1;
    char *dst = malloc(len);
    memcpy(dst, src, len);
    dst[len] = 0;
    return dst;
}

char * strndup(const char *src, size_t max) {
    size_t len = strnlen(src, max);
    char *dst = malloc(len + 1);
    memcpy(dst, src, len);
    dst[len] = 0;
    return dst;
}
#endif
