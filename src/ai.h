#pragma once

#include "math_ext.h"

typedef struct ai_model
{
	vec2_t position;       /* position */
	vec2_t velocity;       /* velocity */
	vec2_t steering;       /* steering force (acceleration * mass) */
	float_t view_range;    /* range of view */
	float_t field_of_view; /* field of view */
	float_t max_speed;     /* maximal speed */
	float_t max_force;     /* maximal steering force */
	float_t angle;         /* orientation (depends from velocity) */
	float_t mass;          /* mass */
	float_t radius;        /* size (for collision detection) */
}
ai_model_t;

typedef struct ai_path
{
	int num_nodes;
	vec2_t nodes[];
}
ai_path_t;

ai_path_t* ai_make_path(int num_nodes, ...);
void ai_free_path(ai_path_t* path);

/******************************************************************************
 * Apply steering
 *****************************************************************************/

void ai_update(ai_model_t* model, double dt);

/******************************************************************************
 * Seek, flee
 *****************************************************************************/

void ai_seek(ai_model_t* model, const vec2_t target);
void ai_flee(ai_model_t* model, const vec2_t target);

/******************************************************************************
 * Pursiut, evade
 *****************************************************************************/

void ai_pursuit(ai_model_t* model, const ai_model_t* target);
void ai_evade(ai_model_t* model, const ai_model_t* target);

/******************************************************************************
 * Arrival, departure
 *****************************************************************************/

void ai_arrival(ai_model_t* model, const vec2_t target, float_t radius);
void ai_departure(ai_model_t* model, const vec2_t target, float_t radius);

/******************************************************************************
 * Loyal
 *****************************************************************************/

void ai_wander(ai_model_t* model);
void ai_avoid(ai_model_t* model, const ai_model_t* avoid);

/******************************************************************************
 * Flocking
 *****************************************************************************/

void al_separate(ai_model_t* model, ai_model_t** flock, float_t distance);
void ai_align(ai_model_t* model, ai_model_t** flock);
void ai_cohesion(ai_model_t* model, ai_model_t** flock);

/******************************************************************************
 * Path following
 *****************************************************************************/

void ai_follow_path(ai_model_t* model, const ai_path_t* path, int direction);
void ai_stick_path(ai_model_t* model, const ai_path_t* path);

