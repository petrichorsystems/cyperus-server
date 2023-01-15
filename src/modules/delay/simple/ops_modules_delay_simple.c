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

  params.name = "delay_simple";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));  
  params.parameters->float32_type = malloc(sizeof(float) * 3);
  params.parameters->int32_type = malloc(sizeof(int) * 3);
  params.parameters->float32_arr_type = malloc(sizeof(float *) * 1);
  
  /* user-facing parameters */
  params.parameters->float32_type[0] = amount;
  params.parameters->float32_type[1] = time * jackcli_samplerate; 
  params.parameters->float32_type[2] = feedback; 

  /* internal parameters */
  params.parameters->int32_type[0] = (int)(time * jackcli_samplerate); /* time_samples */
  params.parameters->int32_type[1] = 0; /* delay_pos */
  params.parameters->int32_type[2] = 0; /* delay_time_pos */
  
  params.parameters->float32_arr_type[0] = calloc(time * jackcli_samplerate * 30, sizeof(float));

  
  ins = dsp_port_in_init("in", 512, NULL);
  ins->next = dsp_port_in_init("param_amount", 512, &(params.parameters->float32_type[0]));
  ins->next->next = dsp_port_in_init("param_time", 512, &(params.parameters->float32_type[1]));
  ins->next->next->next = dsp_port_in_init("param_feedback", 512, &(params.parameters->float32_type[2]));  
  outs = dsp_port_out_init("out", 1);
  
  dsp_add_module(target_bus,
		 "delay_simple",
		 dsp_delay_simple,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_delay_simple */

void
dsp_delay_simple(struct dsp_operation *delay_simple, int jack_samplerate, int pos) {
  /* printf("ops_modules_delay_simple.c::dsp_delay_simple()\n"); */
  
  float insample = 0.0f;
  float outsample = 0.0f;

  insample = dsp_sum_summands(delay_simple->ins->summands);
  delay_simple->module->dsp_param.in = insample;

  /* param_time input */
  if( delay_simple->ins->next->next->summands != NULL ) {
      delay_simple->module->dsp_param.parameters->float32_type[1] = dsp_sum_summands(delay_simple->ins->next->next->summands);
        delay_simple->module->dsp_param.parameters->int32_type[0] = (int)(delay_simple->module->dsp_param.parameters->float32_type[1] * jack_samplerate);
  }
  
  outsample = math_modules_delay_simple(&delay_simple->module->dsp_param,
                                              jack_samplerate,
                                              pos);
  
  /* Drive audio outputs */
  delay_simple->outs->sample->value = outsample;
  
} /* dsp_delay_simple */


void dsp_edit_delay_simple(struct dsp_module *delay_simple,
                            float amount,
                            float time,
                            float feedback
                           ) {
  printf("about to assign amount\n");
  delay_simple->dsp_param.parameters->float32_type[0] = amount;
  printf("assigned delay_simple->dsp_param.parameters->float32_type[0]: %f\n",
	 delay_simple->dsp_param.parameters->float32_type[0]);
  printf("about to assign time\n");
  delay_simple->dsp_param.parameters->float32_type[1] = time;
  delay_simple->dsp_param.parameters->int32_type[0] = (int)(time * jackcli_samplerate);
  printf("assigned delay_simple->dsp_param.parameters->float32_type[1]: %f\n",
	 delay_simple->dsp_param.parameters->float32_type[1]);
  printf("about to assign reset\n");
  delay_simple->dsp_param.parameters->float32_type[2] = feedback;
  printf("assigned delay_simple->dsp_param.parameters->float32_type[2]: %f\n",
	 delay_simple->dsp_param.parameters->float32_type[2]);  

  printf("returning\n");
  
} /* dsp_edit_delay_simple */
