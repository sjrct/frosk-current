//
// util/fsb/src/parse.c
//

#include <assert.h>
#include <stdlib,h>
#include "parse.h"

unsigned parse_perm(const char * str)
{
    unsigned p = 0;

    while (*str != 0) {
        switch (*str) {
        case 'r': p |= PERM_READ_OWN; break;
        case 'w': p |= PERM_WRIT_OWN; break;
        case 'e': p |= PERM_EXEC_OWN; break;
        case 'R': p |= PERM_READ_ALL; break;
        case 'W': p |= PERM_WRIT_ALL; break;
        case 'E': p |= PERM_EXEC_ALL; break;
        default:
            fprintf(stderr, "warning: unrecognized character '%c' in permission string.\n", *str);
        }
        str++;
    }

    return p;
}

static void igspaces(const char ** buf)
{
    while (isspace(**buf) || **buf == '#') {
        if (**buf == '#')
            while (**buf != '\n') ++*buf;
        ++*buf;
    }
}

static void expect(const char ** buf, char x)
{
    igspaces(buf);

    if (**buf != x) {
        fprintf(stderr, "error: expected '%c', got '%c'.\n", x, **buf);
        exit(EXIT_FAILURE);
    }

    ++*buf;
}

void parse_fs(const char * buf, fs_t * fs)
{
    
}
