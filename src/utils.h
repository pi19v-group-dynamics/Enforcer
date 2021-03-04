#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

#ifndef NDEBUG
#define assert(cond, ...) do { \
		if (!(cond))               \
		{                          \
			log_fatal(__VA_ARGS__);  \
			abort();                 \
		}                          \
	} while (0)
#else
#define assert(cond, ...)
#endif /* NDEBUG */

#define confirm(cond, ...) do { \
		if (!(cond))                \
		{                           \
			log_error(__VA_ARGS__);   \
			abort();                  \
		}                           \
	} while (0)

#endif /* UTILS_H */

