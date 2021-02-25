#ifndef COMMON_H
#define COMMON_H

/*
 * Common used libraries and definitions of shortucts
 * Must automatically included to each file
 */

#include <stddef.h>      /* NULL */
#include <stdbool.h>     /* bool, true, false */
#include <stdnoreturn.h> /* noreturn */
#include <stdatomic.h>   /* atomic_flag, atomic_*, ... */
#include <stdalign.h>    /* alignof, aligned_alloc */
#include <stdint.h>      /* (u)int*_t */
#include <stdlib.h>      /* abort */

#define ATTRIBUTE(...) __attribute__((__VA_ARGS__))

#ifdef DEBUG
#define assert(cond, ...) do { \
		if (!(cond)) \
		{ \
			log_error("'%s' assertion failed!", #cond); \
			abort(); \
		} \
	} while (0)
#else
#define assert(cond, ...)
#endif /* DEBUG */

#define fatal(...) do { \
		log_fatal(__VA_ARGS__); \
		abort(); \
	} while (0)

#include "memalloc.h"
#include "log.h"

#endif /* COMMON_H */

