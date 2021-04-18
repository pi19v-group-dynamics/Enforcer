#include "fastmath.h"
#include <stdint.h>

float_t _fast_sin_lookup[FASTMATH_LUT_RANGE];
float_t _fast_cos_lookup[FASTMATH_LUT_RANGE];

#define PI2 (M_PI * 2.0)

__attribute__((constructor(102))) static void fastmath_build_lookup(void)
{
	log_debug("Building lookup tables...");	

	for (int i = 0; i < FASTMATH_LUT_RANGE; ++i)
	{
		_fast_sin_lookup[i] = sin(((float_t)i / FASTMATH_LUT_RANGE) * PI2);
		_fast_cos_lookup[i] = cos(((float_t)i / FASTMATH_LUT_RANGE) * PI2);
	}

	log_debug("Lookup tables build complete!");	
}

__attribute__((always_inline, pure)) inline float_t fastsin(float_t x)
{
	float_t i = fmod(x < 0.0 ? -x : x, PI2) * ((float_t)FASTMATH_LUT_RANGE / PI2);
	int idx = i;
	return fastlerp(_fast_sin_lookup[idx], _fast_sin_lookup[idx + 1], i - floor(i));
}

__attribute__((always_inline, pure)) inline float_t fastcos(float_t x)
{
	float_t i = fmod(x < 0.0 ? -x : x, PI2) * ((float_t)FASTMATH_LUT_RANGE / PI2);
	int idx = i;
	return fastlerp(_fast_cos_lookup[idx], _fast_cos_lookup[idx + 1], i - floor(i));
}

__attribute__((always_inline, pure)) inline float_t fasttan(float_t x)
{
	float_t i = fmod(x < 0.0 ? -x : x, PI2) * ((float_t)FASTMATH_LUT_RANGE / PI2);
	int idx = i;
	return fastlerp(_fast_sin_lookup[idx] / _fast_cos_lookup[idx],
	                _fast_sin_lookup[idx + 1] / _fast_cos_lookup[idx + 1],
	                i - floor(i));
}

