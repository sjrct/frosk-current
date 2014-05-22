#include <frosk.h>

int main() {
	file_t inp;
	file_t f = fs_aquire("/dev/tvb", 0);

	for (;;) {
		inp = fs_aquire("/dev/kb", 0);
		if (inp && fs_size(inp) > 6) {
			fs_write((byte *)"asdfkasdffasdfas", 0, 10, f);
		}
		fs_release(inp);
	}
	return 0;
}
