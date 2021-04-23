#include "stages.h"
#include "memalloc.h"
#include "renderer.h"
#include "event.h"
#include "stage.h"

#define prefix(a, b) a##b
#define create_stage(name) do { \
		extern void prefix(name, _stage_open)(stage_t*); \
		extern void prefix(name, _stage_update)(stage_t*); \
		extern void prefix(name, _stage_render)(stage_t*); \
		extern void prefix(name, _stage_close)(stage_t*); \
		stage_create(prefix(name, _stage_open), \
		             prefix(name, _stage_update), \
		             prefix(name, _stage_render), \
		             prefix(name, _stage_close)); \
	} while (0)

__attribute__((constructor)) static void stages_setup(void)
{
	log_debug("Creating stages...");
	/* INTRO_STAGE */
	create_stage(intro);
	log_debug("Stages created successfully!");
}

#undef create_stage
#undef prefix

