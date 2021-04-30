#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "stage.h"
#include "input.h"
#include "assetloader.h"

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt; /* variable for storing time between two frames */
	int state; /* state of application */
	
	ren_reset();
	mix_reset();

	ren_font_t* font = ren_make_font(asset_fontsheet, 3, 5);
	ren_state->font = font;

	stage_switch(MAIN_MENU_STAGE);

	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED && !inp_kb_down(ESCAPE))
	{
		ren_fill((ren_pixel_t){.raw = 0x00000000});
		stage_update(dt);
		ren_flip();
	}

	ren_free_font(font);
	

	return 0;
}

