#include "params_modules_envelope_follower.h"
#include "../../../dsp.h"

extern void params_modules_envelope_follower_init(dsp_parameter *params,
						float attack,
						float decay,
						float scale) {
	params->name = "envelope_follower";  

	/* signal input/output */
	params->in = malloc(sizeof(float) * dsp_global_period);
	params->out = malloc(sizeof(float) * dsp_global_period);
  
	params->parameters = malloc(sizeof(dsp_module_parameters_t));

	params->parameters->float32_type = malloc(sizeof(float) * 4);
	params->parameters->float32_arr_type = malloc(sizeof(float *) * 7);  
	params->parameters->bool_type = malloc(sizeof(bool) * 2);
  
	/* user-facing parameter allocation */
	params->parameters->float32_arr_type[PARAM_USER_ATTACK_MS] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_USER_DECAY_MS] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_USER_SCALE] = calloc(dsp_global_period, sizeof(float));

	/* internal parameter assignment */
	params->parameters->float32_type[PARAM_INTERNAL_LAST_OUTPUT] = 0.0f; /* last output sample */
	params->parameters->float32_arr_type[PARAM_INTERNAL_ATTACK_MS] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_INTERNAL_DECAY_MS] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_INTERNAL_COEFF_ATTACK] = calloc(dsp_global_period, sizeof(float));
	params->parameters->float32_arr_type[PARAM_INTERNAL_COEFF_DECAY] = calloc(dsp_global_period, sizeof(float));
	params->parameters->bool_type[PARAM_INTERNAL_ATTACK_MS_CONNECTED] = false;
	params->parameters->bool_type[PARAM_INTERNAL_DECAY_MS_CONNECTED] = false;	  

	/* osc listener parameters */
	params->parameters->float32_type[PARAM_LISTENER_ATTACK_MS] = attack;
	params->parameters->float32_type[PARAM_LISTENER_DECAY_MS] = decay;
	params->parameters->float32_type[PARAM_LISTENER_SCALE] = scale;

	/* pending parameter changes setup */
	atomic_flag_clear(&params->state_pending);	
	params->parameters_pending = malloc(sizeof(dsp_module_parameters_t));
	params->parameters_pending->float32_arr_type = malloc(sizeof(float *) * 3);  	
	params->parameters_pending->float32_arr_type[PARAM_USER_ATTACK_MS] = calloc(dsp_global_period, sizeof(float));
	params->parameters_pending->float32_arr_type[PARAM_USER_DECAY_MS] = calloc(dsp_global_period, sizeof(float));
	params->parameters_pending->float32_arr_type[PARAM_USER_SCALE] = calloc(dsp_global_period, sizeof(float));

	for(int p=0; p<dsp_global_period; p++) {
		/* user-facing parameter assignment */
		params->parameters->float32_arr_type[PARAM_USER_ATTACK_MS][p] = attack; 
		params->parameters->float32_arr_type[PARAM_USER_DECAY_MS][p] = decay;
		params->parameters->float32_arr_type[PARAM_USER_SCALE][p] = scale;

		params->parameters_pending->float32_arr_type[PARAM_USER_ATTACK_MS][p] = attack; 
		params->parameters_pending->float32_arr_type[PARAM_USER_DECAY_MS][p] = decay;
		params->parameters_pending->float32_arr_type[PARAM_USER_SCALE][p] = scale;		
	}	
	
} /* params_modules_envelope_follower_init */

extern void params_modules_envelope_follower_edit_pending(dsp_parameter *follower,
							float attack_ms,
							float decay_ms,
							float scale) {
	int p = 0;
	bool found_param_changes = false;

	if( follower->parameters_pending->float32_arr_type[PARAM_USER_ATTACK_MS][0] != attack_ms ||
	    follower->parameters_pending->float32_arr_type[PARAM_USER_DECAY_MS][0] != decay_ms ||
	    follower->parameters_pending->float32_arr_type[PARAM_USER_SCALE][0] != scale ) {
		found_param_changes = true;
	}
	
	if( found_param_changes ) {
		for(p=0; p<dsp_global_period; p++) {
			follower->parameters_pending->float32_arr_type[PARAM_USER_ATTACK_MS][p] = attack_ms;
			follower->parameters_pending->float32_arr_type[PARAM_USER_DECAY_MS][p] = decay_ms;	
			follower->parameters_pending->float32_arr_type[PARAM_USER_SCALE][p] = scale;
		}
		atomic_flag_test_and_set(&follower->state_pending);
	}
} /* params_modules_envelope_follower_edit_pending */

extern void params_modules_envelope_follower_edit_apply(dsp_parameter *follower) {
	if( atomic_flag_test_and_set(&follower->state_pending) ) {
		memcpy(follower->parameters->float32_arr_type[PARAM_USER_ATTACK_MS],
		       follower->parameters_pending->float32_arr_type[PARAM_USER_ATTACK_MS],
		       sizeof(float) * dsp_global_period);
		memcpy(follower->parameters->float32_arr_type[PARAM_USER_DECAY_MS],
		       follower->parameters_pending->float32_arr_type[PARAM_USER_DECAY_MS],
		       sizeof(float) * dsp_global_period);		
		memcpy(follower->parameters->float32_arr_type[PARAM_USER_SCALE],
		       follower->parameters_pending->float32_arr_type[PARAM_USER_SCALE],
		       sizeof(float) * dsp_global_period);	
	}
	/* atomic_flag's can't be checked without setting them. so, whether the check
	   on &follower->state_pending is true or false, we need to clear it either way */	
	atomic_flag_clear(&follower->state_pending);
} /* params_modules_envelope_follower_edit_apply */

extern void params_modules_envelope_follower_free(dsp_parameter *follower) {
	free(follower->parameters->float32_arr_type[PARAM_USER_ATTACK_MS]);
	free(follower->parameters->float32_arr_type[PARAM_USER_DECAY_MS]);
	free(follower->parameters->float32_arr_type[PARAM_USER_SCALE]);
	free(follower->parameters->float32_arr_type[PARAM_INTERNAL_ATTACK_MS]);
	free(follower->parameters->float32_arr_type[PARAM_INTERNAL_DECAY_MS]);
	free(follower->parameters->float32_arr_type[PARAM_INTERNAL_COEFF_ATTACK]);
	free(follower->parameters->float32_arr_type[PARAM_INTERNAL_COEFF_DECAY]);
	free(follower->parameters->float32_arr_type);	
	free(follower->parameters->float32_type);
	free(follower->parameters_pending->float32_arr_type[PARAM_USER_ATTACK_MS]);
	free(follower->parameters_pending->float32_arr_type[PARAM_USER_DECAY_MS]);
	free(follower->parameters_pending->float32_arr_type[PARAM_USER_SCALE]);
	free(follower->parameters_pending->float32_arr_type);	
	free(follower->parameters_pending->float32_type);	
	free(follower->parameters);
	free(follower->out);
	free(follower->in);
	free(follower);
} /* params_modules_envelope_follower_free */
