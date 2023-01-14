
#include <math.h>

#include "math_modules_oscillator_triangle.h"

extern
float math_modules_oscillator_triangle(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float frequency = parameters->float32_type[0];
  float amplitude = parameters->float32_type[1];

  return 2 * amplitude / M_PI * asinf( sinf( (2 * M_PI / (samplerate/frequency)) * pos ) );
}
