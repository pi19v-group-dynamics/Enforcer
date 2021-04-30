#include "stage.h"
#include "ecs.h"
#include "actor_components.h"
#include "assetloader.h"

static ecs_manager_t* manager;

void play_stage_open(void)
{
	manager = ecs_make_manager(2, sizeof(actor_steer_component_t), sizeof(actor_render_component_t));
	ecs_entity_t ent = ecs_create(manager);
	ecs_attach(manager, ent, ACTOR_STEER_COMPONENT_BIT | ACTOR_RENDER_COMPONENT_BIT);
	/* Init steer component */
	actor_steer_component_t* steer = ecs_get(manager, ent, ACTOR_STEER_COMPONENT_ID);
	steer->model.position[0] = (float_t)(REN_WIDTH / 2);
	steer->model.position[1] = (float_t)(REN_HEIGHT / 2);
	steer->model.mass = 1;
	steer->behavior = actor_steer_static;
	/* Init render component */
	actor_render_component_t* render = ecs_get(manager, ent, ACTOR_RENDER_COMPONENT_ID);
	render->bitmap = asset_spritesheet;
	render->transform.ang = 0;
	render->transform.ox = render->transform.oy = 8;
	render->transform.sx = render->transform.sy = render->transform_sz = 3;
	ren_recalc(&render->transform, &(ren_rect_t){.x = 0, .y = 0, .w = 16, .h = 16});
	render->num_frames = 1;
	actor_render_frame_t* frame = render->frames;
	frame->num_layers = 8;
	frame->layers[0] = (ren_rect_t){.x =   0, .y = 0, .w = 16, .h = 16};
	frame->layers[1] = (ren_rect_t){.x =  16, .y = 0, .w = 16, .h = 16};
	frame->layers[2] = (ren_rect_t){.x =  32, .y = 0, .w = 16, .h = 16};
	frame->layers[3] = (ren_rect_t){.x =  48, .y = 0, .w = 16, .h = 16};
	frame->layers[4] = (ren_rect_t){.x =  64, .y = 0, .w = 16, .h = 16};
	frame->layers[5] = (ren_rect_t){.x =  80, .y = 0, .w = 16, .h = 16};
	frame->layers[6] = (ren_rect_t){.x =  96, .y = 0, .w = 16, .h = 16};
	frame->layers[7] = (ren_rect_t){.x = 112, .y = 0, .w = 16, .h = 16};
}

void play_stage_update(double dt)
{
	ecs_process(manager, actor_steer_system, ACTOR_STEER_COMPONENT_BIT, &dt);
	ecs_process(manager, actor_render_system, ACTOR_STEER_COMPONENT_BIT | ACTOR_RENDER_COMPONENT_BIT, NULL);
}

void play_stage_close(void)
{
	ecs_free_manager(manager);
}

