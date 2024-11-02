/* ops_modules_filter_bandpass.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under the terms of the GNU General Public License as published by
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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

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
  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));

  params.parameters->float32_arr_type = malloc(sizeof(float *) * 10);
  params.parameters->float32_type = malloc(sizeof(float) * 3);
  
  /* user-facing parameter allocation */
  params.parameters->float32_arr_type[0] = calloc(dsp_global_period, sizeof(float)); /* frequency */
  params.parameters->float32_arr_type[1] = calloc(dsp_global_period, sizeof(float)); /* q */  
  params.parameters->float32_arr_type[2] = calloc(dsp_global_period, sizeof(float)); /* amount */  

  /* internal parameter allocation */
  params.parameters->float32_arr_type[3] = calloc(dsp_global_period, sizeof(float)); /* frequency_old */
  params.parameters->float32_arr_type[4] = calloc(dsp_global_period, sizeof(float)); /* q_old */  
  params.parameters->float32_arr_type[5] = calloc(dsp_global_period, sizeof(float)); /* last */
  params.parameters->float32_arr_type[6] = calloc(dsp_global_period, sizeof(float)); /* prev */
  params.parameters->float32_arr_type[7] = calloc(dsp_global_period, sizeof(float)); /* q */  
  params.parameters->float32_arr_type[8] = calloc(dsp_global_period, sizeof(float)); /* coef1 */    
  params.parameters->float32_arr_type[8] = calloc(dsp_global_period, sizeof(float)); /* coef2 */
  params.parameters->float32_arr_type[9] = calloc(dsp_global_period, sizeof(float)); /* gain */      

  /* parameter assignment */
  for(int p=0; p<dsp_global_period; p++) {
	  /* user-facing parameters */	  
	  params.parameters->float32_arr_type[0][p] = frequency;
	  params.parameters->float32_arr_type[1][p] = q; 
	  params.parameters->float32_arr_type[2][p] = amount;

	  /* internal parameters */
	  params.parameters->float32_arr_type[3][p] = 0.0f; /* frequency_old */
	  params.parameters->float32_arr_type[4][p] = 0.0f; /* q_old */
	  params.parameters->float32_arr_type[5][p] = 0.0f; /* last */
	  params.parameters->float32_arr_type[6][p] = 0.0f; /* prev */
	  params.parameters->float32_arr_type[7][p] = 0.0f; /* coef1 */
	  params.parameters->float32_arr_type[8][p] = 0.0f; /* coef2 */
	  params.parameters->float32_arr_type[9][p] = 0.0f; /* gain */

  }

  /* osc listener parameter assignment */
  params.parameters->float32_type[0] = frequency;
  params.parameters->float32_type[1] = q;
  params.parameters->float32_type[2] = amount;  
  
  math_modules_filter_bandpass_init(&params);
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_frequency", 512);
  ins->next->next = dsp_port_in_init("param_q", 512);
  ins->next->next->next = dsp_port_in_init("param_amount", 512);  

  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "filter_bandpass",
		 dsp_filter_bandpass,
                 dsp_osc_listener_filter_bandpass,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_filter_bandpass */

void
dsp_filter_bandpass(struct dsp_operation *filter_bandpass, int jack_samplerate) {
	float *outsamples;

	dsp_sum_summands(filter_bandpass->module->dsp_param.in, filter_bandpass->ins->summands);

	/* handle params with connected inputs */
	if (filter_bandpass->ins->next->summands != NULL ) { /* frequency */
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[0], filter_bandpass->ins->next->summands);
	}
	
	if( filter_bandpass->ins->next->next->summands != NULL ) {
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[1], filter_bandpass->ins->next->next->summands);
	}

	if( filter_bandpass->ins->next->next->next->summands != NULL ) {
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[2], filter_bandpass->ins->next->next->next->summands);
	}
	
	outsamples = math_modules_filter_bandpass(&filter_bandpass->module->dsp_param,
						  jack_samplerate);

	
	/* drive audio outputs */
	memcpy(filter_bandpass->outs->sample->value,
	       outsamples,
	       sizeof(float) * dsp_global_period);

	free(outsamples);
} /* dsp_filter_bandpass */


void dsp_edit_filter_bandpass(struct dsp_module *filter_bandpass,
                              float frequency,
                              float q,
                              float amount) {

	for(int p=0; p<dsp_global_period; p++) {
		filter_bandpass->dsp_param.parameters->float32_arr_type[0][p] = frequency;
		filter_bandpass->dsp_param.parameters->float32_arr_type[1][p] = q;
		filter_bandpass->dsp_param.parameters->float32_arr_type[2][p] = amount;
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
       filter_bandpass->module->dsp_param.parameters->float32_type[0] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[0][0] ||
       filter_bandpass->module->dsp_param.parameters->float32_type[1] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[1][0] ||
       filter_bandpass->module->dsp_param.parameters->float32_type[2] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[2][0]
       ) {
      int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
      char *path = (char *)malloc(sizeof(char) * path_len);
      snprintf(path, path_len, "%s%s", "/cyperus/listener/", filter_bandpass->module->id);    

      osc_send_broadcast( path, "fff",
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[0][0],
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[1][0],
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[2][0]);

      /* assign new parameter to last parameter after we're reported the change */
      filter_bandpass->module->dsp_param.parameters->float32_type[0] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[0][0];
      filter_bandpass->module->dsp_param.parameters->float32_type[1] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[1][0];
      filter_bandpass->module->dsp_param.parameters->float32_type[2] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[2][0];
    }
  }
  
  return;
} /* dsp_osc_listener_filter_bandpass */
