#ifndef _ASSERT_H_
#define _ASSERT_H_

#ifdef NDEBUG
#  define assert(COND)
#else
#  include <stdlib.h>
#  define assert(COND) \
     if (!(COND)) { \
       fprintf(stderr, "Assertion %s failed %s:%d\n", #COND, __FILE__, __LINE__); \
       exit(EXIT_FAILURE); \
     }

#endif

#endif
