#ifndef ECS_H
#define ECS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint_least32_t ecs_entity_t;
typedef uint_least64_t ecs_mask_t;
typedef struct ecs_world ecs_world_t;
typedef void (*ecs_callback_t)(ecs_world_t* world, ecs_entity_t ent);

#define ECS_MAX_COMPONENTS (sizeof(ecs_mask_t))
#define ECS_INVALID_ENTITY ((ecs_entity_t)-1)

/******************************************************************************
 * World creation/destruction
 *****************************************************************************/

ecs_world_t* ecs_make_world(int num_components, ...);
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
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_world_t* world, ecs_callback_t cb, ecs_mask_t mask);

#endif /* ECS_H */

