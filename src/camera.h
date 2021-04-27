#pragma once

#include "vec.h"

typedef struct camera
{
	vec3_t pos;
	float_t zoom;
	float_t angle;
}
camera_t;

void camera_move(vec3_t to, float_t step);
void camera_zoom(float_t to, float_t step);
void camera_rotate(float_t to, float_t step);

