#include "params_modules_oscillator_triangle.h"
#include "../../../dsp.h"

extern void params_modules_oscillator_triangle_init(dsp_parameter *params,
						    float frequency,
						    float amplitude) {	
	int p = 0;

	params->name = "oscillator_triangle";

	params->out = malloc(sizeof(float) * dsp_global_period);
	
	params->parameters = malloc(sizeof(dsp_module_parameters_t));  

	params->parameters->float32_arr_type = malloc(sizeof(float *) * 2);
	params->parameters->float32_type = malloc(sizeof(float) * 2);
	params->parameters->int32_type = malloc(sizeof(int) * 1);
	
	/* user-facing parameter allocation */
	params->parameters->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float)); /* frequency */
	params->parameters->float32_arr_type[PARAM_USER_AMPLITUDE] = calloc(dsp_global_period, sizeof(float)); /* amplitude */

	/* osc listener param state parameters */
	params->parameters->float32_type[PARAM_LISTENER_FREQUENCY] = frequency;
	params->parameters->float32_type[PARAM_LISTENER_AMPLITUDE] = amplitude;

	/* pending parameter changes setup */
	atomic_flag_clear(&params->state_pending);	
	params->parameters_pending = malloc(sizeof(dsp_module_parameters_t));
	params->parameters_pending->float32_arr_type = malloc(sizeof(float*) * 2);
	params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float));
	params->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE] = calloc(dsp_global_period, sizeof(float));
	
	/* user-facing parameter assigment */
	for (p=0; p<dsp_global_period; p++) {
		params->parameters->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
		params->parameters->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;

		params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
		params->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;		
	}

	/* internal parameter assignment */
	params->parameters->int32_type[PARAM_INTERNAL_X] = 0;

	
} /* params_modules_oscillator_triangle_init */

extern void params_modules_oscillator_triangle_edit_pending(dsp_parameter *triangle,
							    float frequency,
							    float amplitude) {
	int p = 0;
	bool found_param_changes = false;

	if( triangle->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][0] != frequency ||
	    triangle->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][0] != amplitude ) {
		found_param_changes = true;
	}
	
	if( found_param_changes ) {
		for(p=0; p<dsp_global_period; p++) {
			triangle->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
			triangle->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE][p] = amplitude;	
		}
		atomic_flag_test_and_set(&triangle->state_pending);
	}
} /* params_modules_oscillator_triangle_edit_pending */

extern void params_modules_oscillator_triangle_edit_apply(dsp_parameter *triangle) {
	if( atomic_flag_test_and_set(&triangle->state_pending) ) {
		memcpy(triangle->parameters->float32_arr_type[PARAM_USER_FREQUENCY],
		       triangle->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY],
		       sizeof(float) * dsp_global_period);
		memcpy(triangle->parameters->float32_arr_type[PARAM_USER_AMPLITUDE],
		       triangle->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE],
		       sizeof(float) * dsp_global_period);		
	}
	/* atomic_flag's can't be checked without setting them. so, whether the check
	   on &triangle->state_pending is true or false, we need to clear it either way */	
	atomic_flag_clear(&triangle->state_pending);
} /* params_modules_oscillator_triangle_edit_apply */

extern void params_modules_oscillator_triangle_free(dsp_parameter *triangle) {
	free(triangle->parameters->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(triangle->parameters->float32_arr_type[PARAM_USER_AMPLITUDE]);
	free(triangle->parameters->float32_arr_type);
	free(triangle->parameters->float32_type);
	free(triangle->parameters->int32_type);
	free(triangle->parameters);
	free(triangle->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(triangle->parameters_pending->float32_arr_type[PARAM_USER_AMPLITUDE]);
	free(triangle->parameters_pending->float32_arr_type);
	free(triangle->parameters_pending);	
	free(triangle->out);
} /* params_modules_oscillator_triangle_free */
