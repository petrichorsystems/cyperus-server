
#ifndef MATH_MODULES_DSP_FILTER_BANDPASS_H
#define MATH_MODULES_DSP_FILTER_BANDPASS_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"

extern float
math_modules_filter_bandpass(dsp_parameter *filter, int samplerate, int pos);

extern float
math_modules_filter_bandpass_init(dsp_parameter *filter);

#endif
