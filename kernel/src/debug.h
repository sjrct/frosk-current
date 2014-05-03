//
// kernel/src/debug.h
//

#include "stubout.h"

#undef assert

#ifdef DEBUG
	#include "stubout.h"

	#define assert(X) \
		if (!(X)) { \
			dprintf("Assertation '" #X "' on line %d in %s failed.\n", __LINE__, __FILE__); \
		}
#else
	#define assert(X)
#endif
