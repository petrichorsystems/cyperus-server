/* ops_modules_delay_simple.c
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

#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_delay_simple.h"
#include "math_modules_delay_simple.h"
#include "ops_modules_delay_simple.h"

int
dsp_create_delay_simple(struct dsp_bus *target_bus,
                        float amount,
                        float time,
                        float feedback
                        ) {

  printf("ops_modules_delay_simple.c::dsp_create_delay_simple()\n");
  
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "delay_simple";  

  /* audio input/output */
  params.in = malloc(sizeof(float) * dsp_global_period);
  params.out = malloc(sizeof(float) * dsp_global_period);  
  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));
  
  params.parameters->int32_type = malloc(sizeof(int) * 2);
  params.parameters->float32_type = malloc(sizeof(int) * 3);  
  params.parameters->float32_arr_type = malloc(sizeof(float *) * 4);
  params.parameters->int32_arr_type = malloc(sizeof(int *));  

  /* internal sample buffer -- max size 30sec */
  /*   allocation */
  params.parameters->float32_arr_type[PARAM_INTERNAL_SAMPLE_BUFFER] = calloc(jackcli_samplerate * 30, sizeof(float));
  /*   assignment */ 
  memset(params.parameters->float32_arr_type[PARAM_INTERNAL_SAMPLE_BUFFER], 0, jackcli_samplerate * 30 * sizeof(float));
  
  /* user-facing parameter allocation */
  params.parameters->float32_arr_type[PARAM_USER_AMOUNT] = calloc(dsp_global_period, sizeof(float)); /* amount */
  params.parameters->float32_arr_type[PARAM_USER_TIME] = calloc(dsp_global_period, sizeof(float)); /* time */  
  params.parameters->float32_arr_type[PARAM_USER_FEEDBACK] = calloc(dsp_global_period, sizeof(float)); /* feedback */

  /* internal parameter allocation */
  params.parameters->int32_arr_type[PARAM_INTERNAL_TIME_SAMPLES] = calloc(dsp_global_period, sizeof(int)); /* time (% of sec) to sample conversion */

  
  for(int p=0; p<dsp_global_period; p++) {
    /* user-facing parameter assignment */
    params.parameters->float32_arr_type[PARAM_USER_AMOUNT][p] = amount;
    params.parameters->float32_arr_type[PARAM_USER_TIME][p] = time;
    params.parameters->float32_arr_type[PARAM_USER_FEEDBACK][p] = feedback;

    /* internal parameter assignment, time (% of sec) to sample conversion */
    params.parameters->int32_arr_type[PARAM_INTERNAL_TIME_SAMPLES][p] = (int)(time * jackcli_samplerate);
  }
  
  /* internal parameter assignment */
  params.parameters->int32_type[PARAM_INTERNAL_DELAY_POS] = 0; /* delay_pos */
  params.parameters->int32_type[PARAM_INTERNAL_DELAY_TIME_POS] = 0; /* delay_time_pos */  

  /* osc listener parameters */
  params.parameters->float32_type[PARAM_LISTENER_AMOUNT] = amount;
  params.parameters->float32_type[PARAM_LISTENER_TIME] = time;
  params.parameters->float32_type[PARAM_LISTENER_FEEDBACK] = feedback;
  
  ins = dsp_port_in_init("in");
  ins->next = dsp_port_in_init("param_amount");
  ins->next->next = dsp_port_in_init("param_time");
  ins->next->next->next = dsp_port_in_init("param_feedback");  
  outs = dsp_port_out_init("out");
  
  dsp_add_module(target_bus,
		 "delay_simple",
		 dsp_delay_simple,
		 dsp_destroy_delay_simple,
                 dsp_osc_listener_delay_simple,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_delay_simple */

int
dsp_destroy_delay_simple(struct dsp_module *target_module) {
	free(target_module->dsp_param.parameters->int32_type);
	free(target_module->dsp_param.parameters->float32_type);	
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_INTERNAL_SAMPLE_BUFFER]);	
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_FEEDBACK]);
	free(target_module->dsp_param.parameters->float32_arr_type);
	free(target_module->dsp_param.parameters->int32_arr_type[PARAM_INTERNAL_TIME_SAMPLES]);		
	free(target_module->dsp_param.parameters);
	free(target_module->dsp_param.out);
	free(target_module->dsp_param.in);
	return 0;
} /* dsp_destroy_delay_simple */

