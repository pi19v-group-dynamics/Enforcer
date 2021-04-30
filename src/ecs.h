#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "index_buffer.h"

typedef index_t ecs_entity_t;
typedef uint_least32_t ecs_mask_t;
typedef struct ecs_manager ecs_manager_t;
typedef void (*ecs_callback_t)(ecs_manager_t* man, ecs_entity_t ent, void* udata);

#define ECS_MAX_COMPONENTS (sizeof(ecs_mask_t))
#define ECS_INVALID_ENTITY ((ecs_entity_t)-1)

/******************************************************************************
 * World creation/destruction
 *****************************************************************************/

ecs_manager_t* ecs_make_manager(int num_components, ...);
void ecs_free_manager(ecs_manager_t* world);

/******************************************************************************
 * Entity-component control
 *****************************************************************************/

ecs_entity_t ecs_create(ecs_manager_t* man);
void ecs_destroy(ecs_manager_t* man, ecs_entity_t ent); 
void ecs_attach(ecs_manager_t* man, ecs_entity_t ent, ecs_mask_t mask);
void ecs_detach(ecs_manager_t* man, ecs_entity_t ent, ecs_mask_t mask);
bool ecs_has(ecs_manager_t* man, ecs_entity_t ent, ecs_mask_t mask);
void* ecs_get(ecs_manager_t* man, ecs_entity_t ent, int comp);

/******************************************************************************
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_manager_t* man, ecs_callback_t callback, ecs_mask_t mask, void* udata);

