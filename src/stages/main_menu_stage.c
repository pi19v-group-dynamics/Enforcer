#include "stage.h"
#include "ui.h"
#include "event.h"
#include <string.h>
#include <stdlib.h>
#include "system.h"

static ui_context_t* ui = NULL;

void main_menu_stage_open(stage_t* stage)
{
	(void) stage;
	ui = ui_make_context();
}

void main_menu_stage_update(stage_t* stage)
{
	(void) stage;
}

void main_menu_stage_render(stage_t* stage)
{
	static int show_message_time = 0;
	(void) stage;

	if (ui_button(ui, __LINE__, "Start", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20, 60, 20))
	{
		show_message_time = 300;
	}

	if (ui_button(ui, __LINE__, "Settings", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20 + 30, 60, 20))
	{
		stage_switch(SETTINGS_STAGE);	
	}

	if (ui_button(ui, __LINE__, "Exit", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20 + 60, 60, 20))
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
}

void main_menu_stage_close(stage_t* stage)
{
	(void) stage;
	ui_free_context(ui);
}

