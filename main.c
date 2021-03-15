#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "input.h"
#include "event.h"

static void mouse_click_handler1(void* args, void* udata)
{
	(void) udata;
	log_trace("Handler 1 mouse clicked at x: %i, y: %i", *(int*)args, *((int*)args + 1));
}

static void mouse_click_handler2(void* args, void* udata)
{
	(void) udata;
	(void) args;
	log_trace("Handler 2 called");
}

static float t = 0.0f;

static void print_text(void)
{
	static const char str1[] = "$ vi main.c";
	static const char str2[] = "#include <stdio.h>\n\nint main(void)\n{\n\tputs(\"Hello, world!\");\n\treturn 0;\n}";
	static const char str3[] = "$ make";
	ren_state.blend = ren_blend_alpha;
	ren_state.color.raw = 0xFF00FF00;
	size_t ts = t * 4;
	if (ts == 0)
	{
	}
	else if (ts <= sizeof(str1))
	{
		ren_printf(str1, ts, 0, 0, 0, &(ren_transform_t){.sx = 1, .sy = 1});
	}
	else if (ts - sizeof(str1) <= sizeof(str2))
	{
		ren_printf(str2, ts - sizeof(str1), 0, 0, 0, &(ren_transform_t){.sx = 1, .sy = 1});
	}
	else if (ts - sizeof(str1) - sizeof(str2) <= sizeof(str3))
	{
		ren_printf(str3, ts - sizeof(str1) - sizeof(str2), 0, 0, 0, &(ren_transform_t){.sx = 1, .sy = 1});
	}
	else
	{
		ren_print("Hello, world!", 0, 0, 0, &(ren_transform_t){.sx = 1, .sy = 1});
	} 
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt; /* variable for storing time between two frames */
	int state; /* state of application */

	enum
	{
		MOUSE_CLICK = 0,
		EVENT_COUNT
	};

	event_bus_t* bus = event_make_bus(EVENT_COUNT);
	event_subscribe(bus, MOUSE_CLICK, mouse_click_handler1, NULL);
	event_subscribe(bus, MOUSE_CLICK, mouse_click_handler2, NULL);
	
	ren_reset(); /* reset renderer state */
	mix_reset(); /* reset mixer state */

	sys_vsync(true);
	sys_fullscreen(true);

	/* Load font image */
	ren_bitmap_t* font = ren_load_bitmap("bin/font.png");
	/* Make font and set it as primary */
	ren_state.font = ren_make_font(font, 3, 5);
	/* Load test image */
	ren_bitmap_t* img = ren_load_bitmap("bin/asset.png");
	ren_transform_t img_tr = /* buffer transformation */
	{
		.ang = 0.0f,
		.sx = 2.0f,
		.sy = 2.0f,
		.ox = 8.0f,
		.oy = 8.0f
	};
	ren_rect_t img_rect = {0, 0, 16, 16}; /* visible part of image */
	/* Init batch */
	ren_batch_t* bat = ren_make_batch(img, &img_tr, &(ren_rect_t){0, 0, 16, 16}, 4);
	ren_batch_add(bat, -32, 0);
	ren_batch_add(bat, +32, 0);
	ren_batch_add(bat, 0, -32);
	ren_batch_add(bat, 0, +32);

	ren_state.font->tabwidth = 2;
	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		if (inp_ms_down(LEFT))
		{
			event_notify(bus, MOUSE_CLICK, (int[2]){inp_mouse_x, inp_mouse_y});
		}
		if (inp_kb_down(ESCAPE))
		{
			break;
		}
		/* Render plasma */
		for (int y = 0; y < ren_screen->height; ++y)
		{
			for (int x = 0; x < ren_screen->width; ++x)
			{
				int px = (x - ren_screen->width / 2) % ren_screen->width;
				int py = (y - ren_screen->height / 2) % ren_screen->height;
				ren_pixel_t pix = (ren_pixel_t)
				{
					( 128.0 + (128.0 * sin(t + px * 4.0 / 36.0)) 
					+ 128.0 + (128.0 * sin(t + py * 4.0 / 37.0))
					+ 128.0 + (128.0 * sin((px + py) * 4.0 / 32.0 - t * 2))
					+ 128.0 + (128.0 * sin(sqrt((px * px + py * py) * 4.0) / 8.0 - t * 3))) / 5
				};
				ren_screen->data[x + y * ren_screen->width] = (ren_pixel_t){.raw = pix.raw | (pix.raw << 16)};
			}
		}
		/* Calculate transform of image */
		img_tr.ang = t / 4;
		img_tr.sx = 1.0f + sinf(t) * 2.0f;
		img_tr.sy = 1.0f + cosf(t) * 2.0f;
		ren_recalc_transform(&img_tr, &img_rect);		
		/* Render image */
		ren_state.blend = img_tr.sx + img_tr.sy > 3.5f ? ren_blend_alpha : ren_blend_lighten;
		ren_flush(bat, REN_WIDTH >> 1, REN_HEIGHT >> 1);
		/* Print text */
		print_text();		
		/* Render lamp */
		ren_state.blend = ren_blend_add;
		ren_state.color.raw = 0xFF476020;
		ren_circ(inp_mouse_x, inp_mouse_y, 4);
		/* End of frame */
		ren_flip();
		t += 0.02f;
	}

	ren_free_bitmap(font);
	ren_free_bitmap(img);
	ren_free_font(ren_state.font);
	ren_free_batch(bat);
	event_free_bus(bus);

	return 0;
}

