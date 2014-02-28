//
// util/fsb/src/fs.c
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

#define align(ADDR, TO) ((ADDR) - ((ADDR) % (TO)) + (TO))

fs_t fs;

meta_t * new_meta(node_t * branch)
{
	unsigned hdr_size;
	node_t * fr = malloc(sizeof(node_t));
	meta_t * m  = malloc(sizeof(meta_t));

	assert(branch == NULL || branch->location->shift < 24);
	m->lba   = fs.next_lba;
	m->subs  = 0;
	fs.next_lba += 2;

	fr->type = FNODE_FREE;
	fr->next = NULL;
	fr->prev = NULL;
	fr->location = m;
	m->first = fr;

	if (branch != NULL) {
		fr->ptr  = branch->ptr;
		m->shift = branch->location->shift + 8;
		fr->size = fs.block_size;
	} else {
		hdr_size = align(sizeof(fmeta_header_t), ENTRY_SIZE);
		m->shift = 0;
		fr->ptr  = hdr_size / ENTRY_SIZE;
		fr->size = fs.block_size - hdr_size;
	}

	fr->u.free.next = fs.free_meta;
	fr->u.free.prev = NULL;
	fs.free_meta = fr;

	return m;
}

static node_t * find_free(unsigned size)
{
	node_t * f = fs.free_meta;
	assert(f == NULL || f->type == FNODE_FREE);

	while (f != NULL && f->size < size) {
		assert(f->type == FNODE_FREE);
		f = f->u.free.next;
	}

	return f;
}

static node_t * new_nodeh(unsigned size, int nometa)
{
	node_t * new, * p;
	node_t * fnd;

	size = align(size + FNODE_ADD_SIZE, ENTRY_SIZE);
	fnd = find_free(size);

	if (fnd == NULL) {
		fputs("warning (I am not sure if this works): no free node found.", stderr);

		if (nometa) return NULL;

		// create a new meta block
		fnd = new_nodeh(sizeof(fnode_branch_t), 1);
		if (fnd == NULL) {
			assert(0);
		}

		fnd->type = FNODE_BRANCH;
		fnd->u.branch.sub = new_meta(fnd);
	}

	if (fnd->size == size) {
		// remove from free node list
		p = fnd->u.free.prev;
		if (p == NULL) fs.free_meta = fnd->u.free.next;
		else p->u.free.next = fnd->u.free.next;

		new = fnd;
	} else {
		// create new node
		new = malloc(sizeof(node_t));
		new->size = size;
		new->ptr = fnd->ptr;
		new->location = fnd->location;

		fnd->ptr += (size / ENTRY_SIZE) << fnd->location->shift;
		fnd->size -= size;

		assert(fnd->size % ENTRY_SIZE == 0);

		// add to nodes in block list
		if (fnd->prev != NULL) {
			fnd->prev->next = new;
		} else {
			fnd->location->first = new;
		}

		new->prev = fnd->prev;
		new->next = fnd;
		fnd->prev = new;
	}

	return new;
}

node_t * new_node(unsigned size)
{
	return new_nodeh(size, 0);
}

static node_t * new_common(const char * name, node_t * parent)
{
	node_t * new = new_node(sizeof(fnode_common_t));

	if (parent != NULL) {
		assert(parent->type == FNODE_DIRECTORY);
		new->u.common.perm  = parent->u.common.perm;
		new->u.common.owner = parent->u.common.owner;
		new->u.common.next  = parent->u.common.data;
		parent->u.common.data = new;
	} else {
		new->u.common.perm  = 0;
		new->u.common.owner = NULL;
		new->u.common.next  = NULL;
	}

	new->u.common.name = get_string(name);

	return new;
}

node_t * new_file(const char * name, const char * actual, node_t * parent)
{
	unsigned long size;
	FILE * tmp;
	node_t * new, * bl;

	// get the file size
	tmp = fopen(actual, "r");
	if (tmp == NULL)
		return NULL;

	fseek(tmp, 0, SEEK_END);
	size = ftell(tmp);

	fclose(tmp);

	// create the file node
	new = new_common(name, parent);
	new->type = FNODE_FILE;

	// create the block node
	bl = new_node(sizeof(fnode_block_t));
	bl->type = FNODE_BLOCK;
	bl->u.block.next = NULL;
	bl->u.block.lba  = fs.next_lba;
	bl->u.block.size = size;
	bl->u.block.filename = malloc(strlen(actual) + 1);
	strcpy(bl->u.block.filename, actual);

	new->u.common.data = bl;
	fs.next_lba += align(size, fs.block_size) / fs.block_size;

	return new;
}

