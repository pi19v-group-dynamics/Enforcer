#pragma once

#include "ecs.h"
#include "vec.h"
#include "renderer.h"

enum
{
	ACT_STEER_ID,
	ACT_RENDER_ID,
	ACT_UNIT_ID,
	ACT_ID_COUNT
};

enum
{
	ACT_STEER_BIT  = 1 << ACT_STEER_ID,
	ACT_RENDER_BIT = 1 << ACT_RENDER_ID,
	ACT_UNIT_BIT   = 1 << ACT_UNIT_ID,
};

/******************************************************************************
 * Steering component
 *****************************************************************************/

typedef struct act_steer
{
	float_t mass;
	vec3_t position;
	vec3_t velocity;
	vec3_t max_force;
	vec3_t max_speed;
	float_t size;
	ecs_callback_t behavior;
}
act_steer_t;

void act_steer_seek(act_steer_t* steer, vec3_t target, float_t slowing_radius);
void act_steer_flee(act_steer_t* steer, vec3_t target);
void act_steer_wander(act_steer_t* steer);
void act_steer_evade(act_steer_t* steer, const act_steer_t* from);
void act_steer_pursuit(act_steer_t* steer, const act_steer_t* to);

void act_steer_system(ecs_world_t* world, ecs_entity_t ent);

/******************************************************************************
 * Render component
 *****************************************************************************/

#define ACT_RENDER_MAX_FRAMES 16
#define ACT_RENDER_MAX_LAYERS 32

typedef struct act_render
{
	enum
	{
		ACT_RENDER_SPRITE,
		ACT_RENDER_STACK,
		ACT_RENDER_ZMAP
	}
	type;
	enum
	{
		ACT_RENDER_PLAYING,
		ACT_RENDER_PAUSED,
		ACT_RENDER_STOPPED
	}
	state;
	enum
	{
		ACT_RENDER_TO_END,
		ACT_RENDER_LOOP,
		ACT_RENDER_PING_PONG
	}
	anim;
	int cur_frame, rate;
	int fst_frame, lst_frame;
	const ren_bitmap_t bitmap;
	ren_transform_t transform;
	union
	{
		ren_rect_t rect;
		struct
		{
			int num_layers;
			ren_rect_t layers[ACT_RENDER_MAX_LAYERS];
		};
	}
	frames[ACT_RENDER_MAX_FRAMES];
}
act_render_t;

void act_render_system(ecs_world_t* world, ecs_entity_t ent);

/******************************************************************************
 * Unit component
 *****************************************************************************/

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

