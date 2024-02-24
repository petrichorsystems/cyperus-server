
#include "math_modules_utils_counter.h"

extern float* math_modules_utils_counter(dsp_parameter *counter, int samplerate) {
	float *reset = counter->parameters->float32_arr_type[0];
	float *start = counter->parameters->float32_arr_type[1];
	float *step_size = counter->parameters->float32_arr_type[2];
	float *min = counter->parameters->float32_arr_type[3];
	float *max = counter->parameters->float32_arr_type[4];
	float *direction = counter->parameters->float32_arr_type[5];
	float *auto_reset = counter->parameters->float32_arr_type[6];

	float current_step = counter->parameters->float32_type[0];

	float *out = calloc(dsp_global_period, sizeof(float));
	
	for (int p=0; p<dsp_global_period; p++) {
		if( reset[p] ) {
			if( direction[p] > 0.0f ) {
				current_step = min[p];
			} else if( direction[p] < 0.0f ) {
				current_step = max[p];
			} else {
				/* direction is 0, so do nothing */
			}
		} else if( counter->in[p] ) {
			if( direction[p] > 0.0f ) {
				current_step += step_size[p]; 
				if( current_step > max[p] ) {
					if( auto_reset[p] ) {
						current_step = min[p];
					} else {
						current_step = max[p];
					}
				}
			} else if( direction[p] < 0.0f ) {
				current_step -= step_size[p];
				if( current_step < min[p] ) {
					if( auto_reset[p] ) {
						current_step = max[p];
					} else {
						current_step = min[p];
					}
				}    
			} else {
				/* direction is 0, so do nothing */
			}
		}
		out[p] = current_step;
	}
	
	if( current_step != counter->parameters->float32_type[0] ) {
		counter->parameters->float32_type[0] = current_step;
	}
  
	return out;
}
