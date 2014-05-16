/*
 * kernel/src/fs/fs.h
 */

#ifndef FS_FS_H
#define FS_FS_H

#include <types.h>
#include <fs/node.h>
#include "../lock.h"

/* types */
#define FS_ENT_FILE      1
#define FS_ENT_DIRECTORY 2
#define FS_ENT_INDIRECT  3

/* flags */
#define FS_ENT_VIRTUAL 0x01
#define FS_ENT_PRESENT 0x02
#define FS_ENT_HASNAME 0x04
#define FS_ENT_HASDATA 0x08
#define FS_ENT_HOOKED  0x10
#define FS_ENT_STATIC  0x20

typedef struct fs_entry {
	struct fs_entry * left;
	struct fs_entry * right;
	struct fs_entry * parent; /* not the parent directory */
	char * name;
	fnode_t * node;
	uint type, flags;
	lock_t lock;

	union {
		struct {
			void * data;
			ulong size;
			qword (* read_hook) (byte *, qword, qword, void *);
			qword (* write_hook)(byte *, qword, qword, void *);
		} file;

		struct {
			struct fs_entry * root;
		} directory;

		struct {
			char * dest;
		} indirect;
	} u;
} fs_entry_t;

extern fs_entry_t * root_dir;

void setup_fs(void);

/* Finds an entry with the given name in a given parent directory. Does not
   recurse the directory tree. */
fs_entry_t * fs_resolve(const char * name, fs_entry_t * parent);

/* Retrieves the pointer to a fs_entry_t with regard to the given entry path */
fs_entry_t * fs_retrieve(const char * name, fs_entry_t * cd);

/* Gets the first entry within a given directory */
fs_entry_t * fs_first(fs_entry_t * directory);

/* Given an entry in a directory, retieves the next entry in that directory.
   Primarily for use with the fs_first function */
fs_entry_t * fs_next(fs_entry_t * last);

/* This function creates an directory with the given name in a given directory */
fs_entry_t * fs_mkdir(const char * name, fs_entry_t * parent);

/* Same as fs_mkdir, but the constructed directory is virtual */
fs_entry_t * fs_mkvdir(const char * name, fs_entry_t * parent);

/* Creates a virtual file containing the given data */
fs_entry_t * fs_enter(const char * name, void * data, ulong length, fs_entry_t * parent);

/* Gets the literial size of the file */
ulong fs_size(fs_entry_t * file);

/* Reads from a file */
qword fs_read(byte * buffer, qword addr, qword size, fs_entry_t * file);

/* Writes to a file */
qword fs_write(byte * buffer, qword addr, qword size, fs_entry_t * file);

#endif
