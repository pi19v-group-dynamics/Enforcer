#pragma once
#include "index_buffer.h"

typedef struct spatial_hash spatial_hash_t;
typedef void (*spatial_hash_callback_t)(void* udata);

/******************************************************************************
 * Spatial hash create / destroy
 *****************************************************************************/

spatial_hash_t* spatial_hash_make(int width, int height);
void spatial_hash_free(spatial_hash_t* hash);

/******************************************************************************
 * Spatial hash update 
 *****************************************************************************/

void spatial_hash_clear(spatial_hash_t* hash);
index_t spatial_hash_add(spatial_hash_t* hash, void* udata, int x, int y, int w, int h);
void spatial_hash_update(spatial_hash_t* hash, index_t id, int x, int y, int w, int h);
void spatial_hash_remove(spatial_hash_t* hash, index_t id);

/******************************************************************************
 * Spatial hash update 
 *****************************************************************************/

void spatial_hash_each_into(spatial_hash_t* hash, int x, int y, int w, int h, spatial_hash_callback_t callback);
void spatial_hash_each_over(spatial_hash_t* hash, index_t id, spatial_hash_callback_t callback);

