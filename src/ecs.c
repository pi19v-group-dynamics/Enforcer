#include "ecs.h"
#include "memalloc.h"
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "utils.h"

typedef struct ecs_cell
{
	int fullness;
	ecs_entity_t content[ECS_CELL_CAPACITY];
}
ecs_cell_t;

typedef struct ecs_hash_info
{
	int x, y, w, h;
}
ecs_hash_info_t;

struct ecs_iter
{
	ecs_cell_t* cell;
	int index;
	int cur_x, cur_y,
	    beg_x, beg_y,
	    end_x, end_y;
};

struct ecs_world
{
	/* Spatial hash part */
	int width, height; /* width and height of world in cells */
	ecs_cell_t* cells; /* cells pool */
	/* ECS part */
	int num_components;
	uintptr_t info_size, hash_offset, mask_offset;
	bool* free;
	ecs_entity_t capacity, occupied, index;
	char* infos;
	size_t offsets[];
};

/******************************************************************************
 * World creation/destruction
 *****************************************************************************/

ecs_world_t* ecs_make_world(int width, int height, int num_components, ...)
{
	va_list va;
	ecs_world_t* world = malloc(sizeof(ecs_world_t) + sizeof(size_t[num_components]));
	/* Spatial hash data */	
	world->width = width / ECS_CELL_SIZE;
	world->height = height / ECS_CELL_SIZE;
	world->cells = calloc(world->width * world->height, sizeof(ecs_cell_t));
	/* ECS data */
	world->num_components = num_components; /* number of components */
	world->capacity = 0;                    /* memory allocated */
	world->occupied = 0;                    /* memory used */
	world->index = 0;                       /* current index (candidate to next ecs_entity_t) */
	/* TODO: make 'free' to variable length bitfield */
	world->free = NULL;                     /* free indices */
	world->infos = NULL;                    /* entity info */
	world->info_size = 0;                   /* entity info size */
	/* Begin info size calculation */
	va_start(va, num_components);
	for (int i = 0; i < num_components; ++i)
	{
		world->offsets[i] = world->info_size;
		world->info_size += va_arg(va, size_t);
	}
	va_end(va);
	/* Reserve space for hash info and mask info */
	world->hash_offset = world->info_size;
	world->info_size += sizeof(ecs_hash_info_t);
	world->mask_offset = world->info_size;
	world->info_size += sizeof(ecs_mask_t);
	return world;
}

void ecs_free_world(ecs_world_t* world)
{
	free(world->infos);
	free(world->free);
	free(world->cells);
	free(world);
}

/******************************************************************************
 * Spatial hash utils
 *****************************************************************************/

static inline void remove_entity_from_cell(ecs_cell_t* cell, ecs_entity_t ent)
{
	ecs_entity_t* border = cell->content + cell->fullness;
	for (ecs_entity_t* i = cell->content; i < border; ++i)
	{
		if (*cell->content == ent)
		{
			memcpy(i, i + 1, border - i - 1);
			return;
		}
	}
}

static inline void add_entity_to_cell(ecs_cell_t* cell, ecs_entity_t ent)
{
	assert(cell->fullness < ECS_CELL_CAPACITY, "Cell overflow!");
	cell->content[cell->fullness++] = ent;
}

static inline void remove_old_position(ecs_world_t* world, ecs_entity_t ent, ecs_hash_info_t* hash_info)
{
	if (hash_info->x < 0 || hash_info->y < 0)
	{
		#pragma omp parallel for collapse(2) schedule(nonmonotonic:dynamic)
		for (int y = hash_info->y; y <= hash_info->y + hash_info->h; ++y)
		{
			for (int x = hash_info->x; x <= hash_info->x + hash_info->w; ++x)
			{
				remove_entity_from_cell(world->cells + x + y * world->width, ent);
			}
		}
	}
}

static inline void set_new_position(ecs_world_t* world, ecs_entity_t ent, ecs_hash_info_t* hash_info)
{
	if (hash_info->x < 0 || hash_info->y < 0)
	{
		#pragma omp parallel for collapse(2) schedule(nonmonotonic:dynamic)
		for (int y = hash_info->y; y <= hash_info->y + hash_info->h; ++y)
		{
			for (int x = hash_info->x; x <= hash_info->x + hash_info->w; ++x)
			{
				add_entity_to_cell(world->cells + x + y * world->width, ent);
			}
		}
	}
}

/******************************************************************************
 * Entity-component control
 *****************************************************************************/

#define get_data_info(world, ent) \
	((world)->infos + (ent) * (world)->info_size)

#define get_mask_info(world, ent) \
	(*(ecs_mask_t*)(get_data_info((world), (ent)) + (world)->mask_offset))

#define get_hash_info(world, ent) \
	((ecs_hash_info_t*)(get_data_info((world), (ent)) + (world)->hash_offset))

ecs_entity_t ecs_create(ecs_world_t* world)
{
	/* Need reallocate memory? */
	if (world->occupied >= world->capacity)
	{
		ecs_entity_t i = world->capacity;             /* old capacity */
		world->capacity = (world->capacity + 1) << 1; /* new capacity */
		/* Reallocate free indices */
		world->free = realloc(world->free, world->capacity);
		/* Reset */
		while (i < world->capacity) world->free[i++] = true;
		/* Reallocate info memory */
		world->infos = realloc(world->infos, world->capacity * world->info_size);
	}
	/* Find free index */
	while (!world->free[world->index])
	{
		world->index = (world->index + 1) % world->capacity;
	}
	++world->occupied;
	world->free[world->index] = false;
	/* Reset hash info and mask */
	get_hash_info(world, world->index)->x = -1;
	get_mask_info(world, world->index) = 0;
	return world->index;
}

