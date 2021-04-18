#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "input.h"
#include "event.h"
#include "ecs.h"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt; /* variable for storing time between two frames */
	int state; /* state of application */
	
	ren_reset();
	mix_reset();

	ren_state.blend = ren_blend_alpha;

	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		ren_fill((ren_pixel_t){.a = 0xFF, .r = 0x00, .g = 0x00, .b = 0x00});	
		ren_flip();
	}

	return 0;
}

