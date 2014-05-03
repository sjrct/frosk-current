//
// kernel/src/fs/parse.c
//

#include <stddef.h>
#include "parse.h"
#include "../memory/kernel.h"

path_parse_t * parse_path(const char * path)
{
	path_parse_t * c, * p;
	path_parse_t * s = NULL;

	while (*path) {
		c = kalloc(sizeof(path_parse_t));

		if (s == NULL) s = c;
		else p->next = c;

		c->name = path;
		c->len  = 0;
		c->next = NULL;

		while (*path && *path != '/') {
			c->len++;
			path++;
		}

		p = c;
	}

	return s;
}

void free_path_parse(path_parse_t * prs)
{
	if (prs == NULL) return;
	free_path_parse(prs->next);
	kfree(prs);
}
