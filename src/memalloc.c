#include <stdlib.h>
#include <stdint.h>

#ifdef MEMALLOC_LOG
#pragma message("MEMALLOC_LOG defined, due many log prints performance may be reduced!")
#endif /* MEMALLOC_LOG */

uint_fast32_t _memalloc_leaks = 0;

__attribute__((destructor(101))) static void memalloc_status(void)
{
	if (_memalloc_leaks != 0)
	{
		log_debug("Detected memory leaks: %lu!", _memalloc_leaks);
	}
	else
	{
		log_debug("Memory leaks not detected!");
	}
}

