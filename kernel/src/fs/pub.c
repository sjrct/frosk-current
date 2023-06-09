/*
 * kernel/src/fs/pub.c
 */

#include <stddef.h>
#include "pub.h"

fs_entry_t * pub_fs_acquire(const char * name, fs_entry_t * cd)
{
	fs_entry_t * f;

	f = fs_retrieve(name, cd);
	if (f == NULL) return NULL;
	if (!trylock(&f->lock)) return NULL;

	/* TODO check file permissions */
	/* TODO associate lock with thread */

	return f;
}

void pub_fs_release(fs_entry_t * f)
{
	/* TODO check lock is associated with thread */
	unlock(&f->lock);
}

fs_entry_t * pub_fs_first(fs_entry_t * f)
{
	/* TODO check permissions */
	return fs_first(f);
}

fs_entry_t * pub_fs_next(fs_entry_t * f)
{
	/* TODO check permissions */
	return fs_next(f);
}

fs_entry_t * pub_fs_mkdir(const char * name, fs_entry_t * par)
{
	/* TODO check permissions */
	return fs_mkdir(name, par);
}

fs_entry_t * pub_fs_mkvdir(const char * name, fs_entry_t * par)
{
	/* TODO check permissions */
	return fs_mkvdir(name, par);
}

qword pub_fs_size(fs_entry_t * f)
{
	/* TODO check permissions */
	return fs_size(f);
}

qword pub_fs_read(byte * buf, qword addr, qword size, fs_entry_t * f)
{
	/* TODO check permissions */
	return fs_read(buf, addr, size, f);
}

qword pub_fs_write(byte * buf, qword addr, qword size, fs_entry_t * f)
{
	/* TODO check permissions */
	return fs_write(buf, addr, size, f);
}

