#include "stage.h"
#include <stdatomic.h>
#include "memalloc.h"
#include "utils.h"

static atomic_flag lock = ATOMIC_FLAG_INIT;
static stage_t stages[STAGES_COUNT];
static stage_t* current_stage = NULL;

#define concat(a, b) a##b
#define create_stage(name, prefix) do { \
		extern void concat(prefix, _stage_open)(stage_t*); \
		extern void concat(prefix, _stage_update)(stage_t*); \
		extern void concat(prefix, _stage_render)(stage_t*); \
		extern void concat(prefix, _stage_close)(stage_t*); \
		stages[name].on_open   = concat(prefix, _stage_open); \
		stages[name].on_update = concat(prefix, _stage_update); \
		stages[name].on_render = concat(prefix, _stage_render); \
		stages[name].on_close  = concat(prefix, _stage_close); \
	} while (0)

__attribute__((constructor)) static void stage_setup(void)
{
	log_debug("Creating stages...");
	create_stage(INTRO_STAGE, intro);
	create_stage(MAIN_MENU_STAGE, main_menu);
	create_stage(SETTINGS_STAGE, settings);
	/* Put 'create_stage(STAGE_IDENTIFIER, stage_prefix)' here */
	log_debug("Stages created successfully!");
}

#undef create_stage
#undef concat

__attribute__((destructor)) static void stage_cleanup(void)
{
	if (current_stage) current_stage->on_close(current_stage);
	log_debug("Stages closed!");
}

__attribute__((always_inline)) inline void stage_update(double dt)
{
	assert(current_stage, "No one stage isn't open!");
	current_stage->dt = dt;
	current_stage->on_update(current_stage);
}

__attribute__((always_inline)) inline void stage_render(void)
{
	assert(current_stage, "No one stage isn't open!");
	current_stage->on_render(current_stage);
}

void stage_switch(int id)
{
	assert(id >= 0 && id < STAGES_COUNT, "Invalid stage identifier: %i", id);
	spinlock_lock(&lock);
	if (current_stage)
	{
		current_stage->on_close(current_stage);
	}
	current_stage = stages + id;
	current_stage->on_open(current_stage);
	spinlock_unlock(&lock);
}

