#include <frosk.h>
#include <time.h>
#include <stddef.h>

time_t time(time_t * p) {
    // FIXME
    time_t t = rdtsc();
    if (p != NULL) {
        *p = t;
    }
    return t;
}
