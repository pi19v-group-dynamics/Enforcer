#include "stage.h"
#include "ui.h"
#include "event.h"
#include <string.h>
#include <stdlib.h>
#include "system.h"

static ui_context_t* ui = NULL;
static double offset_x, offset_y;

void main_menu_stage_open(stage_t* stage)
{
	(void) stage;
	ui = ui_make_context();
	ren_push();
	offset_x = -REN_WIDTH;
	offset_y = REN_HEIGHT;
}

void main_menu_stage_update(stage_t* stage)
{
	(void) stage;
	offset_x = fastlerp(offset_x, 0, 0.1);
	offset_y = fastlerp(offset_y, 0, 0.1);
	ren_state->translate.x = offset_x;
	ren_state->translate.y = offset_y;
}

void main_menu_stage_render(stage_t* stage)
{
	static int show_message_time = 0;
	(void) stage;
	/* Stage label */	
	ui_label(ui, "Main menu", REN_WIDTH / 2, REN_HEIGHT / 8);
	/* Play button */
	if (ui_button(ui, __LINE__, "Start", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20, 60, 20) & UI_PRESS)
	{
		show_message_time = 300;
		log_trace("pressed");
	}
	/* Go to settings menu */
	if (ui_button(ui, __LINE__, "Settings", REN_WIDTH / 2 - 30, REN_HEIGHT / 2 - 20 + 30, 60, 20) & UI_PRESS)
	{
		stage_switch(SETTINGS_STAGE);	
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
}

void main_menu_stage_close(stage_t* stage)
{
	(void) stage;
	ui_free_context(ui);
	ren_pop();
}

