//
// util/fsb/src/main.c
//

#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

static size_t sb_offset = 0x1000;
static unsigned root_perm = 0;
static char * root_owner = NULL;
static char * inp_file = NULL;
static char * out_file = NULL;

static void print_help(void)
{
	puts(
	"fsb:\n"
	"\tBuilds a frosk file system.\n"
	"Options\n"
	"\t-h\n"
	"\t\t\nPrint this help.\n"
	"\t-i[input file]\n"
	"\t\tSpecify the input source file.\n"
	"\t-o[output file]\n"
	"\t\tSpecify the disk image to create and output to.\n"
	"\t-w[owner]\n"
	"\t\tSpecify the owner of the root directory.\n"
	"\t-p[perm]\n"
	"\t\tSpecify the permissions of the root directory.\n"
	"\t-b[block size]\n"
	"\t\tSpecify the size of blocks in bytes for the filesystem. (default = 4096)\n"
	"\t-s[offset]\n"
	"\t\tSpecify the block of the superblock for the filesystem. (default = 1)\n"
	);
}

static void init_opts(int argc, char ** argv)
{
	int i;

	for (i = 0; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'h':
				print_help();
				exit(EXIT_FAILURE);
			case 'i':
				inp_file = argv[i] + 2;
				break;
			case 'o':
				out_file = argv[i] + 2;
				break;
			case 'p':
				root_perm = parse_perm(argv[i] + 2);
				break;
			case 'w':
				root_owner = argv[i] + 2;
				break;
			default:
				fprintf(stderr, "%s: error: unknown switch '-%c'.\n", argv[0], argv[i][1]);
				exit(EXIT_FAILURE);
			}
		} else {
			fprintf(stderr, "%s: error: I don't know what to do with '%s'.\n", argv[0], argv[i]);
			exit(EXIT_FAILURE);
		}
	}
}

static char * read_file(const char * file)
{
	FILE * f = fopen(file, "r");
	if (f == NULL)
		return NULL;

	fclose(f);
}

int main(int argc, char ** argv)
{
	FILE * out;
	char * buf;
	fs_t * fs;

	init_opts(argc, argv);

	// read and parse
	buf = read_file(inp_file);
	if (buf == NULL) {
		fprintf(stderr, "%s: error: could not open file '%s'.\n", argv[0], inp_file);
		return EXIT_FAILURE;
	}

	fs = create_fs(root_perm, root_owner);
	parse_fs(buf, fs);
	free(buf);

	// write disk image
	out = fopen(out_file, "wb");
	if (out == NULL) {
		fprintf(stderr, "%s: error: could not open file '%s'.\n", argv[0], out_file);
		return EXIT_FAILURE;
	}

	write_fs(fs, out, sb_offset);
	fclose(out);

	return 0;
}
