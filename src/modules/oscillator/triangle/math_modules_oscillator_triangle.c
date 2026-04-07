
#include <math.h>

#include "../../../dsp_types.h"
#include "../../../dsp.h"

#include "params_modules_oscillator_triangle.h"

extern void math_modules_oscillator_triangle(dsp_parameter *triangle, int samplerate)
{	
	float *frequency = triangle->parameters->float32_arr_type[PARAM_USER_FREQUENCY];
	float *amplitude = triangle->parameters->float32_arr_type[PARAM_USER_AMPLITUDE];
	
	int x = triangle->parameters->int32_type[PARAM_INTERNAL_X];
	
	for (int p = 0; p<dsp_global_period; p++) {
		if (x >= (int)(samplerate / frequency[p]))
			x = 0;
		triangle->out[p] = ((2 * amplitude[p]) / M_PI) * asinf( sinf( 2 * M_PI * (float)x/(samplerate / frequency[p])));
		x += 1;
	}
	triangle->parameters->int32_type[PARAM_INTERNAL_X] = x;
}
