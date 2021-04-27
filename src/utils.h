#pragma once

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

#define error(...) do {       \
			log_error(__VA_ARGS__); \
			abort();                \
	} while (0)

#define confirm(cond, ...) do {      \
		if (!(cond)) error(__VA_ARGS__); \
	} while (0)

#include <stdatomic.h>

#define spinlock_lock(x) while (atomic_flag_test_and_set(x))
#define spinlock_unlock(x) atomic_flag_clear(x)