node_t * new_virt(const char * name, ulong lba, ulong size, node_t * parent)
{
	node_t * new, * bl;

	// create the file node
	new = new_common(name, parent);
	new->type = FNODE_FILE;

	// create the block node
	bl = new_node(sizeof(fnode_block_t));
	bl->type = FNODE_BLOCK;
	bl->u.block.next = NULL;
	bl->u.block.lba  = lba * fs.block_size;
	bl->u.block.size = size;
	bl->u.block.filename = NULL;

	new->u.common.data = bl;

	return new;
}

node_t * new_dir(const char * name, node_t * parent)
{
	node_t * new = new_common(name, parent);
	new->type = FNODE_DIRECTORY;
	new->u.common.data = NULL;
	return new;
}

node_t * new_indir(const char * name, const char * to, node_t * parent)
{
	node_t * new = new_common(name, parent);
	new->type = FNODE_INDIRECT;
	new->u.common.data = get_string(to);
	return new;
}

node_t * get_string(const char * str)
{
	int cmp, length;
	node_t * n;
	node_t ** m = &fs.strtbl;

	if (str == NULL) return NULL;

	while (*m != NULL) {
		n = *m;
		cmp = strcmp(str, n->u.string.value);

		if (cmp < 0) {
			m = &n->u.string.left;
		} else if (cmp > 0) {
			m = &n->u.string.right;
		} else {
			return *m;
		}
	}

	length = strlen(str) + 1;
	assert(length < 2000); // Rough, might want to fix eventually?

	n = new_node(sizeof(fnode_string_t) + length);
	n->type = FNODE_STRING;
	n->u.string.left  = NULL;
	n->u.string.right = NULL;
	n->u.string.next  = NULL;
	n->u.string.size  = length;
	n->u.string.value = malloc(length);
	strcpy(n->u.string.value, str);

	*m = n;
	return n;
}

static void debug_node(FILE * f, node_t * n)
{
	if (n == NULL) return;

	fprintf(f, "node(%p)\n", n);
	fprintf(f, "\tnode.type = %x\n", n->type);
	fprintf(f, "\tnode.size = %x\n", n->size);
	fprintf(f, "\tnode.ptr  = %x\n", n->ptr);
	fprintf(f, "\tnode.loc  = %p\n", n->location);
	fprintf(f, "\tnode.next = %p\n", n->next);
	fprintf(f, "\tnode.prev = %p\n", n->prev);

	switch (n->type) {
	case FNODE_FREE:
		fprintf(f, "\tnode.free.next = %p\n", n->u.free.next);
		fprintf(f, "\tnode.free.prev = %p\n", n->u.free.prev);
		break;

	case FNODE_FILE:
	case FNODE_DIRECTORY:
	case FNODE_INDIRECT:
		fprintf(f, "\tnode.common.perm  = %x\n", n->u.common.perm);
		fprintf(f, "\tnode.common.name  = %p\n", n->u.common.name);
		fprintf(f, "\tnode.common.owner = %p\n", n->u.common.owner);
		fprintf(f, "\tnode.common.next  = %p\n", n->u.common.next);
		fprintf(f, "\tnode.common.data  = %p\n", n->u.common.data);
		break;

	case FNODE_STRING:
		fprintf(f, "\tnode.string.size  = %x\n", n->u.string.size);
		fprintf(f, "\tnode.string.next  = %p\n", n->u.string.next);
		fprintf(f, "\tnode.string.left  = %p\n", n->u.string.left);
		fprintf(f, "\tnode.string.right = %p\n", n->u.string.right);
		fprintf(f, "\tnode.string.value = '%s'\n", n->u.string.value);
		break;

	case FNODE_BLOCK:
		fprintf(f, "\tnode.block.lba  = %lx\n", n->u.block.lba);
		fprintf(f, "\tnode.block.next = %p\n",   n->u.block.next);
		fprintf(f, "\tnode.block.size = %lx\n", n->u.block.size);
		break;

	case FNODE_BRANCH:
		fprintf(f, "\tnode.branch.sub  = %p\n", n->u.branch.sub);
		break;
	}

	debug_node(f, n->next);
}

static void debug_meta(FILE * f, meta_t * m)
{
	if (m == NULL) return;

	fprintf(f, "meta(%p)\n", m);
	fprintf(f, "\tmeta.lba   = %llx\n", m->lba);
	fprintf(f, "\tmeta.base  = %x\n",   m->base);
	fprintf(f, "\tmeta.shift = %x\n",   m->shift);
	fprintf(f, "\tmeta.subs  = %x\n",   m->subs);
	fprintf(f, "\tmeta.first = %p\n",   m->first);

	debug_node(f, m->first);
}

