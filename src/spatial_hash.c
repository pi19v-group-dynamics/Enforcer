#include "spatial_hash.h"
#include "memalloc.h"
#include <string.h>
#include <stdbool.h>
#include "utils.h"

#define SPATIAL_HASH_CELL_SIZE     (64)
#define SPATIAL_HASH_CELL_CAPACITY ((ECS_CELL_SIZE * ECS_CELL_SIZE) >> 3)

typedef struct spatial_hash_entity
{
	void* udata;
	int x, y, w, h;
}
spatial_hash_entity_t;

typedef struct spatial_hash_cell
{
	int fullness;
	index_t content[SPATIAL_HASH_CELL_SIZE];
}
spatial_hash_cell_t;

struct spatial_hash
{
	int width, height; /* (in cells) */
	index_buffer_t* index_buffer;
	unsigned capacity;
	spatial_hash_entity_t* entities;
	spatial_hash_cell_t cells[];
};

/******************************************************************************
 * Spatial hash create / destroy
 *****************************************************************************/

spatial_hash_t* spatial_hash_make(int width, int height)
{
	width /= SPATIAL_HASH_CELL_SIZE;
	height /= SPATIAL_HASH_CELL_SIZE;
	spatial_hash_t* hash = calloc(1, sizeof(spatial_hash_t) + sizeof(spatial_hash_cell_t[width][height]));
	hash->width = width;
	hash->height = height;
	hash->index_buffer = index_buffer_make();
	hash->capacity = 0;
	hash->entities = NULL;
	return hash;
}

__attribute__((always_inline)) inline void spatial_hash_free(spatial_hash_t* hash)
{
	index_buffer_free(hash->index_buffer);
	free(hash);
}

/******************************************************************************
 * Spatial hash update 
 *****************************************************************************/

__attribute__((always_inline)) inline void spatial_hash_clear(spatial_hash_t* hash)
{
	memset(hash->cells, 0, sizeof(spatial_hash_cell_t[hash->width][hash->height]));
	index_buffer_reset(hash->index_buffer);
}

static inline void remove_from_cells(spatial_hash_t* hash, index_t id)
{
	spatial_hash_entity_t* ent = hash->entities + id;
	#pragma omp parallel for collapse(2) schedule(nonmonotonic:dynamic)
	for (int y = ent->y; y <= ent->y + ent->h; ++y)
	{
		for (int x = ent->x; x <= ent->x + ent->w; ++x)
		{
			spatial_hash_cell_t* cell = hash->cells + x + y * hash->width;
			for (int i = 0; i < cell->fullness; ++i)
			{
				if (cell->content[i] == id)
				{
					memcpy(cell->content + i, cell->content + i + 1, cell->fullness - i - 1);
					break;
				}
			} 
		}
	}
}

static inline void add_to_cells(spatial_hash_t* hash, index_t id)
{
	spatial_hash_entity_t* ent = hash->entities + id;
	#pragma omp parallel for collapse(2) schedule(nonmonotonic:dynamic)
	for (int y = ent->y; y <= ent->y + ent->h; ++y)
	{
		for (int x = ent->x; x <= ent->x + ent->w; ++x)
		{
			spatial_hash_cell_t* cell = hash->cells + x + y * hash->width;
			assert(cell->fullness >= SPATIAL_HASH_CELL_SIZE, "Cell overflow!");
			cell->content[cell->fullness++] = id;
		}
	}
}

index_t spatial_hash_add(spatial_hash_t* hash, void* udata, int x, int y, int w, int h)
{
	index_t id = index_buffer_get(hash->index_buffer);
	if (hash->capacity >= id)
	{
		hash->capacity = id << 1;
		hash->entities = realloc(hash->entities, sizeof(spatial_hash_entity_t[hash->capacity]));
	}
	spatial_hash_entity_t* ent = hash->entities + id;
	ent->udata = udata;
	ent->x = x / SPATIAL_HASH_CELL_SIZE;
	ent->y = y / SPATIAL_HASH_CELL_SIZE;
	ent->w = w / SPATIAL_HASH_CELL_SIZE;
	ent->h = h / SPATIAL_HASH_CELL_SIZE;
	return id;
}

void spatial_hash_update(spatial_hash_t* hash, index_t id, int x, int y, int w, int h)
{
	remove_from_cells(hash, id);
	spatial_hash_entity_t* ent = hash->entities + id;
	ent->x = x / SPATIAL_HASH_CELL_SIZE;
	ent->y = y / SPATIAL_HASH_CELL_SIZE;
	ent->w = w / SPATIAL_HASH_CELL_SIZE;
	ent->h = h / SPATIAL_HASH_CELL_SIZE;
	add_to_cells(hash, id);
}

void spatial_hash_remove(spatial_hash_t* hash, index_t id)
{
	remove_from_cells(hash, id);
	index_buffer_unget(hash->index_buffer, id);
}

/******************************************************************************
 * Spatial hash update 
 *****************************************************************************/

void spatial_hash_each_into(spatial_hash_t* hash, int x, int y, int w, int h, spatial_hash_callback_t callback)
{
	x /= SPATIAL_HASH_CELL_SIZE;
	y /= SPATIAL_HASH_CELL_SIZE;
	w /= SPATIAL_HASH_CELL_SIZE;
	h /= SPATIAL_HASH_CELL_SIZE;
	#pragma omp parallel for collapse(2) schedule(nonmonotonic:dynamic)
	for (int py = y; py < y + h; ++py)
	{
		for (int px = x; px < x + w; ++px)
		{
			spatial_hash_cell_t* cell = hash->cells + x + y * hash->width;
			for (int i = 0; i < cell->fullness; ++i)
			{
				callback(hash->entities[cell->content[i]].udata);
			}
		}
	}
}

void spatial_hash_each_over(spatial_hash_t* hash, index_t id, spatial_hash_callback_t callback)
{
	spatial_hash_entity_t* ent = hash->entities + id;
	spatial_hash_each_into(hash, ent->x, ent->y, ent->w, ent->h, callback);
}