void ecs_destroy(ecs_world_t* world, ecs_entity_t ent)
{
	/* Remove from spatial hash */
	remove_old_position(world, ent, get_hash_info(world, ent));
	/* Free index */
	world->free[ent] = true;
} 

void ecs_attach(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask)
{
	get_mask_info(world, ent) |= mask;
}

void ecs_detach(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask)
{
	get_mask_info(world, ent) &= ~mask;
}

bool ecs_has(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask)
{
	return (get_mask_info(world, ent) & mask) == mask;
}

void* ecs_get(ecs_world_t* world, ecs_entity_t ent, int comp)
{
	return (void*)(get_data_info(world, ent) + world->offsets[comp]);
}

/******************************************************************************
 * Spatial hash control
 *****************************************************************************/

void ecs_clear(ecs_world_t* world)
{
	/* FIXME: is efficient? */
	memset(world->cells, 0, sizeof(ecs_cell_t[world->width][world->height]));
}

void ecs_update(ecs_world_t* world, ecs_entity_t ent, int x, int y, int w, int h)
{
	ecs_hash_info_t* hash_info = get_hash_info(world, ent);
	/* Remove entity from old position */
	remove_old_position(world, ent, hash_info);
	/* Add entitiy to new positions */
	int beg_x = x / ECS_CELL_SIZE, end_x = (x + w) / ECS_CELL_SIZE,
	    beg_y = y / ECS_CELL_SIZE, end_y = (y + h) / ECS_CELL_SIZE;
	if (beg_x > end_x) { int tmp = beg_x; beg_x = end_x; end_x = tmp; }
	if (beg_y > end_y) { int tmp = beg_y; beg_y = end_y; end_y = tmp; }
	/* Sort */
	if (beg_x < 0) beg_x = 0;
	if (beg_y < 0) beg_y = 0;
	if (end_x >= world->width) end_x = world->width - 1;
	if (end_y >= world->height) end_y = world->height - 1;
	set_new_position(world, ent, hash_info);
}

void ecs_remove(ecs_world_t* world, ecs_entity_t ent)
{
	ecs_hash_info_t* hash_info = get_hash_info(world, ent);
	remove_old_position(world, ent, hash_info);
	hash_info->x = hash_info->y = -1;
}

void ecs_each_into(ecs_world_t* world, ecs_iter_t* iter, int x, int y, int w, int h)
{
	/* Shrink */
	iter->beg_x = x / ECS_CELL_SIZE, iter->end_x = (x + w) / ECS_CELL_SIZE;
	iter->beg_y = y / ECS_CELL_SIZE, iter->end_y = (y + h) / ECS_CELL_SIZE;
	/* Sort */
	if (iter->beg_x > iter->end_x)
	{
		int tmp = iter->beg_x;
		iter->beg_x = iter->end_x;
		iter->end_x = tmp;
	}
	if (iter->beg_y > iter->end_y)
	{
		int tmp = iter->beg_y;
		iter->beg_y = iter->end_y;
		iter->end_y = tmp;
	}
	/* Clip */
	if (iter->beg_x < 0) iter->beg_x = 0;
	if (iter->beg_y < 0) iter->beg_y = 0;
	if (iter->end_x >= world->width) iter->end_x = world->width - 1;
	if (iter->end_y >= world->height) iter->end_y = world->height - 1;
	/* Prepare */
	iter->cur_x = iter->beg_x;
	iter->cur_y = iter->beg_y;
	iter->cell = world->cells + iter->cur_x + (iter->cur_y * world->width);
	iter->index = 0;
}

void ecs_each_overlaps(ecs_world_t* world, ecs_iter_t* iter, ecs_entity_t ent)
{
	ecs_hash_info_t* hash_info = get_hash_info(world, ent);
	ecs_each_into(world, iter, hash_info->x, hash_info->y, hash_info->w, hash_info->h);
}

ecs_entity_t ecs_next(ecs_world_t* world, ecs_iter_t* iter)
{
begin:
	/* Current cell isn't end? */
	if (iter->index < iter->cell->fullness)
	{
		return iter->cell->content[iter->index++];
	}
	iter->index = 0;
	/* Go to next cell */
	if (iter->cur_x < iter->end_x)
	{
		++iter->cur_x;
		iter->cell = world->cells + iter->cur_x + iter->cur_y * world->width;
	}
	else if (iter->cur_y < iter->end_y)
	{
		iter->cur_x = iter->beg_x;
		++iter->cur_y;
		iter->cell = world->cells + iter->cur_x + iter->cur_y * world->width;
	}
	else
	{
		return ECS_INVALID_ENTITY;
	}
	goto begin;
}

/******************************************************************************
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_world_t* world, ecs_callback_t cb, ecs_mask_t mask)
{
	for (ecs_entity_t i = 0; i < world->occupied; ++i)
	{
		if (!world->free[i] && ecs_has(world, i, mask))
		{
			cb(world, i);
		}
	}
}

