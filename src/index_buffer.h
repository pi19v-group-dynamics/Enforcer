#pragma once

#include <stdlib.h>
#include <stdbool.h>

typedef size_t index_t;
typedef struct index_buffer
{
	index_t capacity, index, count;
	index_t* indices;
}
index_buffer_t;

/******************************************************************************
 * Create / destroy index buffer
 *****************************************************************************/

index_buffer_t* index_buffer_make(void);
void index_buffer_free(index_buffer_t* index_buffer);

/******************************************************************************
 * Get / unget index 
 *****************************************************************************/

bool index_buffer_validate(index_buffer_t* index_buffer, index_t index);
void index_buffer_reset(index_buffer_t* index_buffer);
index_t index_buffer_get(index_buffer_t* index_buffer);
void index_buffer_unget(index_buffer_t* index_buffer, index_t index);

