#ifndef VEC_H
#define VEC_H

#include <math.h>
#include <stdint.h>
#include "fastmath.h"

typedef float_t vec3_t[3];

static inline void vec3_cpy(vec3_t dst, const vec3_t src)
{
	dst[0] = src[0];
	dst[1] = src[1];
	dst[2] = src[2];
}

static inline void vec3_add(vec3_t dst, const vec3_t a, const vec3_t b)
{
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
	dst[2] = a[2] + b[2];
}

static inline void vec3_sub(vec3_t dst, const vec3_t a, const vec3_t b)
{
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
	dst[2] = a[2] - b[2];
}

static inline void vec3_mul(vec3_t dst, const vec3_t v, float_t n)
{
	dst[0] = v[0] * n;
	dst[1] = v[1] * n;
	dst[2] = v[2] * n;
}

static inline void vec3_div(vec3_t dst, const vec3_t v, float_t d)
{
	float_t n = 1.0 / d;
	dst[0] = v[0] * n;
	dst[1] = v[1] * n;
	dst[2] = v[2] * n;
}

static inline void vec3_cross(vec3_t dst, const vec3_t a, const vec3_t b)
{
	vec3_t tmp;
	tmp[0] = a[1] * b[2] - a[2] * b[1];
	tmp[1] = a[2] * b[0] - a[0] * b[2];
	tmp[2] = a[0] * b[1] - a[1] * b[0];
	dst[0] = tmp[0];
	dst[1] = tmp[1];
	dst[2] = tmp[2];
}

static inline float_t vec3_dot(const vec3_t a, const vec3_t b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

static inline float_t vec3_len2(const vec3_t v)
{
	return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}

static inline float_t vec3_len(const vec3_t v)
{
	return fastsqrt(vec3_len2(v));
}

static inline float_t vec3_dist(const vec3_t a, const vec3_t b)
{
	vec3_t res;
	vec3_sub(res, b, a);
	return vec3_len(res);
}

static inline float_t vec3_dist2(const vec3_t a, const vec3_t b)
{
	vec3_t res;
	vec3_sub(res, b, a);
	return vec3_len2(res);
}

static inline bool vec3_eql(const vec3_t a, const vec3_t b)
{
	return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
}

#endif /* VEC_H */