void
dsp_delay_simple(struct dsp_operation *delay_simple, int jack_samplerate) {
  /* printf("ops_modules_delay_simple.c::dsp_delay_simple()\n"); */

  float *outsamples;
  int p;  
  
  dsp_sum_summands(delay_simple->module->dsp_param.in, delay_simple->ins->summands);
  
  /* handle params with connected inputs */
  if( delay_simple->ins->next->summands != NULL ) { /* amount */
    dsp_sum_summands(delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT], delay_simple->ins->next->summands);
  }
  if( delay_simple->ins->next->next->summands != NULL ) { /* time */
    dsp_sum_summands(delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME], delay_simple->ins->next->next->summands);
    /* calculate time in samples across current period */
    for(int p=0; p<dsp_global_period; p++) {
      delay_simple->module->dsp_param.parameters->int32_arr_type[PARAM_INTERNAL_TIME_SAMPLES][p] = (int)(delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME][p] * jack_samplerate);
    }
  }
  if( delay_simple->ins->next->next->next->summands != NULL ) { /* feedback */
    dsp_sum_summands(delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FEEDBACK], delay_simple->ins->next->next->next->summands);
  }  

  math_modules_delay_simple(&delay_simple->module->dsp_param,
			    jack_samplerate);

  memcpy(delay_simple->outs->sample->value,
         delay_simple->module->dsp_param.out,
         sizeof(float) * dsp_global_period);  
  
} /* dsp_delay_simple */

void dsp_edit_delay_simple(struct dsp_module *delay_simple,
                            float amount,
                            float time,
                            float feedback) {
  int time_samples = (int)(time * jackcli_samplerate);
  for(int p=0; p<dsp_global_period; p++) {
    delay_simple->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][p] = amount;  
    delay_simple->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME][p] =  time; 
    delay_simple->dsp_param.parameters->int32_arr_type[PARAM_INTERNAL_TIME_SAMPLES][p] = time_samples; /* internal time (% of sec) to sample conversion */
    delay_simple->dsp_param.parameters->float32_arr_type[PARAM_USER_FEEDBACK][p] = feedback;
  }
} /* dsp_edit_delay_simple */

void
dsp_osc_listener_delay_simple(struct dsp_operation *delay_simple, int jack_samplerate) {
  unsigned short param_connected = 0;
  if( (delay_simple->ins->next->summands != NULL) ||
      (delay_simple->ins->next->next->summands != NULL) ||
      (delay_simple->ins->next->next->next->summands != NULL) ) {
    param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* if new value is different than old value, send osc messages */
    if(
       delay_simple->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMOUNT] != delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0] ||
       delay_simple->module->dsp_param.parameters->float32_type[PARAM_LISTENER_TIME] != delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME][0] ||
       delay_simple->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FEEDBACK] != delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FEEDBACK][0]
       ) {
      int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
      char *path = (char *)malloc(sizeof(char) * path_len);
      snprintf(path, path_len, "%s%s", "/cyperus/listener/", delay_simple->module->id);    

      osc_send_broadcast( path, "fff",
              delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0],
              delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME][0],
              delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FEEDBACK][0]);

      /* assign new parameter to last parameter after we're reported the change */
      delay_simple->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMOUNT] = delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0];
      delay_simple->module->dsp_param.parameters->float32_type[PARAM_LISTENER_TIME] = delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_TIME][0];
      delay_simple->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FEEDBACK] = delay_simple->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FEEDBACK][0];
    }
  }
  
  return;
} /* dsp_osc_listener_delay_simple */
