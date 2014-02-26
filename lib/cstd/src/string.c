//
// lib/cstd/src/string.c
// kernel/src/cstd/string.c
//

#include <string.h>

size_t strlen(const char * a)
{
	size_t i;
	for (i = 0; a[i] != 0; i++) i++;
	return i;
}

char * strcpy(char * d, const char * s)
{
	while (*s != 0) {
		*d = *s;
		d++;
		s++;
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
