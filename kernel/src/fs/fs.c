/*
 * kernel/src/fs/fs.c
 */

#include <stddef.h>
#include <string.h>
#include <common.h>
#include <fs/node.h>
#include "fs.h"
#include "../debug.h"
#include "../paging.h"
#include "../dev/dev.h"
#include "../memory/pages.h"
#include "../memory/kernel.h"

#define FS_SIZE_UNKNOWN ((ulong)-1)

fs_entry_t * root_dir = NULL;

/* Root File System Metadata Block */
static fnode_t * rfsmb = (fnode_t *)RFSMB_LOC;

/* The size of the individual nodes; this is relative to page size */
static long node_size = 0;

/* This function is called in order to create a new entry in a directory */
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
	x->node   = NULL;
	x->lock   = UNLOCKED;

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

/* This function translates a given fptr into a fnode_t* the data of which can
   be found in memory. */
static fnode_t * resolve_node(fptr p)
{
	fnode_t * n;
	fnode_t * mb = rfsmb;
	fmeta_header_t * hdr;
	ulong virt, phys;

	if (!p) return NULL;

	for (;;) {
		n = (fnode_t *)((ulong)mb + (p & 0xff) * node_size);
		p >>= 8;

		if (!p || n->type != FNODE_BRANCH) break;

		if (!n->u.branch.os_use) {
			dprintf("warning: must load meta block, I didn't test this code.\n");

			hdr = (fmeta_header_t *)rfsmb;
			phys = alloc_pgs(hdr->page_size, PHYS_PAGES);
			virt = alloc_pgs(hdr->page_size, VIRT_PAGES);
			pageto(virt, phys | KERN_PAGE_FL);

			dev_read(root_dev, (byte *)virt, n->u.branch.lba1 * hdr->page_size, hdr->page_size);

			n->u.branch.os_use = virt;
		}
		mb = (fnode_t *)(ulong)n->u.branch.os_use;
	}

	return n;
}

void setup_fs(void)
{
	fmeta_header_t * hdr = (fmeta_header_t *)rfsmb;
	node_size = hdr->page_size / 0x100;
	root_dir = create_entry(NULL, NULL, FS_ENT_DIRECTORY);
	root_dir->flags |= FS_ENT_HASDATA;
	root_dir->node = resolve_node(hdr->root);
}

static fs_entry_t * insert_entry(const char *, fs_entry_t * par, uint type);

static void assure_dir(fs_entry_t * dir)
{
	uint type;
	fs_entry_t * e;
	fnode_t * n, * name;

	assert(dir->type == FS_ENT_DIRECTORY);

	/* assures the given directory is present */
	if (root_dev != NULL && (~dir->flags & FS_ENT_PRESENT)) {
		assert(dir->flags & FS_ENT_HASDATA);
		assert(dir->node != NULL);
		assert(dir->node->type == FNODE_DIRECTORY);

		dir->flags |= FS_ENT_PRESENT;
		n = resolve_node(dir->node->u.common.data);

		while (n != NULL) {
			/* Translate node types to entry types */
			switch (n->type) {
			case FNODE_FILE:      type = FS_ENT_FILE;      break;
			case FNODE_DIRECTORY: type = FS_ENT_DIRECTORY; break;
			case FNODE_INDIRECT:  type = FS_ENT_INDIRECT;  break;
			default: assert(0);
			}

			name = resolve_node(n->u.common.name);
			assert(name->type == FNODE_STRING);

			e = insert_entry(FNODE_STRING(name), dir, type);
			e->node = n;
			switch (n->type) {
			case FNODE_FILE:
				e->u.file.size = FS_SIZE_UNKNOWN;
				break;
			case FNODE_DIRECTORY:
				e->flags |= FS_ENT_HASDATA;
				break;
			}

			n = resolve_node(n->u.common.next);
		}
	}
}

