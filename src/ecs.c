#include "ecs.h"
#include "memalloc.h"
#include <stdarg.h>

typedef struct ecs_info
{
	ecs_component_mask_t mask;
	char* component_data;
}
ecs_info_t;

struct ecs_world
{
	int num_components;
	size_t info_size;
	ecs_entity_t* free;
	ecs_entity_t capacity, occupied, index;
	ecs_info_t* infos;
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
	world->info_size += sizeof(ecs_info_t);
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
	++world->capacity;
	return world->index;
}

#define get_info(world, ent) \
	((ecs_info_t*)((char*)world->infos + ent * world->info_size))

void ecs_attach(ecs_world_t* world, ecs_entity_t ent, int n, ...)
{
	va_list va;
	ecs_info_t* info = get_info(world, ent);
	va_start(va, n);
	while (n-- > 0)
	{
		info->mask |= (1 << va_arg(va, int));
	}
	va_end(va);
}

void ecs_detach(ecs_world_t* world, ecs_entity_t ent, int n, ...)
{
	va_list va;
	va_start(va, n);
	int mask = 0;
	while (n-- > 0)
	{
		mask |= (1 << va_arg(va, int));
	}
	get_info(world, ent)->mask &= ~mask;
	va_end(va);
}

bool ecs_has(ecs_world_t* world, ecs_entity_t ent, int n, ...)
{
	va_list va;
	va_start(va, n);
	int mask = 0;
	while (n-- > 0)
	{
		mask |= (1 << va_arg(va, int));
	}
	va_end(va);
	return get_info(world, ent)->mask & mask;
}

void* ecs_get(ecs_world_t* world, ecs_entity_t ent, int comp)
{
	return (void*)(get_info(world, ent)->component_data + world->offsets[comp]);
}

/******************************************************************************
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_world_t* world, ecs_callback_t cb, int n, ...)
{
	va_list va;
	va_start(va, n);
	unsigned mask = 0;	
	while (n-- > 0)
	{
		mask |= (1 << va_arg(va, int));
	}
	va_end(va);
	for (ecs_entity_t i = 0; i < world->occupied; ++i)
	{
		if ((get_info(world, i)->mask & mask) == mask)
		{
			cb(world, i);
		}
	}
}

