#include "stage.h"
#include <stdio.h>
#include "fastmath.h"
#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "ui.h"

static ui_context_t* ui = NULL;
static double offset_x, offset_y;

void settings_stage_open(stage_t* stage)
{
	(void) stage;
	ui = ui_make_context();
	ren_push();
	offset_x = REN_WIDTH;
	offset_y = REN_HEIGHT;
}

void settings_stage_update(stage_t* stage)
{
	(void) stage;
	offset_x = fastlerp(offset_x, 0, 0.1);
	offset_y = fastlerp(offset_y, 0, 0.1);
	ren_state->translate.x = offset_x;
	ren_state->translate.y = offset_y;
}

void settings_stage_render(stage_t* stage)
{
	static float volume = 1.0f;	
	static char volume_strbuf[32];
	static bool mute = false;
	static bool fullscreen = false;
	(void) stage;
	/* Stage label */	
	ui_label(ui, "Settings", REN_WIDTH / 2, REN_HEIGHT / 8);
	/* Toggle fullscreen */
	ui_string(ui, "Fullscreen", REN_WIDTH / 2 - 16, REN_HEIGHT / 2 - 18);
	if (ui_checkbox(ui, __LINE__, REN_WIDTH / 2 - 32, REN_HEIGHT / 2 - 24, 12, 12, &fullscreen) & UI_PRESS)
	{
		sys_fullscreen(fullscreen);
	}
	/* Toggle mute */
	ui_string(ui, "Mute", REN_WIDTH / 2 - 16, REN_HEIGHT / 2 - 4);
	if (ui_checkbox(ui, __LINE__, REN_WIDTH / 2 - 32, REN_HEIGHT / 2 - 8, 12, 12, &mute) & UI_PRESS)
	{
		sys_mute(mute);
	}
	/* Volume slider */
	snprintf(volume_strbuf, sizeof(volume_strbuf), "Volume: %g%%", volume * 100.0);
	ui_label(ui, volume_strbuf, REN_WIDTH / 2, REN_HEIGHT / 2 + 16);;
	if (ui_slider(ui, __LINE__, REN_WIDTH / 2 - 32, REN_HEIGHT / 2 + 22, 64, 12, &volume) & UI_HOLD)
	{
		mix_master_gain(volume);
	}
	/* Go back */
	if (ui_button(ui, __LINE__, "Go back", REN_WIDTH / 2 - 32, 3 * REN_HEIGHT / 4 - 12, 64, 24) & UI_PRESS)
	{
		stage_switch(MAIN_MENU_STAGE);
	}
}

void settings_stage_close(stage_t* stage)
{
	(void) stage;
	ui_free_context(ui);
	ren_pop();
}

