
#include <math.h>

#include "math_modules_oscillator_sine.h"

extern
float math_modules_oscillator_sine(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float frequency = parameters->float32_type[0];
  float amplitude = parameters->float32_type[1];
  float phase = parameters->float32_type[2];
  
  float phase_delta = parameters->float32_type[3];
  
  phase_delta += 2.0f * M_PI * frequency * (1.0f/samplerate) + phase;
  while( phase_delta > 2.0f * M_PI )
    phase_delta -= 2 * M_PI;

  parameters->float32_type[3] = phase_delta;

  return sin(phase_delta) * amplitude;
}
