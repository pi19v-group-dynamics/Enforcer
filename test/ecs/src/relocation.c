#include "../../../src/ecs.h"
#include "../../../src/input.h"
#include "../../../src/system.h"
#include "../../../src/renderer.h"

enum
{
	PHYS_COMP_ID,
	CTRL_COMP_ID,
	COMP_ID_COUNT
};

enum
{
	PHYS_COMP_BIT = (1 << PHYS_COMP_ID),
	CTRL_COMP_BIT = (1 << CTRL_COMP_ID)
};

typedef struct
{
	double* dt;
	struct { float x, y; } pos, vel;
}
phys_comp_t;

typedef struct
{
	int dummy;
}
ctrl_comp_t;

void phys_system(ecs_world_t* world, ecs_entity_t entity)
{
	phys_comp_t* phys = ecs_get(world, entity, PHYS_COMP_ID);
	phys->pos.x += phys->vel.x;
	phys->pos.y += phys->vel.y;
	phys->vel.x *= 0.9f;
	phys->vel.y *= 0.9f;
}

void ctrl_system(ecs_world_t* world, ecs_entity_t ent)
{
	#define SPEED 1.0

	phys_comp_t* phys = ecs_get(world, ent, PHYS_COMP_ID);

	if (inp_kb_down(LEFT))
	{
		phys->vel.x = -SPEED;
	}
	if (inp_kb_down(RIGHT))
	{
		phys->vel.x = +SPEED;
	}
	if (inp_kb_down(UP))
	{
		phys->vel.y = -SPEED;
	}
	if (inp_kb_down(DOWN))
	{
		phys->vel.y = +SPEED;
	}
}

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;

	double dt;
	int state;

	phys_comp_t* phys = NULL;

	float x, y;

	ecs_world_t* world = ecs_make_world(COMP_ID_COUNT, sizeof(phys_comp_t), sizeof(ctrl_comp_t));
	ecs_entity_t entity = ecs_create(world);

	ecs_attach(world, entity, PHYS_COMP_BIT | CTRL_COMP_BIT);

	phys = ecs_get(world, entity, PHYS_COMP_ID);
	phys->dt = &dt;
	phys->pos.x = REN_WIDTH  >> 1;
	phys->pos.y = REN_HEIGHT >> 1;
	phys->vel.x = phys->vel.y = 0;

	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		if (inp_kb_down(ESCAPE))
		{
			break;
		}

		phys = ecs_get(world, entity, PHYS_COMP_ID);
		x = phys->pos.x;
		y = phys->pos.y;

		ecs_process(world, phys_system, PHYS_COMP_BIT);
		ecs_process(world, ctrl_system, CTRL_COMP_BIT | PHYS_COMP_BIT);

		phys = ecs_get(world, entity, PHYS_COMP_ID);

		log_trace("Entity changed coordinates from (%f, %f) to (%f, %f) with velocity (%f, %f)",
			x, y, phys->pos.x, phys->pos.y, phys->vel.x, phys->vel.y);
	}

	ecs_free_world(world);

	return 0;
}

