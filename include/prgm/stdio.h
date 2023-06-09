#ifndef _STDIO_H_
#define _STDIO_H_

#include <frosk.h>
#include <stddef.h>
#include <stdarg.h>

typedef struct FILE {
    file_t file;
    size_t cursor;

    char *buffer;
    size_t buffer_length; /* length of used string in buffer */
    size_t buffer_size;   /* allocated size of the buffer */

    // TODO mode
} FILE;

#define stdout ((FILE *)1)
#define stderr ((FILE *)2)

#define EOF (-1)

extern FILE _stdin;
#define stdin (&_stdin)

enum _seek {
    SEEK_SET,
    SEEK_CUR,
    SEEK_END,
};

FILE *fopen(const char *, const char *);
int fclose(FILE *);

size_t fread(void *, size_t, size_t, FILE *);
size_t fwrite(const void *, size_t, size_t, FILE *);

#define printf(...) fprintf(stdout, __VA_ARGS__)
int fprintf(FILE *, const char *, ...);
#define vprintf(FMT, LS) vfprintf(stdout, FMT, LS)
int vfprintf(FILE *, const char *, va_list);

int sprintf(char *, const char *, ...);
int snprintf(char *, size_t, const char *, ...);
int vsprintf(char *, const char *, va_list);
int vsnprintf(char *, size_t, const char *, va_list);

#define putc(C,F) fputc(C,F)
int puts(const char *);
int fputc(int, FILE *);
int fputs(const char *, FILE *);

char * fgets(char *, size_t, FILE *);

int fseek(FILE *, long, enum _seek);
long ftell(FILE *);
int feof(FILE *);

#endif
