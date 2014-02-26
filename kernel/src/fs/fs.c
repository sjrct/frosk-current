//
// kernel/src/fs/fs.c
//

#include <string.h>
#include <fs/node.h>
#include "fs.h"
#include "../debug.h"
#include "../memory/kernel.h"

static fs_entry_t * root_ent = NULL;

static int lenof(const char * path)
{
	int l = 0;
	while (*path != 0 && *path != '/') path++;
	return l;
}

static fs_entry_t ** find(const char * path, uint len)
{
	int cmp;
	fs_entry_t ** p = &root_ent;

	while (*p != NULL) {
		if (len > (*p)->length) {
			p = &(*p)->left;
		} else if (len < (*p)->length) {
			p = &(*p)->right;
		} else {
			cmp = memcmp(path, (*p)->name, len);
			if (cmp > 0) {
				p = &(*p)->left;
			} else if (cmp < 0) {
				p = &(*p)->right;
			} else {
				break;
			}
		}
	}

	if (*p != NULL) {
		path += len;
		if (*path == '/')
			return find(path + 1, lenof(path + 1));

		assert(*path == 0);
	}

	return p;
}

fs_entry_t * fs_aquire(const char * path)
{
	// TODO locking
	return *find(path, lenof(path));
}

void fs_enter(char * name, uint len, char * value, uint size)
{
	fs_entry_t ** p = find(name, len);

	if (*p == NULL) {
		*p = kalloc(sizeof(fs_entry_t));
		(*p)->name   = name;
		(*p)->length = len;
		(*p)->type   = FNODE_FILE;
		(*p)->flags  = FS_ENT_VIRTUAL;
		(*p)->left   = NULL;
		(*p)->right  = NULL;
		(*p)->u.file.data  = value;
		(*p)->u.file.size  = size;
		(*p)->u.file.flags = FS_FILE_PRESENT;
	} else {
		if ((*p)->flags & FS_FILE_HASNAME) kfree((*p)->name);
		if ((*p)->flags & FS_FILE_HASDATA) kfree((*p)->u.file.data);
		dprintf("fs_enter overwrite");
	}
}
