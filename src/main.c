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

	ren_buffer_t* image = ren_load_buffer("bin/asset.bmp");
	ren_transform_t tr = *REN_NULL_TRANSFORM;
	tr.ox = 8.0f;
	tr.oy = 8.0f;

	ren_state.font = ren_make_font(ren_load_buffer("bin/asset.png"), 3, 5);
	ren_state.blend = ren_blend_replace;

	float t = 0.0f;
	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		if (inp_kb_down(W))
		{
			tr.sy -= 0.1f;
		}
		else if (inp_kb_down(S))
		{
			tr.sy += 0.1f;
		}
		if (inp_kb_down(A))
		{
			tr.sx -= 0.1f;
		}
		else if (inp_kb_down(D))
		{
			tr.sx += 0.1f;
		}
		if (inp_kb_down(LEFT))
		{
			tr.ang -= 0.04f;
		}
		else if (inp_kb_down(RIGHT))
		{
			tr.ang += 0.04f;
		}
		
#if 1
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
#endif
		ren_buffer(image, REN_WIDTH >> 1, REN_HEIGHT >> 1, &(ren_rect_t){0, 0, 16, 16}, &tr);
		ren_text("Hello!", 10, 10, NULL);
		ren_flip();
		t += 0.02f;
	}

	ren_free_buffer(image);
	ren_free_buffer(ren_state.font->buffer);
	ren_free_font(ren_state.font);

	return 0;
}

