/*
 * kernel/src/debug.h
 */

#include "stubout.h"

#undef assert

#ifdef DEBUG
	#include "stubout.h"

	#define assert(X) \
		if (!(X)) { \
			dprintf("Assertation '%s' on line %d in %s failed.\n", #X, __LINE__, __FILE__); \
		}
	#define do_assert(X) assert(X)
#else
	#define assert(X)
	#define do_assert
#endif
