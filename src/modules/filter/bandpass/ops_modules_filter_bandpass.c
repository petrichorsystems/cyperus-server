/* ops_modules_filter_bandpass.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2021 murray foster */

#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_filter_bandpass.h"
#include "math_modules_filter_bandpass.h"
#include "ops_modules_filter_bandpass.h"

int
dsp_create_filter_bandpass(struct dsp_bus *target_bus,
                           float frequency,
                           float q,
                           float amount) {
	
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "filter_bandpass";  

  /* audio input */
  params.in = malloc(sizeof(float) * dsp_global_period);
  params.out = malloc(sizeof(float) * dsp_global_period);  
  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));

  params.parameters->float32_arr_type = malloc(sizeof(float *) * 10);
  params.parameters->float32_type = malloc(sizeof(float) * 3);
  
  /* user-facing parameter allocation */
  params.parameters->float32_arr_type[PARAM_USER_FREQUENCY] = calloc(dsp_global_period, sizeof(float)); /* frequency */
  params.parameters->float32_arr_type[PARAM_USER_Q] = calloc(dsp_global_period, sizeof(float)); /* q */  
  params.parameters->float32_arr_type[PARAM_USER_AMOUNT] = calloc(dsp_global_period, sizeof(float)); /* amount */  

  /* internal parameter allocation */
  params.parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD] = calloc(dsp_global_period, sizeof(float)); /* frequency_old */
  params.parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD] = calloc(dsp_global_period, sizeof(float)); /* q_old */  
  params.parameters->float32_arr_type[PARAM_INTERNAL_LAST] = calloc(dsp_global_period, sizeof(float)); /* last */
  params.parameters->float32_arr_type[PARAM_INTERNAL_PREV] = calloc(dsp_global_period, sizeof(float)); /* prev */
  params.parameters->float32_arr_type[PARAM_INTERNAL_COEF1] = calloc(dsp_global_period, sizeof(float)); /* coef1 */    
  params.parameters->float32_arr_type[PARAM_INTERNAL_COEF2] = calloc(dsp_global_period, sizeof(float)); /* coef2 */
  params.parameters->float32_arr_type[PARAM_INTERNAL_GAIN] = calloc(dsp_global_period, sizeof(float)); /* gain */      

  /* parameter assignment */
  for(int p=0; p<dsp_global_period; p++) {
	  /* user-facing parameters */	  
	  params.parameters->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
	  params.parameters->float32_arr_type[PARAM_USER_Q][p] = q; 
	  params.parameters->float32_arr_type[PARAM_USER_AMOUNT][p] = amount;

	  /* internal parameters */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD][p] = 0.0f; /* frequency_old */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD][p] = 0.0f; /* q_old */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_LAST][p] = 0.0f; /* last */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_PREV][p] = 0.0f; /* prev */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_COEF1][p] = 0.0f; /* coef1 */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_COEF2][p] = 0.0f; /* coef2 */
	  params.parameters->float32_arr_type[PARAM_INTERNAL_GAIN][p] = 0.0f; /* gain */

  }

  /* osc listener parameter assignment */
  params.parameters->float32_type[PARAM_LISTENER_FREQUENCY] = frequency;
  params.parameters->float32_type[PARAM_LISTENER_Q] = q;
  params.parameters->float32_type[PARAM_LISTENER_AMOUNT] = amount;  
  
  math_modules_filter_bandpass_init(&params);
  
  ins = dsp_port_in_init("in");
  ins->next = dsp_port_in_init("param_frequency");
  ins->next->next = dsp_port_in_init("param_q");
  ins->next->next->next = dsp_port_in_init("param_amount");  

  outs = dsp_port_out_init("out");

  dsp_add_module(target_bus,
		 "filter_bandpass",
		 dsp_filter_bandpass,
		 dsp_destroy_filter_bandpass,
                 dsp_osc_listener_filter_bandpass,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_filter_bandpass */

int dsp_destroy_filter_bandpass(struct dsp_module *target_module) {
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_LAST]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_PREV]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_COEF1]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_COEF2]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_GAIN]);
        free(target_module->dsp_param.parameters->float32_arr_type);	     
	free(target_module->dsp_param.parameters->float32_type);	
	free(target_module->dsp_param.parameters);
	free(target_module->dsp_param.out);
	free(target_module->dsp_param.in);
	return 0;
} /* dsp_destroy_filter_bandpass */

void
dsp_filter_bandpass(struct dsp_operation *filter_bandpass, int jack_samplerate) {
	dsp_sum_summands(filter_bandpass->module->dsp_param.in, filter_bandpass->ins->summands);

	/* handle params with connected inputs */
	if (filter_bandpass->ins->next->summands != NULL ) { /* frequency */
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY], filter_bandpass->ins->next->summands);
	}
	
	if( filter_bandpass->ins->next->next->summands != NULL ) {
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q], filter_bandpass->ins->next->next->summands);
	}

	if( filter_bandpass->ins->next->next->next->summands != NULL ) {
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT], filter_bandpass->ins->next->next->next->summands);
	}
	
	math_modules_filter_bandpass(&filter_bandpass->module->dsp_param,
				     jack_samplerate);

	
	/* drive audio outputs */
	memcpy(filter_bandpass->outs->sample->value,
	       filter_bandpass->module->dsp_param.out,
	       sizeof(float) * dsp_global_period);

} /* dsp_filter_bandpass */

void dsp_edit_filter_bandpass(struct dsp_module *filter_bandpass,
                              float frequency,
                              float q,
                              float amount) {

	for(int p=0; p<dsp_global_period; p++) {
		filter_bandpass->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][p] = frequency;
		filter_bandpass->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][p] = q;
		filter_bandpass->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][p] = amount;
	}
  
} /* dsp_edit_filter_bandpass */

void
dsp_osc_listener_filter_bandpass(struct dsp_operation *filter_bandpass, int jack_samplerate) {
  unsigned short param_connected = 0;
  if( (filter_bandpass->ins->next->summands != NULL) ||
      (filter_bandpass->ins->next->next->summands != NULL) ||
      (filter_bandpass->ins->next->next->next->summands != NULL) ) {
    param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* if new value is different than old value, send osc messages */
    if(
       filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0] ||
       filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_Q] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][0] ||
       filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMOUNT] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0]
       ) {
      int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
      char *path = (char *)malloc(sizeof(char) * path_len);
      snprintf(path, path_len, "%s%s", "/cyperus/listener/", filter_bandpass->module->id);    

      osc_send_broadcast( path, "fff",
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0],
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][0],
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0]);

      /* assign new parameter to last parameter after we're reported the change */
      filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0];
      filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_Q] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][0];
      filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMOUNT] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0];
    }
  }
  
  return;
} /* dsp_osc_listener_filter_bandpass */
