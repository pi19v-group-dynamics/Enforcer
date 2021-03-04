#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "input.h"
#include <string.h>

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt;
	int state;
	ren_reset();
	mix_reset();

	mix_source_t* music = mix_load_source("bin/music.wav");
	mix_loop(music, true);
	mix_play(music);

	float t = 0.0f;
	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		/* Render plasma */
		for (int y = 0; y < ren_screen->height; ++y)
		{
			for (int x = 0; x < ren_screen->width; ++x)
			{
				int px = (x - ren_screen->width / 2) % ren_screen->width;
				int py = (y - ren_screen->height / 2) % ren_screen->height;
				ren_pixel_t pix = (ren_pixel_t)
				{
					( 128.0 + (128.0 * sin(t + px * SYS_WINDOW_SCALE / 16.0)) 
					+ 128.0 + (128.0 * sin(t + py * SYS_WINDOW_SCALE / 16.0))
					+ 128.0 + (128.0 * sin((px + py) * SYS_WINDOW_SCALE / 32.0 - t * 2))
					+ 128.0 + (128.0 * sin(sqrt((px * px + py * py) * SYS_WINDOW_SCALE) / 8.0 - t * 3))) / 4
				};
				ren_screen->data[x + y * ren_screen->width] = (ren_pixel_t){.raw = pix.raw | (pix.raw << 8)};
			}
		}
		ren_flip();
		t += 0.02f;
	}

	mix_free_source(music);

	return 0;
}

