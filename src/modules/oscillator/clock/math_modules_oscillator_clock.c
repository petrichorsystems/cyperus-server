
#include "math_modules_oscillator_clock.h"

extern float* math_modules_oscillator_clock(dsp_module_parameters_t *parameters, int samplerate)
{
	float *out = malloc(sizeof(float) * dsp_global_period);
	
	float *frequency = parameters->float32_arr_type[0];
	float *amplitude = parameters->float32_arr_type[1];

	int samples_count = parameters->int32_type[0];

	int samples_clock = (int)((float)samplerate / frequency[0]);
	float frequency_last = frequency[0];
	
	for(int p=0; p<dsp_global_period; p++) {
		if (frequency_last != frequency[p]) {
			samples_clock = (int)((float)samplerate / frequency[p]);
			frequency_last = frequency[p];
		}

		if( samples_count >= (samples_clock - 1)) {
			out[p] = 1.0f * amplitude[p];
			samples_count = 0;
		}  else {
			out[p] = 0.0f;
			samples_count += 1;
		}
	}
	parameters->int32_type[0] = samples_count;
	return out;
}
