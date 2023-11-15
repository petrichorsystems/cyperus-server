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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

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

  params.in = malloc(sizeof(float) * dsp_global_period);
  params.name = "delay_simple";  
  params.pos = 0;
  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));
  
  params.parameters->int32_type = malloc(sizeof(int) * 2);
  params.parameters->float32_arr_type = malloc(sizeof(float *) * 4);
  params.parameters->int32_arr_type = malloc(sizeof(int *));  
  
  params.parameters->float32_arr_type[1] = calloc(dsp_global_period, sizeof(float)); /* user-facing amount parameter */
  params.parameters->float32_arr_type[2] = calloc(dsp_global_period, sizeof(float)); /* user-facing time parameter */  
  params.parameters->float32_arr_type[3] = calloc(dsp_global_period, sizeof(float)); /* user-facing feedback parameter */

  params.parameters->int32_arr_type[0] = calloc(dsp_global_period, sizeof(int)); /* internal time (% of sec) to sample conversion */

  for(int p=0; p<dsp_global_period; p++) {
    params.parameters->float32_arr_type[1][p] = amount; /* user-facing amount parameter */
    params.parameters->float32_arr_type[2][p] = time; /* user-facing time parameter */  
    params.parameters->float32_arr_type[3][p] = feedback; /* user-facing feedback parameter */  
    
    params.parameters->int32_arr_type[0][p] = (int)(time * jackcli_samplerate); /* internal time (% of sec) to sample conversion */
  }
  
  /* internal parameters */
  params.parameters->int32_type[0] = 0; /* delay_pos */
  params.parameters->int32_type[1] = 0; /* delay_time_pos */
  
  params.parameters->float32_arr_type[0] = calloc(time * jackcli_samplerate * 30, sizeof(float)); /* delay sample buffer */
  
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_amount", 512);
  ins->next->next = dsp_port_in_init("param_time", 512);
  ins->next->next->next = dsp_port_in_init("param_feedback", 512);  
  outs = dsp_port_out_init("out", 1);
  
  dsp_add_module(target_bus,
		 "delay_simple",
		 dsp_delay_simple,
                 NULL,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_delay_simple */

void
dsp_delay_simple(struct dsp_operation *delay_simple, int jack_samplerate, int pos) {
  /* printf("ops_modules_delay_simple.c::dsp_delay_simple()\n"); */
  
  float *insamples = malloc(sizeof(float) * dsp_global_period);
  float *outsamples = malloc(sizeof(float) * dsp_global_period);

  float *param_time_block = malloc(sizeof(float) * dsp_global_period);

  int p;

  memset(insamples, 0.0f, sizeof(float) * dsp_global_period);
  memset(outsamples, 0.0f, sizeof(float) * dsp_global_period);
  memset(param_time_block, 0.0f, sizeof(float) * dsp_global_period);
  
  dsp_sum_summands(delay_simple->ins->summands, insamples);
  
  memcpy(delay_simple->module->dsp_param.in, insamples, sizeof(float) * dsp_global_period);

  /* param_time input */
  if( delay_simple->ins->next->next->summands != NULL ) {
    dsp_sum_summands(delay_simple->ins->next->next->summands, param_time_block);
    memcpy(delay_simple->module->dsp_param.parameters->float32_arr_type[2], param_time_block, sizeof(float) * dsp_global_period);

    for(int p=0; p<dsp_global_period; p++) {
      delay_simple->module->dsp_param.parameters->int32_arr_type[0][p] = (int)(delay_simple->module->dsp_param.parameters->float32_arr_type[2][p] * jack_samplerate);
    }
 
    free(param_time_block);
  }

  outsamples = math_modules_delay_simple(&delay_simple->module->dsp_param,
                                         jack_samplerate,
                                         pos);
  
  /* drive audio outputs */
  memcpy(delay_simple->outs->sample->value,  outsamples, sizeof(float) * dsp_global_period);
  
  free(insamples);  
  free(outsamples);
  
} /* dsp_delay_simple */


void dsp_edit_delay_simple(struct dsp_module *delay_simple,
                            float amount,
                            float time,
                            float feedback
                           ) {
  for(int p=0; p<dsp_global_period; p++) {
    delay_simple->dsp_param.parameters->float32_arr_type[1][p] = amount; /* user-facing amount parameter */
  
    delay_simple->dsp_param.parameters->float32_arr_type[2][p] =  time; /* user-facing time parameter */
    delay_simple->dsp_param.parameters->int32_arr_type[0][p] = (int)(time * jackcli_samplerate); /* internal time (% of sec) to sample conversion */
  
    delay_simple->dsp_param.parameters->float32_arr_type[3][p] = feedback; /* user-facing feedback parameter */
  }

  printf("returning\n");
  
} /* dsp_edit_delay_simple */
