#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "input.h"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt; /* variable for storing time between two frames */
	int state; /* state of application */
	
	ren_reset(); /* reset renderer state */
	mix_reset(); /* reset mixer state */

	/* Load font image */
	ren_buffer_t* font = ren_load_buffer("bin/font.png");
	/* Make font and set it as primary */
	ren_state.font = ren_make_font(font, 3, 5);
	/* Load test image */
	ren_buffer_t* img = ren_load_buffer("bin/asset.png");
	ren_transform_t img_tr = /* buffer transformation */
	{
		.ang = 0.0f,
		.sx = 2.0f,
		.sy = 2.0f,
		.ox = 8.0f,
		.oy = 8.0f
	};
	ren_rect_t img_rect = {0, 0, 16, 16}; /* visible part of image */

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
		/* Calculate transform of image */
		img_tr.ang = t / 4;
		img_tr.sx = 1.0f + sinf(t) * 2.0f;
		img_tr.sy = 1.0f + cosf(t) * 2.0f;
		ren_recalc_transform(&img_tr, &img_rect);		
		/* Render image */
		ren_state.blend = img_tr.sx + img_tr.sy > 3.5f ? ren_blend_alpha : ren_blend_lighten;
		ren_buffer(img, REN_WIDTH >> 1, REN_HEIGHT >> 1, &img_rect, &img_tr);
		/* Render vertical text */
		ren_state.blend = ren_blend_add;
		ren_state.color.raw = 0xFFFFAA00;
		ren_text("Stretched text!", 10, 10, 0, 1, 2, 1);
		/* Render horizontal text */
		ren_state.blend = ren_blend_alpha;
		ren_state.color.raw = 0xFFFFFFFF;
		ren_text("Pretty horizontal text!", 20, REN_HEIGHT - 10, 1, 0, 1, 1);
		/* Render lamp */
		ren_state.blend = ren_blend_add;
		ren_state.color.raw = 0xFF476020;
		ren_circ(inp_mouse_x, inp_mouse_y, 4);
		ren_circ(inp_mouse_x, inp_mouse_y, 10);
		ren_circ(inp_mouse_x, inp_mouse_y, 18);
		/* End of frame */
		ren_flip();
		t += 0.02f;
	}

	ren_free_buffer(font);
	ren_free_buffer(img);
	ren_free_font(ren_state.font);

	return 0;
}