void debug_fs(FILE * f)
{
	fprintf(f, "block_size = %x\n",   fs.block_size);
	fprintf(f, "next_lba   = %llx\n", fs.next_lba);
	fprintf(f, "first_meta = %p\n",   fs.first_meta);
	fprintf(f, "root       = %p\n",   fs.root);
	fprintf(f, "free_data  = %p\n",   fs.free_data);
	fprintf(f, "free_meta  = %p\n",   fs.free_meta);
	fprintf(f, "strtbl     = %p\n",   fs.strtbl);
	puts("");

	debug_meta(f, fs.first_meta);
}

static void seek_node(FILE * f, node_t * n, qword bl)
{
	qword loc = (n->location->lba + bl) * fs.block_size;
	loc += ((n->ptr >> n->location->shift) & 0xff) * 0x10;
	fseek(f, loc, SEEK_SET);
}

#define getptr(N) (N == NULL ? 0 : N->ptr)

static void write_meta(FILE *, meta_t *);

static void write_node(FILE * f, node_t * n)
{
	FILE * g;
	fnode_t wn;
	qword i, old;
	int bl;

	if (n == NULL) return;

	wn.type = n->type;

	if (n->next != NULL) {
		wn.next = (n->next->ptr >> n->location->shift) & 0xff;

		assert(n->next->location == n->location);
		assert(wn.next != 0);
	} else {
		wn.next = 0;
	}

	switch (n->type) {
	case FNODE_FREE:
		wn.u.free.next = getptr(n->u.free.next);
		wn.u.free.size = n->size;
		break;

	case FNODE_FILE:
	case FNODE_DIRECTORY:
	case FNODE_INDIRECT:
		wn.u.common.perm  = n->u.common.perm;
		wn.u.common.name  = getptr(n->u.common.name);
		wn.u.common.owner = getptr(n->u.common.owner);
		wn.u.common.next  = getptr(n->u.common.next);
		wn.u.common.data  = getptr(n->u.common.data);
		break;

	case FNODE_STRING:
		wn.u.string.size  = n->u.string.size;
		wn.u.string.next  = getptr(n->u.string.next);
		wn.u.string.left  = getptr(n->u.string.left);
		wn.u.string.right = getptr(n->u.string.right);
		break;

	case FNODE_BLOCK:
		wn.u.block.next = getptr(n->u.block.next);
		wn.u.block.lba  = n->u.block.lba;
		wn.u.block.size = n->u.block.size;

		g = fopen(n->u.block.filename, "rb");
		old = ftell(f);
		fseek(f, fs.block_size * n->u.block.lba, SEEK_SET);

		for (i = 0; i < n->u.block.size; i++) fputc(fgetc(g), f);
		for (; i % fs.block_size; i++) fputc(0, f);

		fseek(f, old, SEEK_SET);
		fclose(g);
		break;

	case FNODE_BRANCH:
		assert(n->u.branch.sub != NULL);
		write_meta(f, n->u.branch.sub);
		wn.u.branch.subs = n->u.branch.sub->subs;
		wn.u.branch.lba1 = n->u.branch.sub->lba;
		wn.u.branch.lba2 = n->u.branch.sub->lba + 1;
		break;
	}

	for (bl = 0; bl <= 1; bl++) {
		seek_node(f, n, bl);
		for (i = 0; i < n->size; i++) fputc(0, f);

		seek_node(f, n, bl);

		switch (n->type) {
		case FNODE_FREE:
			fwrite(&wn, 1, sizeof(fnode_free_t) + FNODE_ADD_SIZE, f);
			break;

		case FNODE_STRING:
			fwrite(&wn, 1, sizeof(fnode_string_t) + FNODE_ADD_SIZE, f);
			fwrite(n->u.string.value, 1, n->u.string.size, f);
			break;

		default:
			fwrite(&wn, 1, n->size, f);
			break;
		}
	}

	write_node(f, n->next);
}

static void write_meta(FILE * f, meta_t * m)
{
	if (m == NULL) return;
	write_node(f, m->first);
}

void write_fs(FILE * f)
{
	unsigned i;
	fmeta_header_t hdr = {
		FROSK_FS_MARK,
		fs.block_size,
		0,
		getptr(fs.root),
		getptr(fs.free_meta),
		getptr(fs.free_data),
		getptr(fs.strtbl)
	};

	fseek(f, 0, SEEK_SET);
	for (i = 0; i < fs.block_size; i++) fputc(0, f);

	write_meta(f, fs.first_meta);

	fseek(f, fs.block_size, SEEK_SET);
	fwrite((char *)&hdr, 1, sizeof(fmeta_header_t), f);

	fseek(f, fs.block_size*2, SEEK_SET);
	fwrite((char *)&hdr, 1, sizeof(fmeta_header_t), f);
}
