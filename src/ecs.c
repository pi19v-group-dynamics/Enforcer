#include "ecs.h"
#include "memalloc.h"
#include <stdarg.h>

struct ecs_world
{
	int num_components;
	size_t info_size;
	ecs_entity_t* free;
	ecs_entity_t capacity, occupied, index;
	char* infos;
	size_t offsets[];
};

/******************************************************************************
 * World creation/destruction
 *****************************************************************************/

ecs_world_t* ecs_make_world(int num_components, ...)
{
	va_list va;
	ecs_world_t* world = malloc(sizeof(ecs_world_t) + sizeof(size_t[num_components]));
	world->num_components = num_components;
	world->capacity = 0;
	world->occupied = 0;
	world->index = 0;
	world->free = NULL;
	world->infos = NULL;
	world->info_size = 0;
	va_start(va, num_components);
	for (int i = 0; i < num_components; ++i)
	{
		world->offsets[i] = world->info_size;
		world->info_size += va_arg(va, size_t);
	}
	world->info_size += sizeof(ecs_mask_t);
	va_end(va);
	return world;
}

void ecs_free_world(ecs_world_t* world)
{
	free(world->free);
	free(world->infos);
	free(world);
}

/******************************************************************************
 * Entity-component control
 *****************************************************************************/

ecs_entity_t ecs_create(ecs_world_t* world)
{
	/* Need reallocate memory? */
	if (world->occupied >= world->capacity)
	{
		ecs_entity_t i = world->capacity;
		world->capacity = (world->capacity + 1) << 1;
		world->free = realloc(world->free, world->capacity);
		/* Fill with free indeces */
		while (i < world->capacity)
		{
			world->free[i++] = ECS_INVALID_ENTITY;
		}
		world->infos = realloc(world->infos, world->capacity * world->info_size);
	}
	/* Find free index */
	while (world->free[world->index] != ECS_INVALID_ENTITY)
	{
		world->index = (world->index + 1) % world->capacity;
	}
	++world->occupied;
	return world->index;
}

void ecs_destroy(ecs_world_t* world, ecs_entity_t ent)
{
	world->free[ent] = ECS_INVALID_ENTITY;
} 

#define get_info_mask(world, ent) (*(ecs_mask_t*)((world)->infos + (ent) * (world)->info_size))
#define get_info_data(world, ent) ((world)->infos + (ent) * (world)->info_size + sizeof(ecs_mask_t))

void ecs_attach(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask)
{
	get_info_mask(world, ent) |= mask;
}

void ecs_detach(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask)
{
	get_info_mask(world, ent) &= ~mask;
}

bool ecs_has(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask)
{
	return (get_info_mask(world, ent) & mask) == mask;
}

void* ecs_get(ecs_world_t* world, ecs_entity_t ent, int comp)
{
	return (void*)(get_info_data(world, ent) + world->offsets[comp]);
}

#undef get_info_data
#undef get_info_mask

/******************************************************************************
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_world_t* world, ecs_callback_t cb, ecs_mask_t mask)
{
	for (ecs_entity_t i = 0; i < world->occupied; ++i)
	{
		if (world->free[i] == ECS_INVALID_ENTITY && ecs_has(world, i, mask))
		{
			cb(world, i);
		}
	}
}

