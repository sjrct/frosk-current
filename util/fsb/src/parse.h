//
// parse.h
//

#ifndef PARSE_H
#define PARSE_H

#include <types.h>
#include <stdio.h>
#include "fs.h"

typedef struct source {
	FILE * f;
	int ungot;
	int offset;
	char * str;
} source_t;

extern char * input_file;
extern int line_num;

node_t * parse_directory(source_t *, node_t *);
void parse_fsd(source_t *, uint, ulong, uint, char *);
uint parse_perm(const char *);

source_t * source_file(FILE *);
source_t * source_string(char *);

#endif
