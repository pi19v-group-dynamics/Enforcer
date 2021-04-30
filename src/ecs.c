#include "ecs.h"
#include <stdarg.h>
#include <string.h>
#include "memalloc.h"
#include "utils.h"

struct ecs_manager
{
	index_buffer_t* index_buffer; /* index buffer */
	size_t info_size,             /* size of data per entity */
	       mask_offset;           /* offset from entity info to entity mask */
	size_t capacity;              /* num allocated bytes, num used blocks  */
	char* infos;                  /* raw info data */
	size_t offsets[];             /* component offsets */
};

/******************************************************************************
 * World creation/destruction
 *****************************************************************************/

ecs_manager_t* ecs_make_manager(int num_components, ...)
{
	va_list va;
	ecs_manager_t* man = malloc(sizeof(ecs_manager_t) + sizeof(size_t[num_components]));
	man->index_buffer = index_buffer_make();
	man->infos = NULL;
	man->info_size = 0;
	man->capacity = 0;
	va_start(va, num_components);
	for (int i = 0; i < num_components; ++i)
	{
		man->offsets[i] = man->info_size;
		man->info_size += va_arg(va, size_t);
	}
	va_end(va);
	man->mask_offset = man->info_size;
	man->info_size += sizeof(ecs_mask_t);
	return man;
}

void ecs_free_manager(ecs_manager_t* man)
{
	index_buffer_free(man->index_buffer);
	free(man->infos);
	free(man);
}

/******************************************************************************
 * Entity-component control
 *****************************************************************************/

#define get_data_info(man, ent) \
	((man)->infos + (ent) * (man)->info_size)

#define get_mask_info(man, ent) \
	(*(ecs_mask_t*)(get_data_info((man), (ent)) + (man)->mask_offset))

ecs_entity_t ecs_create(ecs_manager_t* man)
{
	ecs_entity_t ent = index_buffer_get(man->index_buffer);
	if (man->capacity >= ent)
	{
		man->capacity = (ent + 1) << 1;
		man->infos = realloc(man->infos, man->capacity * man->info_size);
	}
	memset(get_data_info(man, ent), 0, man->info_size);
	return ent;
}

__attribute__((always_inline)) inline void ecs_destroy(ecs_manager_t* man, ecs_entity_t ent)
{
	index_buffer_unget(man->index_buffer, ent);
} 

void ecs_attach(ecs_manager_t* man, ecs_entity_t ent, ecs_mask_t mask)
{
	get_mask_info(man, ent) |= mask;
}

void ecs_detach(ecs_manager_t* man, ecs_entity_t ent, ecs_mask_t mask)
{
	get_mask_info(man, ent) &= ~mask;
}

bool ecs_has(ecs_manager_t* man, ecs_entity_t ent, ecs_mask_t mask)
{
	return (get_mask_info(man, ent) & mask) == mask;
}

void* ecs_get(ecs_manager_t* man, ecs_entity_t ent, int comp)
{
	assert(ecs_has(man, ent, 1 << comp), "Requested component (%i) isn't attached to entity (%lu)!", comp, ent);
	return (void*)(get_data_info(man, ent) + man->offsets[comp]);
}

#undef get_mask_info
#undef get_data_info

/******************************************************************************
 * System utilities
 *****************************************************************************/

void ecs_process(ecs_manager_t* man, ecs_callback_t callback, ecs_mask_t mask, void* udata)
{
	assert(callback != NULL, "System callback cannot be NULL!");
	for (ecs_entity_t i = 0; i < man->capacity; ++i)
	{
		if (index_buffer_validate(man->index_buffer, i) && ecs_has(man, i, mask))
		{
			callback(man, i, udata);
		}
	}
}

