
#ifndef MATH_MODULES_AUDIO_OSCILLATOR_SAWTOOTH_H
#define MATH_MODULES_AUDIO_OSCILLATOR_SAWTOOTH_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

extern float
math_modules_audio_oscillator_sawtooth(dsp_module_parameters_t *parameters, int samplerate, int pos);

#endif
