#include "params_modules_utils_float.h"
#include "../../../dsp.h"

extern void params_modules_utils_float_init(dsp_parameter *params,
					    float value) {
	params->name = "utils_float";
	params->pos = 0;

	/* audio input/output */
	params->in = malloc(sizeof(float) * dsp_global_period);
	params->out = malloc(sizeof(float) * dsp_global_period);  

	params->parameters = malloc(sizeof(dsp_module_parameters_t));
  
	params->parameters->float32_type = malloc(sizeof(float));

	for(int p=0; p<dsp_global_period; p++) {
		/* user-facing parameter assignment */
		params->in[p] = value;
	}
  
	/* osc listener parameters */
	params->parameters->float32_type[PARAM_LISTENER_FLOAT_VAL] = value;
  
	/* pending parameter changes setup */
	atomic_flag_clear(&params->state_pending);	
	params->parameters_pending = malloc(sizeof(dsp_module_parameters_t));
	params->parameters_pending->float32_arr_type = malloc(sizeof(float *) * 1);
	
	params->parameters_pending->float32_arr_type[PARAM_USER_FLOAT_VAL] = calloc(dsp_global_period, sizeof(float));
	
	for(int p=0; p<dsp_global_period; p++) {
		params->parameters_pending->float32_arr_type[PARAM_USER_FLOAT_VAL][p] = value;
	}	
	
} /* params_modules_utils_float_init */

extern void params_modules_utils_float_edit_pending(dsp_parameter *utils_float,
						    float value) {
	int p = 0;
	bool found_param_changes = false;

	if( utils_float->parameters_pending->float32_arr_type[PARAM_USER_FLOAT_VAL][0] != value ) {
		found_param_changes = true;
	}
	
	if( found_param_changes ) {
		for(p=0; p<dsp_global_period; p++) {
			utils_float->parameters_pending->float32_arr_type[PARAM_USER_FLOAT_VAL][p] = value;
		}
		atomic_flag_test_and_set(&utils_float->state_pending);
	}
} /* params_modules_utils_float_edit_pending */

extern void params_modules_utils_float_edit_apply(dsp_parameter *utils_float) {
	if( atomic_flag_test_and_set(&utils_float->state_pending) ) {
		memcpy(utils_float->in,
		       utils_float->parameters_pending->float32_arr_type[PARAM_USER_FLOAT_VAL],
		       sizeof(float) * dsp_global_period);	
	}
	/* atomic_flag's can't be checked without setting them. so, whether the check
	   on &utils_float->state_pending is true or false, we need to clear it either way */	
	atomic_flag_clear(&utils_float->state_pending);
} /* params_modules_utils_float_edit_apply */

extern void params_modules_utils_float_free(dsp_parameter *utils_float) {
	free(utils_float->parameters->float32_type);	
	free(utils_float->parameters);
	free(utils_float->parameters_pending->float32_arr_type[PARAM_USER_FLOAT_VAL]);
	free(utils_float->parameters_pending->float32_arr_type);
	free(utils_float->parameters_pending);
	free(utils_float->out);
	free(utils_float->in);
} /* params_modules_utils_float_free */
