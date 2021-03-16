#include "system.h"
#include "renderer.h"
#include "mixer.h"
#include "input.h"
#include "event.h"
#include "ecs.h"

enum
{
	PHYS_COMP_ID,
	REND_COMP_ID,
	CTRL_COMP_ID,
	COMP_ID_COUNT
};

enum
{
	PHYS_COMP_BIT = (1 << PHYS_COMP_ID),
	REND_COMP_BIT = (1 << REND_COMP_ID),
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
	ren_rect_t quad;
	const ren_bitmap_t* bitmap;
	ren_transform_t transform;
}
rend_comp_t;

typedef struct
{
	int dummy;
	/* There's empty! */
}
ctrl_comp_t;

void phys_system(ecs_world_t* world, ecs_entity_t ent)
{
	phys_comp_t* phys = ecs_get(world, ent, PHYS_COMP_ID);
	phys->pos.x += phys->vel.x;
	phys->pos.y += phys->vel.y;
	phys->vel.x *= 0.9f;
	phys->vel.y *= 0.9f;
}

void rend_system(ecs_world_t* world, ecs_entity_t ent)
{
	phys_comp_t* phys = ecs_get(world, ent, PHYS_COMP_ID);
	rend_comp_t* rend = ecs_get(world, ent, REND_COMP_ID);
	ren_blit(rend->bitmap, phys->pos.x, phys->pos.y, &rend->quad, &rend->transform);
}

void ctrl_system(ecs_world_t* world, ecs_entity_t ent)
{
	phys_comp_t* phys = ecs_get(world, ent, PHYS_COMP_ID);
	#define SPEED 1.0
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

	double dt; /* variable for storing time between two frames */
	int state; /* state of application */
	
	ren_reset();
	mix_reset();

	ren_bitmap_t* bitmap = ren_load_bitmap("bin/asset.png");

	ecs_world_t* world = ecs_make_world(COMP_ID_COUNT, sizeof(phys_comp_t), sizeof(rend_comp_t), sizeof(ctrl_comp_t));

	ecs_entity_t ent1 = ecs_create(world);
	ecs_attach(world, ent1, PHYS_COMP_BIT | REND_COMP_BIT | CTRL_COMP_BIT);
	phys_comp_t* phys = ecs_get(world, ent1, PHYS_COMP_ID);
	phys->dt = &dt;
	phys->pos.x = REN_WIDTH  >> 1;
	phys->pos.y = REN_HEIGHT >> 1;
	phys->vel.x = phys->vel.y = 0;
	rend_comp_t* rend = ecs_get(world, ent1, REND_COMP_ID);
	rend->quad = (ren_rect_t){0, 0, 16, 16};
	rend->bitmap = bitmap;
	rend->transform = (ren_transform_t){.ang = 0, .sx = 1, .sy = 1, .ox = 8, .oy = 8};
	ren_recalc_transform(&rend->transform, &rend->quad);

	while ((state = sys_step(1.0 / 60.0, &dt)) != SYS_CLOSED)
	{
		/* Update */
		ecs_process(world, phys_system, PHYS_COMP_BIT);
		ecs_process(world, ctrl_system, CTRL_COMP_BIT | PHYS_COMP_BIT);
		/* Render */
		ren_fill((ren_pixel_t){.a = 0xFF, .r = 0x00, .g = 0x00, .b = 0x00});	
		ecs_process(world, rend_system, PHYS_COMP_BIT | REND_COMP_BIT);
		ren_flip();
	}

	ecs_free_world(world);
	ren_free_bitmap(bitmap);

	return 0;
}

