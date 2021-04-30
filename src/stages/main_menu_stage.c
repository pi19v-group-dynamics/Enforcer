#include "stage.h"
#include "mixer.h"
#include "renderer.h"
#include "ui.h"
#include "system.h"
#include "math_ext.h"

static enum
{
	SCREEN_MENU,
	SCREEN_SETTINGS,
	SCREENS_COUNT
}
current_state = SCREEN_MENU;

static ui_context_t* ui = NULL;
static double offset = 0;
static double offsets[SCREENS_COUNT] = {0, REN_HEIGHT};

void main_menu_stage_open(void)
{
	ui = ui_make_context();
}

static void render_menu_screen(void)
{
	static int show_message_time = 0;

	ren_push();
	ren_state->translate.y += offsets[SCREEN_MENU];
	
	/* Stage label */	
	ui_label(ui, "Main menu", REN_WIDTH / 2, REN_HEIGHT / 8);

	/* Play button */
	if (ui_button(ui, __LINE__, "Start", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20, 60, 20) & UI_PRESS)
	{
		show_message_time = 300;
		stage_switch(PLAY_STAGE);
	}

	/* Go to settings menu */
	if (ui_button(ui, __LINE__, "Settings", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20 + 30, 60, 20) & UI_PRESS)
	{
		current_state = SCREEN_SETTINGS;
	}

	/* Exit from application */
	if (ui_button(ui, __LINE__, "Exit", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20 + 60, 60, 20) & UI_PRESS)
	{
		sys_exit();
	}

	if (show_message_time > 0)
	{
		ren_push();
		ren_state->color = ui->colors[UI_TEXT_COLOR];
		ren_print("Sadly, gameplay not implemented yet! Yuck!", 0, 0, &(ren_transform_t){.sx = 1, .sy = 1});
		ren_pop();
		--show_message_time;
	}

	ren_pop();
}

static void render_settings_screen(void)
{
	static float volume = 1.0f;	
	static char volume_strbuf[32];
	static bool mute = false;
	static bool fullscreen = false;
	
	ren_push();
	ren_state->translate.y += offsets[SCREEN_SETTINGS];

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
	snprintf(volume_strbuf, sizeof(volume_strbuf), "Volume: %i%%", (int)(volume * 100.0f));
	ui_label(ui, volume_strbuf, REN_WIDTH / 2, REN_HEIGHT / 2 + 16);;
	if (ui_slider(ui, __LINE__, REN_WIDTH / 2 - 32, REN_HEIGHT / 2 + 22, 64, 12, &volume) & UI_HOLD)
	{
		mix_master_gain(volume);
	}

	/* Go back */
	if (ui_button(ui, __LINE__, "Go back", REN_WIDTH / 2 - 32, 3 * REN_HEIGHT / 4 - 12, 64, 24) & UI_PRESS)
	{
		current_state = SCREEN_MENU;
	}
	
	ren_pop();
}

void main_menu_stage_update(double dt)
{
	(void) dt;
	
	offset = lerp(offset, offsets[current_state], 0.1);
	ren_state->translate.y = -offset;

	render_menu_screen();
	render_settings_screen();
}

void main_menu_stage_close(void)
{
	ui_free_context(ui);
}

