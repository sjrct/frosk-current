//
// kernel/src/fs/fs.h
//

#ifndef FS_FS_H
#define FS_FS_H

#include <types.h>

#define FS_ENT_VIRTUAL  1
#define FS_FILE_PRESENT 1
#define FS_FILE_HASNAME 2
#define FS_FILE_HASDATA 4

typedef struct fs_entry {
	char * name;
	struct fs_entry * left, * right;
	uint length;
	byte type;
	byte flags;

	union {
		struct {
			char * data;
			uint size;
			int flags;
		} file;

		struct {
			struct fs_entry * root;
		} directory;

		struct {
			const char * toname;
		} indirect;
	} u;
} fs_entry_t;

fs_entry_t * fs_aquire(const char *);
void fs_enter(char *, uint, char *, uint);

#endif
