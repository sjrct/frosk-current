//
// util/fsb/src/parse.h
//

#ifndef PARSE_H
#define PARSE_H

#include "fs.h"

unsigned parse_perm(const char *);
void parse_fs(const char *, fs_t *);

#endif
