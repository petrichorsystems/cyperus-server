
#include <math.h>

#include "math_modules_oscillator_sine.h"

extern float* math_modules_oscillator_sine(dsp_module_parameters_t *parameters, int samplerate) {
  float *out = malloc(sizeof(float) * dsp_global_period);
  
  float *frequency = parameters->float32_arr_type[0];
  float *amplitude = parameters->float32_arr_type[1];
  float *phase = parameters->float32_arr_type[2];
  
  float phase_delta = parameters->float32_type[0];

  for(int p=0; p<dsp_global_period; p++) {
    phase_delta += 2.0f * M_PI * frequency[p] * (1.0f/samplerate) + phase[p];
    while( phase_delta > 2.0f * M_PI )
      phase_delta -= 2 * M_PI;
    out[p] = sin(phase_delta) * amplitude[p];
  }
  parameters->float32_type[0] = phase_delta;

  
  return out;
}
