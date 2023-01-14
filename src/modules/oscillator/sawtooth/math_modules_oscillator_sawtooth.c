
#include <math.h>

#include "math_modules_oscillator_sawtooth.h"

extern
float math_modules_oscillator_sawtooth(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float frequency = parameters->float32_type[0];
  float amplitude = parameters->float32_type[1];
  
  float  *sin_val, *cos_val = NULL;
  sincosf(pos * M_PI / (samplerate / frequency), sin_val, cos_val);
  return (2.0f * amplitude) / M_PI * atanf( (*cos_val / *sin_val) );
}
