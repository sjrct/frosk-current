//
// kernel/src/fs/fs.c
//

#include <stddef.h>
#include <string.h>
#include "fs.h"
#include "../debug.h"
#include "../dev/dev.h"
#include "../memory/kernel.h"

fs_entry_t * root_dir = NULL;

static fs_entry_t * create_entry(const char * name, fs_entry_t * lpar, uint type)
{
	fs_entry_t * x = kalloc(sizeof(fs_entry_t));

	if (name != NULL) {
		x->name = kalloc(strlen(name) + 1);
		strcpy(x->name, name);
	}

	x->left   = NULL;
	x->right  = NULL;
	x->parent = lpar;
	x->type   = type;
	x->flags  = 0;

	switch (type) {
	case FS_ENT_FILE:
		x->u.file.data  = NULL;
		x->u.file.size  = 0;
		break;
	case FS_ENT_DIRECTORY:
		x->u.directory.root = NULL;
		break;
	case FS_ENT_INDIRECT:
		x->u.indirect.dest = NULL;
		break;
	}

	return x;
}

void setup_fs(void)
{
	root_dir = create_entry(NULL, NULL, FS_ENT_DIRECTORY);
}

static fs_entry_t ** find(const char * name, fs_entry_t * par, fs_entry_t ** lpar)
{
	int cmp;
	fs_entry_t ** x;

	assert(par != NULL);
	assert(par->type == FS_ENT_DIRECTORY);

	x = &par->u.directory.root;
	*lpar = NULL;

	// assure the parent directory is present
	if (root_dev != NULL && (~par->flags & FS_ENT_PRESENT)) {
		// TODO
		assert(0);
	}

	// look for the file
	while (*x != NULL) {
		cmp = strcmp(name, (*x)->name);

		if (cmp < 0) {
			*lpar = *x;
			x = &(*x)->left;
		} else if (cmp > 0) {
			*lpar = *x;
			x = &(*x)->right;
		} else {
			return x;
		}
	}

	return x;
}

static void decimate(fs_entry_t *);

static void destroy(fs_entry_t * x)
{
	if (x->flags & FS_ENT_HASNAME) {
		kfree(x->name);
	}

	destroy(x->left);
	destroy(x->right);
	decimate(x);
	kfree(x);
}

static void decimate(fs_entry_t * x)
{
	// destroys only enough so that it can be overwritten
	if (x->flags & FS_ENT_HASDATA) {
		switch (x->type) {
		case FS_ENT_FILE:
			kfree(x->u.file.data);
			break;
		case FS_ENT_DIRECTORY:
			destroy(x->u.directory.root);
			break;
		case FS_ENT_INDIRECT:
			kfree(x->u.indirect.dest);
			break;
		}
	}
}

fs_entry_t * fs_resolve(const char * name, fs_entry_t * par)
{
	fs_entry_t * lpar;
	assert(par != NULL);
	return *find(name, par, &lpar);
}

// FIXME: don't use memory allocation
fs_entry_t * fs_retrieve(const char * name0, fs_entry_t * x)
{
	char * old, * name, * local;

	old = name = kalloc(strlen(name0) + 1);
	strcpy(name, name0);

	if (*name == '/') {
		x = root_dir;
		name++;
	}

	assert(x != NULL);

	while (*name != 0) {
		local = name;
		while (*name != '/' && *name != 0) ++name;

		if (*name == '/') {
			*name = 0;
			name++;
		}

		assert(x->type == FS_ENT_DIRECTORY);
		x = fs_resolve(local, x);
	}

	kfree(old);
	return x;
}

static fs_entry_t * leftmost(fs_entry_t * x)
{
	if (x == NULL) return x;
	while (x->left != NULL && x->left != NULL) {
		x = x->left;
	}
	return x;
}

fs_entry_t * fs_first(fs_entry_t * x)
{
	assert(x->type == FS_ENT_DIRECTORY);
	return leftmost(x->u.directory.root);
}

fs_entry_t * fs_next(fs_entry_t * x)
{
	if (x->right != NULL) {
		return leftmost(x->right);
	}

	while (x->parent != NULL && x->parent->right == x) {
		x = x->parent;
	}
	return x->parent;
}

fs_entry_t * fs_mkdir(const char * name, fs_entry_t * par)
{
	fs_entry_t * lpar;
	fs_entry_t ** dir;

	dir = find(name, par, &lpar);

	if (*dir != NULL) decimate(*dir);
	*dir = create_entry(name, lpar, FS_ENT_DIRECTORY);
	(*dir)->flags |= FS_ENT_PRESENT | FS_ENT_HASDATA;

	return *dir;
}

fs_entry_t * fs_mkvdir(const char * name, fs_entry_t * par)
{
	fs_entry_t * e = fs_mkdir(name, par);
	e->flags |= FS_ENT_VIRTUAL;
	return e;
}

fs_entry_t * fs_enter(const char * name, void * data, ulong size, fs_entry_t * par)
{
	fs_entry_t * lpar;
	fs_entry_t ** ent;

	ent = find(name, par, &lpar);

	if (*ent != NULL) decimate(*ent);
	*ent = create_entry(name, lpar, FS_ENT_FILE);

	(*ent)->flags |= FS_ENT_VIRTUAL | FS_ENT_PRESENT;
	(*ent)->u.file.data = data;
	(*ent)->u.file.size = size;

	return *ent;
}
