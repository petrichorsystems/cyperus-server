
#ifndef MATH_MODULES_AUDIO_OSCILLATOR_PULSE_H
#define MATH_MODULES_AUDIO_OSCILLATOR_PULSE_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

extern void
math_modules_audio_oscillator_pulse_init(dsp_module_parameters_t *parameters);

extern float
math_modules_audio_oscillator_pulse(dsp_module_parameters_t *parameters, int samplerate, int pos);

#endif
