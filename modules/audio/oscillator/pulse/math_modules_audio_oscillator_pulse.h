
#ifndef MATH_MODULES_DSP_FILTER_VARSLOPE_LOWPASS_H
#define MATH_MODULES_DSP_FILTER_VARSLOPE_LOWPASS_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

extern void
math_modules_audio_oscillator_pulse_init(dsp_parameter *params);

extern float
math_modules_audio_oscillator_pulse(dsp_parameter *params, int samplerate, int pos);

#endif
