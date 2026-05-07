#include "params_modules_oscillator_sine.h"
#include "../../../dsp.h"

extern void params_modules_oscillator_sine_init(dsp_parameter *params,
						float frequency,
						float amplitude,
						float phase) {
	int p = 0;
	
	params->name = "oscillator_sine";  

	/* signal output */
	params->out = malloc(sizeof(float) * dsp_global_period);

	params->parameters = malloc(sizeof(dsp_module_parameters_t));
	
	params->parameters->float32_arr_type = malloc(sizeof(float*) * 3);
	params->parameters->float32_type = malloc(sizeof(float) * 7);
	params->parameters->int32_type = malloc(sizeof(int) * 2);

	/* user-facing parameter allocation */
	params->parameters->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_USER_AMPLITUDE] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_USER_PHASE] = calloc(dsp_global_period, sizeof(float));

	/* osc listener param state parameters */
	params->parameters->float32_type[PARAM_LISTENER_FREQUENCY] = frequency;
	params->parameters->float32_type[PARAM_LISTENER_AMPLITUDE] = amplitude;
	params->parameters->float32_type[PARAM_LISTENER_PHASE] = phase;
	
	/* pending parameter changes setup */
	atomic_flag_clear(&params->state_pending);	
	params->parameters_pending = malloc(sizeof(dsp_module_parameters_t));
	params->parameters_pending->float32_arr_type = malloc(sizeof(float*) * 3);
	params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float));
	params->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE] = calloc(dsp_global_period, sizeof(float));
	params->parameters_pending->float32_arr_type[PARAM_USER_PHASE] = calloc(dsp_global_period, sizeof(float));

	params->parameters->float32_type[PARAM_INTERNAL_PHASE_DELTA] = 0.0f;
	
	for(p=0; p<dsp_global_period; p++) {
	  params->parameters->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
	  params->parameters->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;
	  params->parameters->float32_arr_type[PARAM_USER_PHASE][p] = phase;

	  params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
	  params->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;
	  params->parameters_pending->float32_arr_type[PARAM_USER_PHASE][p] = phase;	  
	}
	
} /* params_modules_oscillator_sine_init */

extern void params_modules_oscillator_sine_edit_pending(dsp_parameter *sine,
							float frequency,
							float amplitude,
							float phase) {
	int p = 0;
	bool found_param_changes = false;

	if( sine->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][0] != frequency ||
	    sine->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][0] != amplitude ||
	    sine->parameters_pending->float32_arr_type[PARAM_USER_PHASE][0] != phase ) {
		found_param_changes = true;
	}
	
	if( found_param_changes ) {
		for(p=0; p<dsp_global_period; p++) {
			sine->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
			sine->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;	
			sine->parameters_pending->float32_arr_type[PARAM_USER_PHASE][p] = phase;
		}
		atomic_flag_test_and_set(&sine->state_pending);
	}
} /* params_modules_oscillator_sine_edit_pending */

extern void params_modules_oscillator_sine_edit_apply(dsp_parameter *sine) {
	if( atomic_flag_test_and_set(&sine->state_pending) ) {
		memcpy(sine->parameters->float32_arr_type[PARAM_USER_FREQUENCY],
		       sine->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY],
		       sizeof(float) * dsp_global_period);
		memcpy(sine->parameters->float32_arr_type[PARAM_USER_AMPLITUDE],
		       sine->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE],
		       sizeof(float) * dsp_global_period);		
		memcpy(sine->parameters->float32_arr_type[PARAM_USER_PHASE],
		       sine->parameters_pending->float32_arr_type[PARAM_USER_PHASE],
		       sizeof(float) * dsp_global_period);	
	}
	/* atomic_flag's can't be checked without setting them. so, whether the check
	   on &sine->state_pending is true or false, we need to clear it either way */	
	atomic_flag_clear(&sine->state_pending);
} /* params_modules_oscillator_sine_edit_apply */

extern void params_modules_oscillator_sine_free(dsp_parameter *sine) {
	free(sine->parameters->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(sine->parameters->float32_arr_type[PARAM_USER_AMPLITUDE]);
	free(sine->parameters->float32_arr_type[PARAM_USER_PHASE]);
	free(sine->parameters->float32_arr_type);
	free(sine->parameters->float32_type);
	free(sine->parameters->int32_type);
	free(sine->parameters);
	free(sine->out);
} /* params_modules_oscillator_sine_free */
