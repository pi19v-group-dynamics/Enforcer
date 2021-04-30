#pragma once

#include "ecs.h"
#include "math_ext.h"
#include "renderer.h"
#include "ai.h"

enum
{
	ACTOR_STEER_COMPONENT_ID,
	ACTOR_RENDER_COMPONENT_ID,
	ACTOR_UNIT_COMPONENT_ID,
	ACTOR_COMPONENTS_COUNT
};

enum
{
	ACTOR_STEER_COMPONENT_BIT  = 1 << ACTOR_STEER_COMPONENT_ID,
	ACTOR_RENDER_COMPONENT_BIT = 1 << ACTOR_RENDER_COMPONENT_ID,
	ACTOR_UNIT_COMPONENT_BIT   = 1 << ACTOR_UNIT_COMPONENT_ID
};

/******************************************************************************
 * Steering component 
 *****************************************************************************/

typedef void (*actor_steer_behavior_t)(ai_model_t* model);

typedef struct actor_steer_component
{
	ai_model_t model;
	actor_steer_behavior_t behavior;
}
actor_steer_component_t;

void actor_steer_static(ai_model_t* model);
void actor_steer_system(ecs_manager_t* man, ecs_entity_t ent, void* udata);

/******************************************************************************
 * Steering component 
 *****************************************************************************/

#define ACTOR_RENDER_MAX_LAYERS 32
#define ACTOR_RENDER_MAX_FRAMES 16

typedef struct actor_render_frame
{
	int num_layers;
	ren_rect_t layers[ACTOR_RENDER_MAX_LAYERS];
}
actor_render_frame_t;

typedef struct actor_render_component
{
	const ren_bitmap_t* bitmap;
	ren_transform_t transform;
	int transform_sz;
	float cur_frame, frame_speed;
	// TODO: do tweening
	// TODO: animation types
	int num_frames;
	actor_render_frame_t frames[ACTOR_RENDER_MAX_FRAMES];
}
actor_render_component_t;

void actor_render_system(ecs_manager_t* man, ecs_entity_t ent, void* udata);

/******************************************************************************
 * Unit component
 *****************************************************************************/

#if 0

typedef enum
{
	ACT_UNIT_TEAM_NONE = 0,
	ACT_UNIT_TEAM_RED,
	ACT_UNIT_TEAM_GREEN,
	ACT_UNIT_TEAM_BLUE
}
act_unit_team_t;

typedef enum
{
	ACT_UNIT_WEAPON_NONE = 0,
	ACT_UNIT_WEAPON_MACHINE_GUN,
	ACT_UNIT_WEAPON_AUTOMATIC,
	ACT_UNIT_WEAPON_RIFLE,
	ACT_UNIT_WEAPON_LAUNCHER,
	ACT_UNIT_WEAPON_TANK_CANNON
}
act_unit_weapon_t;

typedef enum 
{
	ACT_UNIT_PROTECTION_NONE = 0,         /* for target */
	ACT_UNIT_PROTECTION_BULLETPROOF_VEST, /* for units */
	ACT_UNIT_PROTECTION_COMPOSITE_ARMOUR  /* for tanks */
}
act_unit_protection_t;

typedef union
{
	struct
	{
		int health;
		int view_range;
		act_unit_team_t team;
		act_unit_weapon_t weapon;
		act_unit_protection_t protection;
	}
	unit;

	struct
	{
		float capture_level;
		act_unit_team_t captured;
	}
	point;
}
act_unit_comp_t;

#endif

