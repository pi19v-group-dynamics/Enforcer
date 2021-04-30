#pragma once

#include "ecs.h"
#include "event.h"

typedef struct stage stage_t;
typedef void (*stage_door_callback_t)(void);
typedef void (*stage_step_callback_t)(double dt);

enum
{
	MAIN_MENU_STAGE,
	PLAY_STAGE,
	/* Define new stage identifiers here */
	STAGES_COUNT
};

struct stage
{
	stage_door_callback_t on_open;
	stage_step_callback_t on_update;
	stage_door_callback_t on_close;
};

void stage_update(double dt);
void stage_switch(int id);

