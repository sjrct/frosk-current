//
// util/fsb/src/fs.h
//

#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <fs/node.h>

struct meta;

typedef struct node {
	fnode_type_t type;
	unsigned flags;
	unsigned size;
	unsigned ptr;
	struct meta * location;
	struct node * next;
	struct node * prev;

	union {
		struct {
			struct node * next;
			struct node * prev;
		} free;

		struct {
			unsigned short perm;
			struct node * name;
			struct node * owner;
			struct node * next;
			struct node * data;
		} common;

		struct {
			word size;
			struct node * next;
			struct node * left;
			struct node * right;
			char * value;
		} string;

		struct {
			char * filename;
			struct node * next;
			unsigned long lba;
			unsigned long size;
		} block;

		struct {
			struct meta * sub;
		} branch;
	} u;
} node_t;

typedef struct meta {
	qword lba;
	fptr base;
	uint shift;
	uint subs;
	node_t * first;
} meta_t;

typedef struct fs {
	uint block_size;
	qword next_lba;
	meta_t * first_meta;
	node_t * root;
	node_t * free_data;
	node_t * free_meta;
	node_t * strtbl;
} fs_t;

#define ENTRY_SIZE (fs.block_size / 0x100)

extern fs_t fs;

meta_t * new_meta(node_t *);
node_t * new_node(uint);
node_t * new_file (const char *, const char *, node_t *);
node_t * new_virt (const char *, ulong, ulong, node_t *);
node_t * new_dir  (const char *, node_t *);
node_t * new_indir(const char *, const char *, node_t *);
node_t * get_string(const char *);

void write_fs(FILE *);
void debug_fs(FILE *);

#endif
