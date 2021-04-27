#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "stage.h"
#include "input.h"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt; /* variable for storing time between two frames */
	int state; /* state of application */
	
	ren_reset();
	mix_reset();

	ren_bitmap_t* bmp = ren_load_bitmap("bin/font.png");
	ren_font_t* font = ren_make_font(bmp, 3, 5);
	ren_state->font = font;

	stage_switch(MAIN_MENU_STAGE);

	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED && !inp_kb_down(ESCAPE))
	{
		stage_update(dt);
		ren_fill((ren_pixel_t){.raw = 0xFF000000});
		stage_render();
		ren_flip();
	}

	ren_free_font(font);
	ren_free_bitmap(bmp);

	return 0;
}