static fs_entry_t ** find(const char * name, fs_entry_t * par, fs_entry_t ** lpar)
{
	int cmp;
	fs_entry_t ** x;

	assert(par != NULL);
	assert(par->type == FS_ENT_DIRECTORY);

	x = &par->u.directory.root;
	*lpar = NULL;

	assure_dir(par);

	/* look for the file */
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

/* destroys only enough so that it can be overwritten */
static void decimate(fs_entry_t * x)
{
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

fs_entry_t * fs_retrieve(const char * name0, fs_entry_t * x)
{
	char * old, * name, * local;

	/* FIXME don't use memory allocation */
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
	assure_dir(x);
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

/* This function wraps to find and create entry to do a proper tree insertion */
static fs_entry_t * insert_entry(const char * name, fs_entry_t * par, uint type)
{
	fs_entry_t * lpar;
	fs_entry_t ** ent;

	ent = find(name, par, &lpar);

	if (*ent != NULL) decimate(*ent);
	*ent = create_entry(name, lpar, type);

	return *ent;
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
	fs_entry_t * ent = insert_entry(name, par, FS_ENT_FILE);

	ent->flags |= FS_ENT_VIRTUAL;
	ent->u.file.data = data;
	ent->u.file.size = size;

	return ent;
}

/* Returns the size of the file */
ulong fs_size(fs_entry_t * e)
{
	fnode_t * n;

	assert(e->type == FS_ENT_FILE);

	if (e->u.file.size == FS_SIZE_UNKNOWN) {
		n = e->node;
		assert(n != NULL);
		n = resolve_node(n->u.common.data);
		e->u.file.size = 0;

		while (n != NULL) {
			assert(n->type == FNODE_BLOCK);

			e->u.file.size += n->u.block.size;
			n = resolve_node(n->u.block.next);
		}
	}

	return e->u.file.size;
}

qword fs_read(byte * buf, qword addr, qword size, fs_entry_t * e)
{
	qword max, rem, a, s;
	fnode_t * n;
	fmeta_header_t * hdr;

	if (e->type != FS_ENT_FILE) return 0;
	if (e->u.file.size <= addr) return 0;

	max = fs_size(e) - addr;
	size = max < size ? max : size;
	if (!size) return 0;

	if (e->flags & FS_ENT_HOOKED) {
		if (e->u.file.read_hook == NULL) return 0;
		size = e->u.file.read_hook(buf, addr, size, e->u.file.data);
	} else {
		if (e->flags & FS_ENT_PRESENT) {
			memcpy(buf, e->u.file.data + addr, size);
		} else {
			assert(e->node != NULL);

			rem = size;
			n = resolve_node(e->node->u.common.data);
			hdr = (fmeta_header_t *)rfsmb;

			while (rem && n != NULL) {
				assert(n->type == FNODE_BLOCK);

				if (addr >= n->u.block.size) {
					addr -= n->u.block.size;
				} else {
					a = n->u.block.lba * hdr->page_size + addr;
					addr = 0;

					if (n->u.block.size > rem) {
						s = rem;
						rem = 0;
					} else {
						s = n->u.block.size;
						rem -= s;
					}

					do_assert(dev_read(root_dev, buf, a, s) == s);
					n = resolve_node(n->u.block.next);
					buf += s;
				}
			}

			size -= rem;
		}
	}

	return size;
}

qword fs_write(byte * buf, qword addr, qword size, fs_entry_t * e)
{
	if (e->type != FS_ENT_FILE) return 0;

	if (e->flags & FS_ENT_HOOKED) {
		if (e->u.file.write_hook == NULL) return 0;
		return e->u.file.write_hook(buf, addr, size, e->u.file.data);
	} else {
		if (e->flags & FS_ENT_VIRTUAL) {
			if (addr + size > e->u.file.size) {
				/* TODO possibly expand file */
				assert(0);
			} else {
				memcpy(e->u.file.data + addr, buf, size);
			}
		} else {
			/* TODO write to regular file */
			assert(0);
		}
	}

	return 0;
}
