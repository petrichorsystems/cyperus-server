
#ifndef MATH_MODULES_FILTER_MOOGFF_H
#define MATH_MODULES_FILTER_MOOGFF_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#ifdef __SSE__
#    include <xmmintrin.h>
#endif

#ifdef __SSE4_1__
#    include <smmintrin.h>
#endif

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"

extern float
math_modules_filter_moogff(dsp_parameter *filter, int samplerate, int pos);

#endif
