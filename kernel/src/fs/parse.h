//
// kernel/src/fs/parse.h
//

#ifndef FS_PARSE_H
#define FS_PARSE_H

#include <types.h>

typedef struct path_parse {
	uint len;
	const char * name;
	struct path_parse * next;
} path_parse_t;

path_parse_t * parse_path(const char *);
void free_path_parse(path_parse_t *);

#endif
