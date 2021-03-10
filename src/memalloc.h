#ifndef MEMALLOC_H
#define MEMALLOC_H

#include <stdlib.h>
#include <stdint.h>

#define alloc_trace(bytes, ptr) \
	log_write(LOG_TRACE, file, func, line, "%s() allocated %lu byte(s), block: %p", __func__, bytes, ptr)

__attribute__((always_inline))
static inline void* _memalloc_malloc(size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	void* ptr = malloc(bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to allocate %lu byte(s)", bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif /* MEMALLOC_LOG */
	}
	else
	{
		extern uint_fast32_t _memalloc_leaks;
		++_memalloc_leaks;
#ifdef MEMALLOC_LOG
		alloc_trace(bytes, ptr);
#endif /* MEMALLOC_LOG */
	}
	return ptr;
}

__attribute__((always_inline))
static inline void* _memalloc_calloc(size_t count, size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	void* ptr = calloc(count, bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to allocate %lu elements of %lu byte(s)", count, bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif /* MEMALLOC_ABORT */
	}
	else
	{
		extern uint_fast32_t _memalloc_leaks;
		++_memalloc_leaks;
#ifdef MEMALLOC_LOG
		alloc_trace(count * bytes, ptr);
#endif /* MEMALLOC_LOG */
	}
	return ptr;
}

__attribute__((always_inline))
static inline void* _memalloc_realloc(void* ptr, size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	extern uint_fast32_t _memalloc_leaks;
	if (ptr == NULL)
	{
		ptr = realloc(ptr, bytes);
		if (ptr == NULL)
		{
			log_write(LOG_ERROR, file, func, line, "failed to allocate %lu byte(s)", bytes);
#ifdef MEMALLOC_ABORT
			abort();
#endif /* MEMALLOC_ABORT*/
		}
#ifdef MEMALLOC_LOG
		else
		{
			alloc_trace(bytes, ptr);
		}
#endif /* MEMALLOC_LOG */
		++_memalloc_leaks;
	}
	else if (bytes == 0)
	{
#ifdef MEMALLOC_LOG
		log_write(LOG_TRACE, file, func, line, "%s() freed block %p", __func__, ptr);
#else /* !defined(MEMALLOC_LOG) */
		(void) file;
		(void) func;
		(void) line;
#endif /* MEMALLOC_LOG */
		if (_memalloc_leaks == 0)
		{
			log_error("trying to free unknown memory block %p!", ptr);
			abort();
		}
		else
		{
			--_memalloc_leaks;
			ptr = realloc(ptr, bytes);
		}
	}
#ifdef MEMALLOC_LOG
	else
	{
		log_write(LOG_TRACE, file, func, line, "%s() reallocated %lu bytes, block: %p", __func__, bytes, ptr);
	}
#endif /* MEMALLOC_LOG */
	return ptr;
}

__attribute__((always_inline))
static inline void* _memalloc_aligned_alloc(size_t alig, size_t bytes, const char* restrict file, const char* restrict func, int line)
{
	void* ptr = aligned_alloc(alig, bytes);
	if (ptr == NULL)
	{
		log_write(LOG_ERROR, file, func, line, "failed to allocate %lu byte(s)", bytes);
#ifdef MEMALLOC_ABORT
		abort();
#endif /* MEMALLOC_ABORT */
	}
	else
	{
		extern uint_fast32_t _memalloc_leaks;
		++_memalloc_leaks;
#ifdef MEMALLOC_LOG
		alloc_trace(bytes, ptr);
#endif /* MEMALLOC_LOG */
	}
	return ptr;
}

__attribute__((always_inline))
static inline void _memalloc_free(void* ptr, const char* restrict file, const char* restrict func, int line)
{
	if (ptr != NULL)
	{
#ifdef MEMALLOC_LOG
		log_write(LOG_TRACE, file, func, line, "%s() freed block %p", __func__, ptr);
#else /* !defined(MEMALLOC_LOG) */
		(void) file;
		(void) func;
		(void) line;
#endif /* MEMALLOC_LOG */
		extern uint_fast32_t _memalloc_leaks;
		if (_memalloc_leaks == 0)
		{
			log_fatal("trying to free unknown memory block %p!", ptr);
			abort();
		}
		else
		{
			--_memalloc_leaks;
			free(ptr);
		}
	}
}

#undef alloc_trace

#define malloc(x) _memalloc_malloc(x, __FILE__, __func__, __LINE__)
#define calloc(n, x) _memalloc_calloc(n, x, __FILE__, __func__, __LINE__)
#define realloc(p, x) _memalloc_realloc(p, x, __FILE__, __func__, __LINE__)
#define aligned_alloc(n, x) _memalloc_aligned_alloc(n, x, __FILE__, __func__, __LINE__)
#define free(p) _memalloc_free(p, __FILE__, __func__, __LINE__)

#endif /* MEMALLOC_H */

