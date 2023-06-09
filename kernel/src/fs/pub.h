/*
 * kernel/src/fs/pub.h
 */

#ifndef FS_PUB_H
#define FS_PUB_H

#include "fs.h"

fs_entry_t * pub_fs_acquire(const char *, fs_entry_t *);
void pub_fs_release(fs_entry_t *);
fs_entry_t * pub_fs_first(fs_entry_t *);
fs_entry_t * pub_fs_next(fs_entry_t *);
fs_entry_t * pub_fs_mkdir(const char *, fs_entry_t *);
fs_entry_t * pub_fs_mkvdir(const char *, fs_entry_t *);
qword pub_fs_size(fs_entry_t *);
qword pub_fs_read(byte *, qword, qword, fs_entry_t *);
qword pub_fs_write(byte *, qword, qword, fs_entry_t *);

#endif
