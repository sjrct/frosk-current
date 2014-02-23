//
// parse.c
//

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"
#include "parse.h"
#include "directive.h"

char * input_file = "<stdin>";
int line_num = 1;

static void error(const char * fmt, ...)
{
	extern const char * myname;
	va_list ls;
	va_start(ls, fmt);

	fprintf(stderr, "%s: error (%s, line %d): ", myname, input_file, line_num);
	vfprintf(stderr, fmt, ls);
	fputs("\n", stderr);

	va_end(ls);
}

static int src_getc(source_t * in)
{
	int c;

	if (in->ungot < 0) {
		if (in->f != NULL) {
			c = fgetc(in->f);
		} else {
			c = in->str[in->offset];
			if (c != 0) in->offset++;
			else c = EOF;
		}
	} else {
		c = in->ungot;
		in->ungot = -1;
	}

	return c;
}

static void src_ungetc(int c, source_t * in)
{
	assert(in->ungot < 0);
	in->ungot = c;
}

static int nextc(source_t * in)
{
	int c;
	do {
		c = src_getc(in);

		if (c == '#') {
			while (c != '\n') c = src_getc(in);
		}

		if (c == '\n') line_num++;
	} while (isspace(c));
	return c;
}

static void expect(int c, source_t * in)
{
	int got = nextc(in);
	if (got != c) {
		error("Expected '%c', got '%c'.", c, got);
	}
}

static int peekc(source_t * in)
{
	int c = nextc(in);
	src_ungetc(c, in);
	return c;
}

static char * getstr(source_t * in, char * term)
{
#define GROW_BY 0x10
#define APPEND(S,I,C) \
	if (I % GROW_BY == 0) { \
		S = realloc(S, I + GROW_BY); \
	} \
	S[I++] = C;

	char * str = NULL;
	int c, i, tlen, sz = 0;

	tlen = strlen(term);
	c = nextc(in);

	if (c == '{') {
		c = src_getc(in);

		while (c != '}' && c != EOF) {
			APPEND(str, sz, c);
			c = src_getc(in);
		}
	} else {
		while (!isspace(c)) {
			for (i = 0; i < tlen; i++) {
				if (term[i] == c) {
					src_ungetc(c, in);
					APPEND(str, sz, 0);
					return str;
				}
			}

			APPEND(str, sz, c);
			c = src_getc(in);
		}
	}

	APPEND(str, sz, 0);
	return str;

#undef APPEND
#undef GROW_BY
}

static node_t * parse_next(source_t * in, node_t * parent)
{
	directive_t * d;
	char * left, * right;
	int first = 1;
	int type;

	type = nextc(in);
	expect(':', in);
	expect('[', in);

	d = get_directive(type, parent);
	if (d != NULL) {
		while (peekc(in) != ']') {
			if (!first) expect(',', in);
			first = 0;

			left = getstr(in, "=]");
			expect('=', in);
			right = getstr(in, ",]");

			if (!set_field(d, left, right)) {
				error("Field '%s' invalid for directive '%c'.", left, type);
				free(right);
			}

			free(left);
		}

		if (!confirm_directive(d)) {
			error("Could not verify directive '%c'.", type);
		}

		expect(']', in);
	} else {
		error("Unknown directive type '%c'.", type);
	}

	return d->nd;
}

node_t * parse_directory(source_t * in, node_t * dir)
{
	while (peekc(in) != EOF) {
		parse_next(in, dir);
	}

	return dir;
}

void parse_fsd(source_t * in, uint bs, ulong sb, uint rperm, char * rowner)
{
	fs.block_size = bs;
	fs.next_lba   = sb;
	fs.free_data  = NULL;
	fs.free_meta  = NULL;
	fs.strtbl     = NULL;
	fs.first_meta = new_meta(NULL);

	fs.root = new_dir(NULL, NULL);
	fs.root->u.common.perm = rperm;
	fs.root->u.common.owner = get_string(rowner);

	parse_directory(in, fs.root);
}

unsigned parse_perm(const char * str)
{
	unsigned perm = 0;

	while (*str != 0) {
		switch (*str) {
		case 'r': perm |= FPERM_READ_OWN;  break;
		case 'R': perm |= FPERM_READ_ALL;    break;
		case 'w': perm |= FPERM_WRIT_OWN; break;
		case 'W': perm |= FPERM_WRIT_ALL;   break;
		case 'e': perm |= FPERM_EXEC_OWN;  break;
		case 'E': perm |= FPERM_EXEC_ALL;    break;
		default: error("Unknown character '%c' in permission string.\n", *str);
		}

		str++;
	}

	return perm;
}

source_t * source_file(FILE * f)
{
	source_t * s = malloc(sizeof(source_t));
	s->f = f;
	s->ungot = -1;
	s->offset = 0;
	s->str = NULL;
	return s;
}

source_t * source_string(char * str)
{
	source_t * s = malloc(sizeof(source_t));
	s->f = NULL;
	s->ungot = -1;
	s->offset = 0;
	s->str = str;
	return s;
}
