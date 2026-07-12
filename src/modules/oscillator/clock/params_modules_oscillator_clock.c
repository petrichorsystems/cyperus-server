#include "params_modules_oscillator_clock.h"
#include "../../../dsp.h"

extern void params_modules_oscillator_clock_init(dsp_parameter *params,
						float frequency,
						float amplitude) {
	int p;
  
	params->name = "oscillator_clock";  

	params->out = malloc(sizeof(float) * dsp_global_period);
  
	params->parameters = malloc(sizeof(dsp_module_parameters_t));
  
	params->parameters->float32_arr_type = malloc(sizeof(float *) * 2);
	params->parameters->float32_type = malloc(sizeof(float) * 2);
	params->parameters->int32_type = malloc(sizeof(int) * 1);
	
	params->parameters->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float)); /* frequency */
	params->parameters->float32_arr_type[PARAM_USER_AMPLITUDE] = calloc(dsp_global_period, sizeof(float)); /* amplitude */

	/* internal parameters */
	params->parameters->int32_type[PARAM_INTERNAL_SAMPLES_COUNT] = 0;
                                            
	/* osc listener param state parameters */                                            
	params->parameters->float32_type[PARAM_LISTENER_FREQUENCY] = frequency; /* old frequency */
	params->parameters->float32_type[PARAM_LISTENER_AMPLITUDE] = amplitude; /* old amplitude */

	params->parameters_pending = malloc(sizeof(dsp_module_parameters_t));  
	params->parameters_pending->float32_arr_type = malloc(sizeof(float *) * 2);
	params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float)); /* frequency */
	params->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE] = calloc(dsp_global_period, sizeof(float)); /* amplitude */
	
	for (p=0; p<dsp_global_period; p++) {
		params->parameters->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
		params->parameters->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;
		params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
		params->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;		
	}	
} /* params_modules_oscillator_clock_init */

extern void params_modules_oscillator_clock_edit_pending(dsp_parameter *oscillator_clock,
							float frequency,
							float amplitude) {
	int p = 0;
	bool found_param_changes = false;

	if( oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][0] != frequency ||
	    oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][0] != amplitude ) {
		found_param_changes = true;
	}
	
	if( found_param_changes ) {
		for(p=0; p<dsp_global_period; p++) {
			oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
			oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;
		}
		atomic_flag_test_and_set(&oscillator_clock->state_pending);
	}
} /* params_modules_oscillator_clock_edit_pending */

extern void params_modules_oscillator_clock_edit_apply(dsp_parameter *oscillator_clock) {
	if( atomic_flag_test_and_set(&oscillator_clock->state_pending) ) {
		memcpy(oscillator_clock->parameters->float32_arr_type[PARAM_USER_FREQUENCY],
		       oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY],
		       sizeof(float) * dsp_global_period);
		memcpy(oscillator_clock->parameters->float32_arr_type[PARAM_USER_AMPLITUDE],
		       oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE],
		       sizeof(float) * dsp_global_period);
	}
	/* atomic_flag's can't be checked without setting them. so, whether the check
	   on &oscillator_clock->state_pending is true or false, we need to clear it either way */	
	atomic_flag_clear(&oscillator_clock->state_pending);
} /* params_modules_oscillator_clock_edit_apply */

extern void params_modules_oscillator_clock_free(dsp_parameter *oscillator_clock) {
	free(oscillator_clock->parameters->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(oscillator_clock->parameters->float32_arr_type[PARAM_USER_AMPLITUDE]);
	free(oscillator_clock->parameters->float32_arr_type);
	free(oscillator_clock->parameters->float32_type);
	free(oscillator_clock->parameters->int32_type);
	free(oscillator_clock->parameters);
	free(oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(oscillator_clock->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE]);
	free(oscillator_clock->parameters_pending->float32_arr_type);
	free(oscillator_clock->parameters_pending);	
	free(oscillator_clock->out);
	free(oscillator_clock->in);
} /* params_modules_oscillator_clock_free */
