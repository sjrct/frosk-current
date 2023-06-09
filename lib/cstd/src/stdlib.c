#include <stdlib.h>

void __attribute__((noreturn)) exit(int status) {
    // TODO
    for (;;);
}

static unsigned rand_seed = 0xAaaaAaaa;

unsigned rand(void) {
    rand_seed <<= 13;
    rand_seed >>= 17;
    rand_seed <<= 5;
    return rand_seed;
}

void srand(unsigned seed) {
    rand_seed = seed;
}
