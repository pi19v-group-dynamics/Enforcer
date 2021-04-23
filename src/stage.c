#include "stage.h"
#include <stdatomic.h>
#include "memalloc.h"
#include "utils.h"

static atomic_flag lock = ATOMIC_FLAG_INIT;
static stage_t* stages = NULL;
static int current_stage = -1;
static int num_stages = 0;

void stage_create(stage_callback_t on_open,
                  stage_callback_t on_update,
                  stage_callback_t on_render,
                  stage_callback_t on_close)
{
	stages = realloc(stages, sizeof(stage_t[num_stages + 1]));
	stage_t* data = stages + num_stages;
	data->camera = (camera_t)
	{
		.pos = {0, 0, 0},
		.zoom = 1,
		.angle = 0
	};
	data->on_open = on_open;
	data->on_update = on_update;
	data->on_render = on_render;
	data->on_close = on_close;
	++num_stages;
}

__attribute__((destructor)) static void stage_free(void)
{
	if (current_stage != -1)
	{
		stage_t* stage = stages + current_stage;
		stage->on_close(stage);
	}
	free(stages);
}

void stage_update(double dt)
{
	stage_t* std = stages + current_stage;
	std->dt = dt;
	std->on_update(std);
}

void stage_render(void)
{
	stage_t* std = stages + current_stage;
	std->on_render(std);
}

void stage_switch(int id)
{
	stage_t* std;
	assert(id >= 0 && id < num_stages, "Invalid stage identifier: %i", id);
	spinlock_lock(&lock);
	if (current_stage != -1)
	{
		std = stages + current_stage;
		std->on_close(std);
	}
	current_stage = id;
	std = stages + current_stage;
	std->on_open(std);
	spinlock_unlock(&lock);
}

