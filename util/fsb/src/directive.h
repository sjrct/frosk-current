//
// directive.h
//

#ifndef DIRECTIVE_H
#define DIRECTIVE_H

#include "fs.h"

struct directive;

typedef struct directive_class {
	int mark;
	int (*confirm)(struct directive *);
	int field_count;
	char ** fields;
} directive_class_t;

typedef struct directive {
	directive_class_t * cls;
	node_t * nd, * par;
	char ** values;
} directive_t;

directive_t * get_directive(int c, node_t *);
int confirm_directive(directive_t *);
int set_field(directive_t *, const char *, char *);

#endif
