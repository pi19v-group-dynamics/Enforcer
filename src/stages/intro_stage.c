#include "stage.h"
#include "memalloc.h"
#include "renderer.h"
#include "event.h"

void intro_stage_open(stage_t* stage)
{
	(void) stage;
}

void intro_stage_update(stage_t* stage)
{
	(void) stage;
}

void intro_stage_render(stage_t* stage)
{
	(void) stage;
	ren_push();
	ren_state->color = (ren_pixel_t){.a = 0xFF, .r = 0xAA, .g = 0xFF, .b = 0xFF};
	ren_print("It's intro stage!", 0, 0, &(ren_transform_t){.ang = 0, .sx = 1, .sy = 1, .ox = 0, .oy = 0});
	ren_pop();
}

void intro_stage_close(stage_t* stage)
{
	(void) stage;
}


