#pragma once

#include "ecs.h"
#include "event.h"
#include "camera.h"

typedef struct stage stage_t;
typedef void (*stage_callback_t)(stage_t* std);

enum
{
	INTRO_STAGE,
	MAIN_MENU_STAGE,
	SETTINGS_STAGE,
	/* Define new stage identifiers here */
	STAGES_COUNT
};

struct stage
{
	double dt;
	camera_t camera;
	stage_callback_t on_open;
	stage_callback_t on_update;
	stage_callback_t on_render;
	stage_callback_t on_close;
};

void stage_update(double dt);
void stage_render(void);
void stage_switch(int id);

