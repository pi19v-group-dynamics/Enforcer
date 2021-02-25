#include "platform.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <SDL2/SDL.h>

static void launch(void) ATTRIBUTE(constructor);
static void shutdown(void) ATTRIBUTE(destructor);

static void launch(void)
{
	log_stream(stderr);
	p_setup();
	log_info("app launch");
}

static void shutdown(void)
{
	p_close();
	log_debug("memory leak status: %lu leaks", _memalloc_leaks);
	log_info("app shutdown");
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt;
	while (p_process() && !p_keydown(ESCAPE))
	{
		for (int i = 0; i < P_WIDTH * P_HEIGHT * 2; ++i)
		{
			*((uint16_t*)p_pixels + i) = rand();
		}
		(void) dt;
		dt = p_step();
		p_flip();
	}

	return 0;
}

