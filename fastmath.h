#ifndef FASTMATH_H
#define FASTMATH_H

#include <tgmath.h>

#ifndef FASTMATH_LUT_SHIFT
#define FASTMATH_LUT_SHIFT 0xE
#endif /* FASTMATH_LUT_SHIFT */

#define FASTMATH_LUT_RANGE (1 << FASTMATH_LUT_SHIFT)
#define FASTMATH_LUT_SIZE (sizeof(float_t[FASTMATH_LUT_RANGE]))

__attribute__((always_inline, pure)) inline float_t fastsin(float_t x)
{
	return sin(x);	
}

__attribute__((always_inline, pure)) inline float_t fastcos(float_t x)
{
	return cos(x);
}

__attribute__((always_inline, pure)) inline float_t fasttan(float_t x)
{
	return tan(x);
}

__attribute__((always_inline, pure)) inline float_t fastatan2(float_t y, float_t x)
{
	return atan2(y, x);
}

#endif /* FASTMATH_H */

