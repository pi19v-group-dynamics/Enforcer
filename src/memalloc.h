#ifndef MEMALLOC_H
#define MEMALLOC_H

#include "log.h"

extern unsigned long _memalloc_leaks;

/*
 * Override of malloc, calloc, realloc, aligned_alloc with fail/leak check and
 * logging.
 */

static inline void* _memalloc_malloc(size_t bytes, const char* restrict file, const char* restrict func, int line) ATTRIBUTE(always_inline);
static inline void* _memalloc_calloc(size_t count, size_t bytes, const char* restrict file, const char* restrict func, int line) ATTRIBUTE(always_inline);
static inline void* _memalloc_realloc(void* ptr, size_t bytes, const char* restrict file, const char* restrict func, int line) ATTRIBUTE(always_inline);
static inline void* _memalloc_aligned_alloc(size_t alihg, size_t bytes, const char* restrict file, const char* restrict func, int line) ATTRIBUTE(always_inline);
static inline void _memalloc_free(void* ptr, const char* restrict file, const char* restrict func, int line) ATTRIBUTE(always_inline);

static inline void* _memalloc_malloc(size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	void* ptr = malloc(bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to allocate %lu byte(s)", bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif
	}
	else
	{
		++_memalloc_leaks;
#ifdef MEMALLOC_LOG
		log_debug("allocated %lu byte(s), block: %p", ptr);
#endif
	}
	return ptr;
}

static inline void* _memalloc_calloc(size_t count, size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	void* ptr = calloc(count, bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to allocate %lu elements of %lu byte(s)", count, bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif
	}
	else
	{
		++_memalloc_leaks;
#ifdef MEMALLOC_LOG
		log_debug("allocated %lu bytes, block: %p", count * bytes, ptr);
#endif
	}
	return ptr;
}

static inline void* _memalloc_realloc(void* ptr, size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	ptr = realloc(ptr, bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to reallocate block %p to length %lu", ptr, bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif
	}
#ifdef MEMALLOC_LOG
	else
	{
		log_debug("reallocated %lu bytes, block: %p", bytes, ptr);
	}
#endif
	return ptr;
}

static inline void* _memalloc_aligned_alloc(size_t alig, size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	void* ptr = aligned_alloc(alig, bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to allocate %lu byte(s)", bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif
	}
	else
	{
		++_memalloc_leaks;
#ifdef MEMALLOC_LOG
		log_debug("allocated %lu byte(s), block: %p", ptr);
#endif
	}
	return ptr;
}

static inline void _memalloc_free(void* ptr, const char* restrict file, const char* restrict func, int line)
{
#ifdef MEMALLOC_LOG
	if (ptr != NULL)
	{
		log_write(LOG_ERROR, file, func, line, "freed block %p", ptr);
	}
#else
	(void) file;
	(void) func;
	(void) line;
#endif
	free(ptr);
}

#define malloc(x) _memalloc_malloc(x, __FILE__, __func__, __LINE__)
#define calloc(n, x) _memalloc_calloc(n, x, __FILE__, __func__, __LINE__)
#define realloc(p, x) _memalloc_realloc(p, x, __FILE__, __func__, __LINE__)
#define aligned_alloc(n, x) _memalloc_alloc(n, x, __FILE__, __func__, __LINE__)
#define free(p) _memalloc_free(p, __FILE__, __func__, __LINE__)

#endif /* MEMALLOC_H */

