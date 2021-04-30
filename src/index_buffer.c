#include "index_buffer.h"
#include "memalloc.h"
#include <string.h>
#include "utils.h"

/******************************************************************************
 * Create / destroy index buffer
 *****************************************************************************/

__attribute__((always_inline)) inline index_buffer_t* index_buffer_make(void)
{
	return calloc(1, sizeof(index_buffer_t));
}

__attribute__((always_inline)) inline void index_buffer_free(index_buffer_t* index_buffer)
{
	free(index_buffer->indices);
	free(index_buffer);
}

/******************************************************************************
 * Get / unget index 
 *****************************************************************************/

#define BITS_PER_WORD (sizeof(index_t) << 3)

bool index_buffer_validate(index_buffer_t* index_buffer, index_t index)
{
	index_t word_offset = index / BITS_PER_WORD;
	index_t bit_offset  = index - word_offset * BITS_PER_WORD;
	return index_buffer->indices[word_offset] & (1 << bit_offset);	
}

__attribute__((always_inline)) inline void index_buffer_reset(index_buffer_t* index_buffer)
{
	index_buffer->count = 0;
	index_buffer->index = 0;
	memset(index_buffer->indices, 0, sizeof(index_t[index_buffer->capacity]));
}

index_t index_buffer_get(index_buffer_t* index_buffer)
{
	/* Need memory reallocation? */
	if ((index_buffer->count >> 3) >= index_buffer->capacity)
	{
		index_t old_capacity = index_buffer->capacity;
		index_buffer->capacity = (index_buffer->capacity + 1) << 1;
		index_buffer->indices = realloc(index_buffer->indices, sizeof(index_t[index_buffer->capacity]));
		memset(index_buffer->indices + old_capacity, 0, sizeof(index_t[index_buffer->capacity - old_capacity]));
	}
	/* Find not full block */
	do
	{
		index_buffer->index = (index_buffer->index + 1) % index_buffer->capacity;
	}
	while (index_buffer->indices[index_buffer->index] == (index_t)-1);
	/* Get free index */
	index_t index = index_buffer->indices[index_buffer->index];
	for (index_t i = 0; i < (sizeof(index_t) << 3); ++i)
	{
		if (!(index & (1 << i)))
		{
			index_buffer->indices[index] |= (1 << i);
			index += i;
			break;
		}
	}
	/* Increment index count */
	++index_buffer->count;
	return index;
}

void index_buffer_unget(index_buffer_t* index_buffer, index_t index)
{
	assert(index < (sizeof(index_t[index_buffer->capacity]) << 3), "Trying to unget an index that is not part of this buffer");
	index_t word_offset = index / BITS_PER_WORD;
	index_t bit_offset  = index - word_offset * BITS_PER_WORD;
	index_buffer->indices[word_offset] &= ~(1 << bit_offset);	
}

