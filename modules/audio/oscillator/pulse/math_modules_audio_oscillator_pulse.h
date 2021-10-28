
#ifndef math_modules_dsp_filter_varslope_lowpass_h_
#define math_modules_dsp_filter_varslope_lowpass_h_

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
