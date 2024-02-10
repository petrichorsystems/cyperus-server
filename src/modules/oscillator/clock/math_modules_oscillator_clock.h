
#ifndef MATH_MODULES_OSCILLATOR_CLOCK_H
#define MATH_MODULES_OSCILLATOR_CLOCK_H

#include <float.h>
#include <string.h>
#include <stddef.h>

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"

extern float*
math_modules_oscillator_clock(dsp_module_parameters_t *parameters, int samplerate);

#endif
