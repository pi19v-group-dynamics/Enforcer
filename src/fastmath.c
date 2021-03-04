#include "fastmath.h"
#include <stdint.h>
#include <errno.h>

float_t _fast_sin_lookup[FASTMATH_LUT_RANGE]; 
const float_t* const _fast_cos_lookup = _fast_sin_lookup + FASTMATH_LUT_RANGE;

#define TWO_PI (M_PI * 2.0)

__attribute__((constructor(102))) static void fastmath_build_lookup(void)
{
	log_debug("LUT is building...");	
	for (int i = 0; i < FASTMATH_LUT_RANGE; ++i)
	{
		_fast_sin_lookup[i] = sin(((float_t)i / FASTMATH_LUT_RANGE) * TWO_PI);
	}
	log_debug("LUT building complete!");	
}

