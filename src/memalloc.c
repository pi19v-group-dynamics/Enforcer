#include <stdlib.h>
#include <stdint.h>

uint_fast32_t _memalloc_leaks = 0;

__attribute__((destructor(101))) static void memalloc_status(void)
{
	if (_memalloc_leaks != 0)
	{
		log_debug("Number of detected memory leaks: %lu!", _memalloc_leaks);
	}
	else
	{
		log_debug("Memory leaks not detected!");
	}
}

