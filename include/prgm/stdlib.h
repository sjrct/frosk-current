#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

void __attribute__((noreturn)) exit(int);

void *malloc(size_t);
void free(void *);
void *realloc(void *, size_t);
void *calloc(size_t, size_t);

#define RAND_MAX (1 << 31)

unsigned rand(void);
void srand(unsigned);

#endif
