//
// kernel/src/debug.h
//

#include "stubout.h"

#undef assert

#ifdef DEBUG
	#include "stubout.h"

	#define assert(X) \
		if (!(X)) { \
			dprintf("Assertation '" #X "' failed.\n"); \
		}
#else
	#define assert(X)
#endif
