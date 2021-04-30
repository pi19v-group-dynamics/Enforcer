#include "math_ext.h"

/******************************************************************************
 * Fast versions of common math functions
 *****************************************************************************/

#ifndef MATH_LUT_SHIFT
#define MATH_LUT_SHIFT 0xE
#endif /* MATH_LUT_SHIFT */

#define MATH_LUT_RANGE (1 << MATH_LUT_SHIFT)
#define MATH_LUT_MASK  (MATH_LUT_RANGE - 1)

float_t _fastsin_lookup[MATH_LUT_RANGE];
float_t _fastcos_lookup[MATH_LUT_RANGE];

#define PI2 (M_PI * 2.0)

__attribute__((constructor(102))) static void fastmath_build_lookup(void)
{
	log_debug("Building lookup tables...");	
	
	#pragma omp parallel for simd schedule(static)
	for (int i = 0; i < MATH_LUT_RANGE; ++i)
	{
		_fastsin_lookup[i] = sin(((float_t)i / MATH_LUT_RANGE) * PI2);
		_fastcos_lookup[i] = cos(((float_t)i / MATH_LUT_RANGE) * PI2);
	}

	log_debug("Lookup tables build complete!");	
}

__attribute__((always_inline, pure)) inline float_t _fastsin(float_t x)
{
	float_t i = fmod(x < 0.0 ? -x : x, PI2) * ((float_t)MATH_LUT_RANGE / PI2);
	int idx = i;
	return lerp(_fastsin_lookup[idx], _fastsin_lookup[idx + 1], i - floor(i));
}

__attribute__((always_inline, pure)) inline float_t _fastcos(float_t x)
{
	float_t i = fmod(x < 0.0 ? -x : x, PI2) * ((float_t)MATH_LUT_RANGE / PI2);
	int idx = i;
	return lerp(_fastcos_lookup[idx], _fastcos_lookup[idx + 1], i - floor(i));
}

__attribute__((always_inline, pure)) inline float_t _fasttan(float_t x)
{
	float_t i = fmod(x < 0.0 ? -x : x, PI2) * ((float_t)MATH_LUT_RANGE / PI2);
	int idx = i;
	return lerp(_fastsin_lookup[idx    ] / _fastcos_lookup[idx    ],
	            _fastsin_lookup[idx + 1] / _fastcos_lookup[idx + 1],
	            i - floor(i));
}

#undef PI2

__attribute__((always_inline, pure)) inline float_t _fastsqrt(float_t x)
{
#if 0
	void* ptr = &x;
	unsigned i = (*(unsigned*)ptr + (127 << 23)) >> 1;
	return *(float*)(ptr = &i);
#endif
	return sqrtf(x);
}

/******************************************************************************
 * Common math utils
 *****************************************************************************/

__attribute__((always_inline, pure)) inline float_t lerp(float_t a, float_t b, float_t t)
{
	return a + (b - a) * t;
}

__attribute__((always_inline, pure)) inline float_t map(float_t val, float_t from_min, float_t from_max, float_t to_min, float_t to_max)
{
	return (val - from_min) / from_max * to_max + to_min;
}

/******************************************************************************
 * 3D vector
 *****************************************************************************/

static inline float_t xdiv(float_t a, float_t b)
{
	return b == 0.0 ? 0.0 : a / b;
}

__attribute__((always_inline)) inline void vec2_cpy(vec2_t dst, const vec2_t src)
{
	dst[0] = src[0];
	dst[1] = src[1];
}

__attribute__((always_inline)) inline void vec2_add(vec2_t dst, const vec2_t a, const vec2_t b)
{
	dst[0] = a[0] + b[0];
	dst[1] = a[1] + b[1];
}

__attribute__((always_inline)) inline void vec2_sub(vec2_t dst, const vec2_t a, const vec2_t b)
{
	dst[0] = a[0] - b[0];
	dst[1] = a[1] - b[1];
}

__attribute__((always_inline)) inline void vec2_mul(vec2_t dst, const vec2_t v, float_t n)
{
	dst[0] = v[0] * n;
	dst[1] = v[1] * n;
}

__attribute__((always_inline)) inline void vec2_div(vec2_t dst, const vec2_t v, float_t n)
{
	n = xdiv(1.0, n);
	dst[0] = v[0] * n;
	dst[1] = v[1] * n;
}

__attribute__((always_inline)) inline float_t vec2_cross(const vec2_t a, const vec2_t b)
{
	return a[0] * b[1] - a[1] * b[0];
}

__attribute__((always_inline)) inline float_t vec2_dot(const vec2_t a, const vec2_t b)
{
	return a[0] * b[0] + a[1] * b[1];
}

__attribute__((always_inline)) inline float_t vec2_len2(const vec2_t v)
{
	return v[0] * v[0] + v[1] * v[1];
}

__attribute__((always_inline)) inline float_t vec2_len(const vec2_t v)
{
	return sqrt(vec2_len2(v));
}

__attribute__((always_inline)) inline float_t vec2_dist(const vec2_t a, const vec2_t b)
{
	vec2_t res;
	vec2_sub(res, b, a);
	return vec2_len(res);
}

__attribute__((always_inline)) inline float_t vec2_dist2(const vec2_t a, const vec2_t b)
{
	vec2_t res;
	vec2_sub(res, b, a);
	return vec2_len2(res);
}

__attribute__((always_inline)) inline bool vec2_eql(const vec2_t a, const vec2_t b)
{
	return a[0] == b[0] && a[1] == b[1];
}

__attribute__((always_inline)) inline void vec2_truncate(vec2_t dst, const vec2_t src, float_t max)
{
	vec2_mul(dst, src, xdiv(max, vec2_len(src)));
}

__attribute__((always_inline)) inline void vec2_normalize(vec2_t dst, const vec2_t src)
{
	float_t inv_len = xdiv(1.0, vec2_len(src));
	dst[0] = src[0] * inv_len;
	dst[1] = src[1] * inv_len;
}

void vec2_project(vec2_t dst, const vec2_t a, const vec2_t b)
{
	vec2_t normalized_b = {0, 0};
	vec2_normalize(normalized_b, b);
	vec2_mul(dst, normalized_b, vec2_dot(a, b));
}

__attribute__((always_inline)) inline void vec2_lerp(vec2_t dst, vec2_t a, vec2_t b, float_t t)
{
	dst[0] = a[0] + (b[0] - a[0]) * t;
	dst[1] = a[1] + (b[1] - a[1]) * t;
}

