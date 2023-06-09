#include <frosk.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define ROUGH_CLOCK_SEC ((qword)0x18000000)

void wait_clocks(qword length) {
    qword start = rdtsc();
    while (rdtsc() - start < length);
}

int main() {
    char buf[512];
    const char *prompt = "prompt";

    wait_clocks(ROUGH_CLOCK_SEC);

    while (1) {
        printf("%s> ", prompt);
        fgets(buf, 512, stdin);
        puts(buf);
        wait_clocks(ROUGH_CLOCK_SEC);
    }

    //printf("what what %Dabc\n", 1234);
    //wait_clocks(ROUGH_CLOCK_SEC);
    //printf("what what %Xabc\n", 1234);
    //wait_clocks(ROUGH_CLOCK_SEC);
    //printf("what what %Xabc\n", 1234);
    //wait_clocks(ROUGH_CLOCK_SEC);
    //printf("what what %g\n", 123.456);
    //wait_clocks(ROUGH_CLOCK_SEC);
    for(;;);
}
