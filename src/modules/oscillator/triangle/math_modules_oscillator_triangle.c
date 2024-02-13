
#include <math.h>

#include "math_modules_oscillator_triangle.h"

extern float* math_modules_oscillator_triangle(dsp_module_parameters_t *parameters, int samplerate)
{
	float *out = malloc(sizeof(float) * dsp_global_period);
	
	float *frequency = parameters->float32_arr_type[0];
	float *amplitude = parameters->float32_arr_type[1];
	
	int x = parameters->int32_type[0];
	
	for (int p = 0; p<dsp_global_period; p++) {
		if (x >= samplerate) {
			x = 0;
		}
		out[p] = ((2 * amplitude[p]) / M_PI) * asinf( sinf( 2 * M_PI * (float)x/samplerate * frequency[p]) );
		x += 1;
	}
	parameters->int32_type[0] = x;
	
	return out;
}
