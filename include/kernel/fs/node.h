/*
 * include/util/fs/node.h
 * include/kernel/fs/node.h
 */

#ifndef _FS_NODE_H_
#define _FS_NODE_H_

#include <types.h>

typedef dword fptr;

/* Node types */
typedef enum fnode_type {
	FNODE_FREE = 0,
	FNODE_FILE,
	FNODE_DIRECTORY,
	FNODE_INDIRECT,
	FNODE_STRING,
	FNODE_BLOCK,
	FNODE_BRANCH,
} fnode_type_t;

/* Permission masks */
#define FPERM_READ_OWN 0x01
#define FPERM_READ_ALL 0x03
#define FPERM_WRIT_OWN 0x04
#define FPERM_WRIT_ALL 0x0c
#define FPERM_EXEC_OWN 0x10
#define FPERM_EXEC_ALL 0x30

/* Mark found at begining of filesystem */
#define FROSK_FS_MARK "Frosk FS"

/* This is the size of the data before the union in fnode_t */
#define FNODE_ADD_SIZE (2 * sizeof(byte))

/* This, given an fnode_t, returns the char* to the string contained in it */
#define FNODE_STRING(N) ((char *)(N) + FNODE_ADD_SIZE + sizeof(fnode_string_t))

#pragma pack(push, 1)
typedef struct fmeta_header {
	byte  mark[8];
	dword page_size;
	dword flags;
	fptr  root;
	fptr  free_meta;
	fptr  free_data;
	fptr  strtbl;
} fmeta_header_t;

typedef struct fnode_free {
	word size;
	fptr next;
} fnode_free_t;

typedef struct fnode_common {
	word perm;
	fptr name;
	fptr owner;
	fptr next;
	fptr data;
} fnode_common_t;

typedef struct fnode_string {
	word size;
	fptr next;
	fptr left;
	fptr right;
} fnode_string_t;

typedef struct fnode_block {
	word pack;
	fptr next;
	qword lba;
	qword size;
} fnode_block_t;

typedef struct fnode_branch {
	byte subs;
	byte pack[5];
	qword lba1;
	qword lba2;
	qword os_use;
} fnode_branch_t;

typedef struct fnode {
	byte type;
	byte next;

	union {
		fnode_free_t   free;
		fnode_common_t common;
		fnode_string_t string;
		fnode_block_t  block;
		fnode_branch_t branch;
	} u;
} fnode_t;
#pragma pack(pop)

#endif
