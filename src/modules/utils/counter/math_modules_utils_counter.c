
#include "../../../dsp_types.h"
#include "../../../dsp.h"

#include "params_modules_utils_counter.h"

extern void math_modules_utils_counter(dsp_parameter *counter, int samplerate) {
	float *reset = counter->parameters->float32_arr_type[PARAM_USER_RESET];
	float *step_size = counter->parameters->float32_arr_type[PARAM_USER_STEP_SIZE];
	float *min = counter->parameters->float32_arr_type[PARAM_USER_MIN];
	float *max = counter->parameters->float32_arr_type[PARAM_USER_MAX];
	float *direction = counter->parameters->float32_arr_type[PARAM_USER_DIRECTION];
	float *auto_reset = counter->parameters->float32_arr_type[PARAM_USER_AUTO_RESET];

	float current_step = counter->parameters->float32_type[PARAM_INTERNAL_START];
	
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
		counter->out[p] = current_step;
	}
	
	if( current_step != counter->parameters->float32_type[0] ) {
		counter->parameters->float32_type[PARAM_INTERNAL_START] = current_step;
	}
}
