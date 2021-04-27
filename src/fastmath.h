#pragma once

#include <tgmath.h>

#ifndef FASTMATH_LUT_SHIFT
#define FASTMATH_LUT_SHIFT 0xE
#endif /* FASTMATH_LUT_SHIFT */

#define FASTMATH_LUT_RANGE (1 << FASTMATH_LUT_SHIFT)
#define FASTMATH_LUT_MASK  (FASTMATH_LUT_RANGE - 1)

float_t fastsin(float_t x);
float_t fastcos(float_t x);
float_t fasttan(float_t x);

__attribute__((always_inline, pure)) inline float_t fastsqrt(float_t x)
{
	void* ptr = &x;
	unsigned i = (*(unsigned*)ptr + (127 << 23)) >> 1;
	return *(float*)(ptr = &i);
}

__attribute__((always_inline, pure)) inline float_t fastlerp(float_t a, float_t b, float_t t)
{
	return a + (b - a) * t;
}

