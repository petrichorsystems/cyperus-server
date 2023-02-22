
#include "math_modules_oscillator_clock.h"

extern
float math_modules_oscillator_clock(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float frequency = parameters->float32_type[0];
  float amplitude = parameters->float32_type[1];

  int samples_count = parameters->int32_type[0];
  int samples_clock = (int)((float)samplerate / frequency);
  
  float outsample = 0.0f;

  if( samples_count == samples_clock ) {
    outsample = 1.0f;
    samples_count = 0;
  }  else {
    samples_count += 1;
  }
  parameters->int32_type[0] = samples_count;
  return outsample;
}
