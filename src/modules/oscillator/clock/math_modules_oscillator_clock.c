
#include "../../../dsp_types.h"
#include "../../../dsp.h"

#include "params_modules_oscillator_clock.h"

extern void math_modules_oscillator_clock(dsp_parameter *clock, int samplerate)
{	
	float *frequency = clock->parameters->float32_arr_type[PARAM_USER_FREQUENCY];
	float *amplitude = clock->parameters->float32_arr_type[PARAM_USER_AMPLITUDE];

	int samples_count = clock->parameters->int32_type[PARAM_INTERNAL_SAMPLES_COUNT];

	int samples_clock = (int)((float)samplerate / frequency[0]);
	float frequency_last = frequency[0];
	
	for(int p=0; p<dsp_global_period; p++) {
		if (frequency_last != frequency[p]) {
			samples_clock = (int)((float)samplerate / frequency[p]);
			frequency_last = frequency[p];
		}
		if( samples_count >= (samples_clock - 1)) {
			clock->out[p] = amplitude[p];
			samples_count = 0;
		}  else {
			clock->out[p] = 0.0f;
			samples_count += 1;
		}
	}
	clock->parameters->int32_type[PARAM_INTERNAL_SAMPLES_COUNT] = samples_count;
}
