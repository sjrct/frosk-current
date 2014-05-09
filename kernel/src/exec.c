/*
 * exec.c
 */

#include <string.h>
#include "exec.h"
#include "memory/kernel.h"

#pragma pack(push, 1)
typedef struct {
	uint mark;
	uint version;
	ulong code_size;
	ulong bss_size;
	ulong entry;
} fbe_hdr;
#pragma pack(pop)

#define FBE_MARK 0xFBE0FBE0
#define SHEBANG  "#!"

process_t * bexec(const byte * buf, ulong size, int argc, const char ** argv)
{
	fbe_hdr * hdr = (fbe_hdr *)buf;

	if (size > 4 && *(dword*)buf == FBE_MARK) {
		return schedule(
			buf + sizeof(fbe_hdr),
			hdr->code_size,
			hdr->bss_size,
			hdr->entry,
			argc,
			argv
		);
	}

	return NULL;
}

process_t * fexec(const char * fn, int argc, const char ** argv, fs_entry_t * cd)
{
	int i, nargc;
	const char ** nargv;
	ulong size;
	byte * buf;
	fs_entry_t * file = fs_retrieve(fn, cd);
	process_t * p = NULL;

	if (file != NULL) {
		size = fs_size(file);

		if (size > 1) {
			buf = kalloc(size);
			fs_read(buf, 0, size, file);

			if (size > 2 && !memcmp(buf, SHEBANG, sizeof(SHEBANG)-1)) {
				nargc = argc + 1;
				nargv = kalloc(sizeof(char*) * nargc);

				for (i = 2; buf[i] != '\n' && buf[i] != 0; i++);
				buf[i] = 0;
				nargv[0] = (char *)(buf + 2);

				for (i = 0; i < argc; i++) {
					nargv[i + 1] = argv[i];
				}

				p = fexec((char *)(buf + 2), nargc, nargv, cd);
				kfree(nargv);
			} else {
				p = bexec(buf, size, argc, argv);
			}

			kfree(buf);
		}
	}

	return p;
}
