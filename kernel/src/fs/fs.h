//
// kernel/src/fs/fs.h
//

#ifndef FS_FS_H
#define FS_FS_H

#include <types.h>

// types
#define FS_ENT_FILE      1
#define FS_ENT_DIRECTORY 2
#define FS_ENT_INDIRECT  3

// flags
#define FS_ENT_VIRTUAL 1
#define FS_ENT_PRESENT 2
#define FS_ENT_HASNAME 4
#define FS_ENT_HASDATA 8

typedef struct fs_entry {
	struct fs_entry * left;
	struct fs_entry * right;
	struct fs_entry * parent; // not the parent directory
	char * name;
	uint type, flags;

	union {
		struct {
			void * data;
			ulong size;
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

//
// Finds an entry with the given name in a given parent directory. Does not
//   recurse the directory tree.
//
fs_entry_t * fs_resolve(const char *, fs_entry_t *);

//
// Retrieves the pointer to a fs_entry_t with regard to the given entry path
//
fs_entry_t * fs_retrieve(const char *, fs_entry_t *);

//
// Gets the first entry within a given directory
//
fs_entry_t * fs_first(fs_entry_t * directory);

//
// Given an entry in a directory, retieves the next entry in that directory
// Primarily for use with the fs_first function
//
fs_entry_t * fs_next(fs_entry_t * last);

//
// This function creates an directory with the given name in a given directory
//
fs_entry_t * fs_mkdir(const char * name, fs_entry_t * parent);

//
// Same as fs_mkdir, but the constructed directory is virtual
//
fs_entry_t * fs_mkvdir(const char * name, fs_entry_t * parent);

//
// Creates a virtual file containing the given data
//
fs_entry_t * fs_enter(const char * name, void * data, ulong length, fs_entry_t * parent);

#endif
