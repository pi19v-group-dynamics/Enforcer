#pragma once

/*
 * Entity-component-system implementation with built-in spatial hash.
 */

#include <stdint.h>
#include <stdbool.h>

typedef uint_least64_t ecs_mask_t;
typedef uint_least32_t ecs_entity_t;
typedef struct ecs_world ecs_world_t;
typedef struct ecs_iter ecs_iter_t;
typedef void (*ecs_callback_t)(ecs_world_t* world, ecs_entity_t ent);

#define ECS_CELL_SIZE      (64)
#define ECS_CELL_CAPACITY  ((ECS_CELL_SIZE * ECS_CELL_SIZE) / 8)
#define ECS_MAX_COMPONENTS (sizeof(ecs_mask_t))
#define ECS_INVALID_ENTITY ((ecs_entity_t)-1)

/******************************************************************************
 * World creation/destruction
 *****************************************************************************/

ecs_world_t* ecs_make_world(int width, int height, int num_components, ...);
void ecs_free_world(ecs_world_t* world);

/******************************************************************************
 * Entity-component control
 *****************************************************************************/

ecs_entity_t ecs_create(ecs_world_t* world);
void ecs_destroy(ecs_world_t* world, ecs_entity_t ent); 
void ecs_attach(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask);
void ecs_detach(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask);
bool ecs_has(ecs_world_t* world, ecs_entity_t ent, ecs_mask_t mask);
void* ecs_get(ecs_world_t* world, ecs_entity_t ent, int comp);

/******************************************************************************
 * Spatial hash control
 *****************************************************************************/

void ecs_clear(ecs_world_t* world);
void ecs_update(ecs_world_t* world, ecs_entity_t ent, int x, int y, int w, int h);
void ecs_remove(ecs_world_t* world, ecs_entity_t ent);
void ecs_each_into(ecs_world_t* world, ecs_iter_t* iter, int x, int y, int w, int h);
void ecs_each_overlaps(ecs_world_t* world, ecs_iter_t* iter, ecs_entity_t ent);
ecs_entity_t ecs_next(ecs_world_t* world, ecs_iter_t* iter);

/******************************************************************************
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_world_t* world, ecs_callback_t cb, ecs_mask_t mask);

