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
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));  
  params.parameters->float32_type = malloc(sizeof(float) * 10);
  
  /* user-facing parameters */
  params.parameters->float32_type[0] = frequency;
  params.parameters->float32_type[1] = q; 
  params.parameters->float32_type[2] = amount;

  /* internal parameters */
  params.parameters->float32_type[3] = 0.0f; /* frequency_old */
  params.parameters->float32_type[4] = 0.0f; /* q_old */
  params.parameters->float32_type[5] = 0.0f; /* last */
  params.parameters->float32_type[6] = 0.0f; /* prev */
  params.parameters->float32_type[7] = 0.0f; /* coef1 */
  params.parameters->float32_type[8] = 0.0f; /* coef2 */
  params.parameters->float32_type[9] = 0.0f; /* gain */

  math_modules_filter_bandpass_init(&params);
  
  ins = dsp_port_in_init("in", 512, NULL);
  ins->next = dsp_port_in_init("param_frequency", 512, &(params.parameters->float32_type[0]));
  ins->next->next = dsp_port_in_init("param_q", 512, &(params.parameters->float32_type[1]));
  ins->next->next = dsp_port_in_init("param_amount", 512, &(params.parameters->float32_type[2]));  

  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "filter_bandpass",
		 dsp_filter_bandpass,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_filter_bandpass */

void
dsp_filter_bandpass(struct dsp_operation *filter_bandpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;

  insample = dsp_sum_summands(filter_bandpass->ins->summands);
  filter_bandpass->module->dsp_param.in = insample;

  /*
  if( filter_bandpass->ins->summands != NULL ) {
    dsp_param.filter_bandpass = dsp_sum_summands(filter_bandpass->ins->summands) * jack_samplerate;
  }

  if( filter_bandpass->ins->next->summands != NULL ) {
    dsp_param.filter_bandpass = dsp_sum_summands(filter_bandpass->ins->next->summands) * jack_samplerate;
  }

  if( filter_bandpass->ins->next->next->summands != NULL ) {
    dsp_param.filter_bandpass = dsp_sum_summands(filter_bandpass->ins->next->next->summands) * jack_samplerate;
  }

  if( filter_bandpass->ins->next->next->next->summands != NULL ) {
    dsp_param.filter_bandpass = dsp_sum_summands(filter_bandpass->ins->next->next->next->summands) * jack_samplerate;
  }
  */
  
  outsample = math_modules_filter_bandpass(&filter_bandpass->module->dsp_param,
                                               jack_samplerate,
                                               pos);
  
  /* drive audio outputs */
  filter_bandpass->outs->sample->value = outsample;
  
} /* dsp_filter_bandpass */


void dsp_edit_filter_bandpass(struct dsp_module *filter_bandpass,
                              float frequency,
                              float q,
                              float amount) {
  printf("about to assign frequency\n");
  filter_bandpass->dsp_param.parameters->float32_type[0] = frequency;
  printf("assigned filter_bandpass->dsp_param.parameters->float32_type[0]: %f\n",
	 filter_bandpass->dsp_param.parameters->float32_type[0]);
  printf("about to assign q\n");
  filter_bandpass->dsp_param.parameters->float32_type[1] = q;
  printf("assigned filter_bandpass->dsp_param.parameters->float32_type[1]: %f\n",
	 filter_bandpass->dsp_param.parameters->float32_type[1]);
  printf("about to assign amount\n");
  filter_bandpass->dsp_param.parameters->float32_type[2] = amount;
  printf("assigned filter_bandpass->dsp_param.parameters->float32_type[2]: %f\n",
	 filter_bandpass->dsp_param.parameters->float32_type[2]);  

  printf("returning\n");
  
} /* dsp_edit_filter_bandpass */
