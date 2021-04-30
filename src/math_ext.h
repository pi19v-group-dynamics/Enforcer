#pragma once

#include <math.h>
#include <stdbool.h>

/******************************************************************************
 * Fast versions of common math functions
 *****************************************************************************/

/* Remove standard definitions */
#undef sin
#undef cos
#undef tan
#undef sqrt

float_t _fastsin(float_t x);
float_t _fastcos(float_t x);
float_t _fasttan(float_t x);
float_t _fastsqrt(float_t x);

/* Redefine with '_fast' versions */
#define sin(x)  _fastsin(x)
#define cos(x)  _fastcos(x)
#define tan(x)  _fasttan(x)
#define sqrt(x) _fastsqrt(x)

/******************************************************************************
 * Common math utils
 *****************************************************************************/

float_t lerp(float_t a, float_t b, float_t t);
float_t map(float_t val, float_t from_min, float_t from_max, float_t to_min, float_t to_max);

/******************************************************************************
 * 2D vector
 *****************************************************************************/

typedef float_t vec2_t[2];

void vec2_cpy(vec2_t dst, const vec2_t src);
void vec2_add(vec2_t dst, const vec2_t a, const vec2_t b);
void vec2_sub(vec2_t dst, const vec2_t a, const vec2_t b);
void vec2_mul(vec2_t dst, const vec2_t v, float_t n);
void vec2_div(vec2_t dst, const vec2_t v, float_t n);
float_t vec2_cross(const vec2_t a, const vec2_t b);
float_t vec2_dot(const vec2_t a, const vec2_t b);
float_t vec2_len2(const vec2_t v);
float_t vec2_len(const vec2_t v);
float_t vec2_dist(const vec2_t a, const vec2_t b);
float_t vec2_dist2(const vec2_t a, const vec2_t b);
bool vec2_eql(const vec2_t a, const vec2_t b);
void vec2_truncate(vec2_t dst, const vec2_t src, float_t max);
void vec2_normalize(vec2_t dst, const vec2_t src);
void vec2_project(vec2_t dst, const vec2_t a, const vec2_t b);
void vec2_lerp(vec2_t dst, vec2_t a, vec2_t b, float_t t);

