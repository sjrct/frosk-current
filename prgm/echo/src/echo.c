#include <frosk.h>
#include <stdio.h>
#include <string.h>

#define ROUGH_CLOCK_SEC ((qword)0x18000000)

void wait_clocks(qword length) {
    qword start = rdtsc();
    while (rdtsc() - start < length);
}

int main() {
    char buf[512];

    while (!feof(stdin)) {
        fgets(buf, 512, stdin);
        puts(buf);
        wait_clocks(ROUGH_CLOCK_SEC / 100);
    }
}
