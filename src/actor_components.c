#include "actor_components.h"

/******************************************************************************
 * Steering component 
 *****************************************************************************/

void actor_steer_static(ai_model_t* model)
{
	(void) model;
}

void actor_steer_system(ecs_manager_t* man, ecs_entity_t ent, void* udata)
{
	actor_steer_component_t* steer = ecs_get(man, ent, ACTOR_STEER_COMPONENT_ID);
	steer->behavior(&steer->model);
	ai_update(&steer->model, *(double*)udata);
}

/******************************************************************************
 * Unit component
 *****************************************************************************/

void actor_render_system(ecs_manager_t* man, ecs_entity_t ent, void* udata)
{
	(void) udata;
	const actor_steer_component_t* steer = ecs_get(man, ent, ACTOR_STEER_COMPONENT_ID);
	actor_render_component_t* render = ecs_get(man, ent, ACTOR_RENDER_COMPONENT_ID);
	const actor_render_frame_t* frame = render->frames + (int)render->cur_frame;	
	render->transform.ang += 0.01;
	ren_recalc(&render->transform, &(ren_rect_t){.x = 0, .y = 0, .w = 16, .h = 16});
	for (int i = 0; i < frame->num_layers; ++i)
	{
		const ren_rect_t* layer = frame->layers + i;
		for (int z = i * render->transform_sz; z < i * render->transform_sz + render->transform_sz + 1; ++z)
		{
			ren_blit(render->bitmap,
				steer->model.position[0],
				steer->model.position[1] - z,
				layer,
				&render->transform);
		}
	}
}

