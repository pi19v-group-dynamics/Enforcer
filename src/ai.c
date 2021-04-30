#include "ai.h"

/******************************************************************************
 * Apply steering
 *****************************************************************************/

void ai_update(ai_model_t* model, double dt)
{
	/* Calculate acceleration */
	vec2_t acceleration;
	vec2_truncate(acceleration, model->steering, model->max_force);
	vec2_div(acceleration, acceleration, model->mass);
	/* Update velocity */
	vec2_add(model->velocity, model->velocity, acceleration);
	vec2_truncate(model->velocity, model->velocity, model->max_speed);
	/* Update position */
	vec2_add(model->position, model->position, (vec2_t){model->velocity[0] * dt, model->velocity[1] * dt});
	/* Update angle */
	model->angle = atan2(model->velocity[1], model->velocity[0]);
}

/******************************************************************************
 * Seek, flee
 *****************************************************************************/

static void update_steering(ai_model_t* model, vec2_t desired_velocity)
{
	/* Update steering (steering += desired_velocity - velocity) */	
	vec2_add(model->steering, model->steering, desired_velocity);
	vec2_sub(model->steering, model->steering, model->velocity);
}

void ai_seek(ai_model_t* model, const vec2_t target)
{
	/* Calculate desired velocity */
	vec2_t desired_velocity;
	vec2_sub(desired_velocity, target, model->position);
	vec2_normalize(desired_velocity, desired_velocity);
	vec2_mul(desired_velocity, desired_velocity, model->max_speed);
	update_steering(model, desired_velocity);
}

void ai_flee(ai_model_t* model, const vec2_t target)
{
	/* Calculate desired velocity */
	vec2_t desired_velocity;
	vec2_sub(desired_velocity, model->position, target);
	vec2_normalize(desired_velocity, desired_velocity);
	vec2_mul(desired_velocity, desired_velocity, model->max_speed);
	/* Update steering (steering += desired_velocity - velocity) */	
	update_steering(model, desired_velocity);
}

/******************************************************************************
 * Pursiut, evade
 *****************************************************************************/

static void calculate_future(vec2_t future_position, const ai_model_t* a, const ai_model_t* b)
{
	/* Calculate future position */
	vec2_mul(future_position, b->velocity, vec2_dist(a->position, b->position) / b->max_speed);
	vec2_add(future_position, future_position, b->position);
}

void ai_pursuit(ai_model_t* model, const ai_model_t* target)
{
	vec2_t future_position;
	calculate_future(future_position, model, target);
	ai_seek(model, future_position);
}

void ai_evade(ai_model_t* model, const ai_model_t* target)
{
	vec2_t future_position;
	calculate_future(future_position, model, target);
	ai_flee(model, future_position);
}

/******************************************************************************
 * Arrival, departure
 *****************************************************************************/

void ai_arrival(ai_model_t* model, const vec2_t target, float_t radius)
{
	/* Calculate desired velocity */
	vec2_t desired_velocity;
	vec2_sub(desired_velocity, target, model->position);
	vec2_normalize(desired_velocity, desired_velocity);
	vec2_mul(desired_velocity, desired_velocity, model->max_speed);
	/* Get distance */
	float_t dist = vec2_len(desired_velocity);
	if (dist < radius)
	{
		vec2_truncate(desired_velocity, desired_velocity, radius / dist * model->max_speed);
	}
	update_steering(model, desired_velocity);
}

void ai_departure(ai_model_t* model, const vec2_t target, float_t radius)
{
	/* Calculate desired velocity */
	vec2_t desired_velocity;
	vec2_sub(desired_velocity, model->position, target);
	vec2_normalize(desired_velocity, desired_velocity);
	vec2_mul(desired_velocity, desired_velocity, model->max_speed);
	/* Get distance */
	float_t dist = vec2_len(desired_velocity);
	if (dist < radius)
	{
		vec2_truncate(desired_velocity, desired_velocity, radius / dist * model->max_speed);
	}
	update_steering(model, desired_velocity);
}

/******************************************************************************
 * Loyal
 *****************************************************************************/

void ai_wander(ai_model_t* model);
void ai_avoid(ai_model_t* model, const ai_model_t* target);

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

