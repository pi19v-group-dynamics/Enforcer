#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "ecs.h"
#include "vec.h"
#include "renderer.h"

enum
{
	STEER_COMP_ID,
	REND_COMP_ID,
	UNIT_COMP_ID,
	COMP_ID_COUNT
};

enum
{
	STEER_COMP_BIT = (1 << STEER_COMP_ID),
	REND_COMP_BIT = (1 << REND_COMP_ID)
};

/******************************************************************************
 * Steering component
 *****************************************************************************/

typedef struct steer_comp
{
	float_t mass;
	vec3_t position;
	vec3_t velocity;
	vec3_t max_force;
	vec3_t max_speed;
	float_t size;
	ecs_callback_t behavior;
}
steer_comp_t;

void steer_seek(steer_comp_t* steer, vec3_t target, float_t slowing_radius);
void steer_flee(steer_comp_t* steer, vec3_t target);
void steer_wander(steer_comp_t* steer);
void steer_evade(steer_comp_t* steer, const steer_comp_t* from);
void steer_pursuit(steer_comp_t* steer, const steer_comp_t* to);

void steer_system(ecs_world_t* world, ecs_entity_t ent);

/******************************************************************************
 * Render component
 *****************************************************************************/

#define REND_MAX_FRAMES 16
#define REND_MAX_LAYERS 32

typedef struct rend_comp
{
	enum
	{
		REND_COMP_SPRITE,
		REND_COMP_STACK,
		REND_COMP_ZMAP
	}
	type;
	enum
	{
		REND_ANIM_PLAYING,
		REND_ANIM_PAUSED,
		REND_ANIM_STOPPED
	}
	state;
	enum
	{
		REND_ANIM_TO_END,
		REND_ANIM_LOOP,
		REND_ANIM_PING_PONG
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
			ren_rect_t layers[REND_MAX_LAYERS];
		};
	}
	frames[REND_MAX_FRAMES];
}
rend_comp_t;

void rend_system(ecs_world_t* world, ecs_entity_t ent);

/******************************************************************************
 * Unit component
 *****************************************************************************/

typedef enum
{
	TEAM_NONE = 0,
	TEAM_RED,
	TEAM_GREEN,
	TEAM_BLUE
}
unit_team_t;

typedef enum
{
	WEAPON_NONE = 0,
	WEAPON_MACHINE_GUN,
	WEAPON_AUTOMATIC,
	WEAPON_RIFLE,
	WEAPON_LAUNCHER,
	WEAPON_TANK_CANNON
}
unit_weapon_t;

typedef enum 
{
	PROTECTION_NONE = 0,         /* for target */
	PROTECTION_BULLETPROOF_VEST, /* for units */
	PROTECTION_COMPOSITE_ARMOUR  /* for tanks */
}
unit_protection_t;

typedef union
{
	struct
	{
		int health;
		int view_range;
		unit_team_t team;
		unit_weapon_t weapon;
		unit_protection_t protection;
	}
	unit;

	struct
	{
		float capture_level;
		unit_team_t captured;
	}
	point;
}
unit_comp_t;

#endif /* COMPONENTS_H */

