#include "params_modules_filter_bandpass.h"
#include "../../../dsp.h"

extern void params_modules_filter_bandpass_init(dsp_parameter *params,
						float frequency,
						float q,
						float amount) {

  params->name = "filter_bandpass";  

  /* audio input */
  params->in = malloc(sizeof(float) * dsp_global_period);
  params->out = malloc(sizeof(float) * dsp_global_period);  
  
  params->parameters = malloc(sizeof(dsp_module_parameters_t));

  params->parameters->float32_arr_type = malloc(sizeof(float *) * 10);
  params->parameters->float32_type = malloc(sizeof(float) * 3);
  
  /* user-facing parameter allocation */
  params->parameters->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float)); /* frequency */
  params->parameters->float32_arr_type[PARAM_USER_Q] = calloc(dsp_global_period, sizeof(float)); /* q */  
  params->parameters->float32_arr_type[PARAM_USER_AMOUNT] = calloc(dsp_global_period, sizeof(float)); /* amount */  

  /* internal parameter allocation */
  params->parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD] = calloc(dsp_global_period, sizeof(float)); /* frequency_old */
  params->parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD] = calloc(dsp_global_period, sizeof(float)); /* q_old */  
  params->parameters->float32_arr_type[PARAM_INTERNAL_LAST] = calloc(dsp_global_period, sizeof(float)); /* last */
  params->parameters->float32_arr_type[PARAM_INTERNAL_PREV] = calloc(dsp_global_period, sizeof(float)); /* prev */
  params->parameters->float32_arr_type[PARAM_INTERNAL_COEF1] = calloc(dsp_global_period, sizeof(float)); /* coef1 */    
  params->parameters->float32_arr_type[PARAM_INTERNAL_COEF2] = calloc(dsp_global_period, sizeof(float)); /* coef2 */
  params->parameters->float32_arr_type[PARAM_INTERNAL_GAIN] = calloc(dsp_global_period, sizeof(float)); /* gain */      

  params->parameters->float32_type[PARAM_LISTENER_FREQUENCY] = frequency;
  params->parameters->float32_type[PARAM_LISTENER_Q] = q;
  params->parameters->float32_type[PARAM_LISTENER_AMOUNT] = amount;
  
  /* pending parameter changes setup */
  atomic_flag_clear(&params->state_pending);	
  params->parameters_pending = malloc(sizeof(dsp_module_parameters_t));
  params->parameters_pending->float32_arr_type = malloc(sizeof(float *) * 3);
	
  params->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float));
  params->parameters_pending->float32_arr_type[PARAM_USER_Q] = calloc(dsp_global_period, sizeof(float));
  params->parameters_pending->float32_arr_type[PARAM_USER_AMOUNT] = calloc(dsp_global_period, sizeof(float));
  
  /* parameter assignment */
  for(int p=0; p<dsp_global_period; p++) {
	  /* user-facing parameters */	  
	  params->parameters->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
	  params->parameters->float32_arr_type[PARAM_USER_Q][p] = q; 
	  params->parameters->float32_arr_type[PARAM_USER_AMOUNT][p] = amount;

	  /* internal parameters */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD][p] = 0.0f; /* frequency_old */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD][p] = 0.0f; /* q_old */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_LAST][p] = 0.0f; /* last */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_PREV][p] = 0.0f; /* prev */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_COEF1][p] = 0.0f; /* coef1 */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_COEF2][p] = 0.0f; /* coef2 */
	  params->parameters->float32_arr_type[PARAM_INTERNAL_GAIN][p] = 0.0f; /* gain */

  }	
} /* params_modules_filter_bandpass_init */

extern void params_modules_filter_bandpass_edit_pending(dsp_parameter *filter_bandpass,
							float frequency,
							float q,
							float amount) {
	int p = 0;
	bool found_param_changes = false;

	if( filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][0] != frequency ||
	    filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_Q][0] != q ||
	    filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_AMOUNT][0] != amount ) {
		found_param_changes = true;
	}
	
	if( found_param_changes ) {
		for(p=0; p<dsp_global_period; p++) {
			filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
			filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_Q][p] = q;	
			filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_AMOUNT][p] = amount;
		}
		atomic_flag_test_and_set(&filter_bandpass->state_pending);
	}
} /* params_modules_filter_bandpass_edit_pending */

extern void params_modules_filter_bandpass_edit_apply(dsp_parameter *filter_bandpass) {
	if( atomic_flag_test_and_set(&filter_bandpass->state_pending) ) {
		memcpy(filter_bandpass->parameters->float32_arr_type[PARAM_USER_FREQUENCY],
		       filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY],
		       sizeof(float) * dsp_global_period);
		memcpy(filter_bandpass->parameters->float32_arr_type[PARAM_USER_Q],
		       filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_Q],
		       sizeof(float) * dsp_global_period);		
		memcpy(filter_bandpass->parameters->float32_arr_type[PARAM_USER_AMOUNT],
		       filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_AMOUNT],
		       sizeof(float) * dsp_global_period);			
	}
	/* atomic_flag's can't be checked without setting them. so, whether the check
	   on &filter_bandpass->state_pending is true or false, we need to clear it either way */	
	atomic_flag_clear(&filter_bandpass->state_pending);
} /* params_modules_filter_bandpass_edit_apply */

extern void params_modules_filter_bandpass_free(dsp_parameter *filter_bandpass) {
	free(filter_bandpass->parameters->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_USER_Q]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_USER_AMOUNT]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_LAST]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_PREV]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_COEF1]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_COEF2]);
	free(filter_bandpass->parameters->float32_arr_type[PARAM_INTERNAL_GAIN]);
        free(filter_bandpass->parameters->float32_arr_type);	     
	free(filter_bandpass->parameters->float32_type);	
	free(filter_bandpass->parameters);
	free(filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_Q]);
	free(filter_bandpass->parameters_pending->float32_arr_type[PARAM_USER_AMOUNT]);
	free(filter_bandpass->parameters_pending->float32_arr_type);	
	free(filter_bandpass->parameters_pending);	
	free(filter_bandpass->out);
	free(filter_bandpass->in);	
} /* params_modules_filter_bandpass_free */
