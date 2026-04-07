
#include <math.h>

#include "../../../dsp_types.h"
#include "../../../dsp.h"

#include "params_modules_oscillator_sine.h"

extern void math_modules_oscillator_sine(dsp_parameter *sine, int samplerate)
{
  
  float *frequency = sine->parameters->float32_arr_type[PARAM_USER_FREQUENCY];
  float *amplitude = sine->parameters->float32_arr_type[PARAM_USER_AMPLITUDE];
  float *phase = sine->parameters->float32_arr_type[PARAM_USER_PHASE];
  
  float phase_delta = sine->parameters->float32_type[PARAM_INTERNAL_PHASE_DELTA];

  for(int p=0; p<dsp_global_period; p++) {
    phase_delta += 2.0f * M_PI * frequency[p] * (1.0f/samplerate) + phase[p];
    while( phase_delta > 2.0f * M_PI )
      phase_delta -= 2 * M_PI;
    sine->out[p] = sin(phase_delta) * amplitude[p];
  }
  sine->parameters->float32_type[PARAM_INTERNAL_PHASE_DELTA] = phase_delta;
}
