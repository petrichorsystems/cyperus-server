
#ifndef MATH_MODULES_DSP_FILTER_VARSLOPE_LOWPASS_H
#define MATH_MODULES_DSP_FILTER_VARSLOPE_LOWPASS_H

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

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

extern float
math_modules_audio_filter_moogff(dsp_parameter *filter, int samplerate, int pos);

#endif
