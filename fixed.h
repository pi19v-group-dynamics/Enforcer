#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>
#include <math.h>

/* WARNING! This implementation DO NOT check overflows and wrong arguments! */

#define FIXED_BITS (0x10)            /* number of bits in fractional part (rest is integer) */
#define FIXED_UNIT (1 << FIXED_BITS) /* multiplier for conversion between floats and fixeds */
#define FIXED_MASK (FIXED_MASK - 1)  /* bit mask to take fractional part of number */

#define fixed_int(x)   ((x) >> FIXED_BITS) /* highlight the integer part of the number */
#define fixed_fract(x) ((x) & FIXED_MASK)  /* highlight the fractional part of the number */

#define fixed2float(x) ((float_t)x / FIXED_UNIT) /* convert fixed to float */
#define fixed2int(x) (x >> FIXED_BITS)           /* convert fixed to int */
#define float2fixed(x) ((x) * FIXED_UNIT)        /* convert float to fixed */
#define int2fixed(x) ((x) << FIXED_BITS)         /* convert int to fixed */

/* linear interpolate from a to b with step t, where a, b and t is fixed point numbers */
#define fixed_lerp(a, b, t) ((a) + ((((b) - (a)) * (t)) >> FIXED_BITS))

#endif /* FIXED_H */

