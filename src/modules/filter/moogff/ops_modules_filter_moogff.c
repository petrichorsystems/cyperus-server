/* ops_modules_filter_moogff.c
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

#include "math_modules_filter_moogff.h"
#include "ops_modules_filter_moogff.h"

int
dsp_create_filter_moogff(struct dsp_bus *target_bus,
                         float frequency,
                         float gain,
                         float reset,
                         float mul,
                         float add
                         ) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "filter_moogff";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));  
  params.parameters->float32_type = malloc(sizeof(float) * 6);
  
  /* user-facing parameters */
  params.parameters->float32_type[0] = frequency;
  params.parameters->float32_type[1] = gain; 
  params.parameters->float32_type[2] = reset; 
  params.parameters->float32_type[3] = mul;
  params.parameters->float32_type[4] = add;

  /* internal parameters */
  params.parameters->float32_type[4] = 0.0f; /* m_freq */
  params.parameters->float32_type[5] = gain; /* m_k */

  params.parameters->double_type = malloc(sizeof(double) * 6);

  params.parameters->double_type[0] = 0.0f; /* m_b0 - resonant freq*/
  params.parameters->double_type[1] = 0.0f; /* m_a1 - corresponding value */

  params.parameters->double_type[2] = 0.0f; /* m_s1 - internal parameters, 1st order filter states */
  params.parameters->double_type[3] = 0.0f; /* m_s2 */
  params.parameters->double_type[4] = 0.0f; /* m_s3 */
  params.parameters->double_type[5] = 0.0f; /* m_s4 */

  ins = dsp_port_in_init("in", 512, NULL);
  ins->next = dsp_port_in_init("param_frequency", 512, &(params.parameters->float32_type[0]));
  ins->next->next = dsp_port_in_init("param_gain", 512, &(params.parameters->float32_type[1]));
  ins->next->next = dsp_port_in_init("param_reset", 512, &(params.parameters->float32_type[2]));  
  ins->next->next->next = dsp_port_in_init("param_mul", 512, &(params.parameters->float32_type[3]));
  ins->next->next->next->next = dsp_port_in_init("param_add", 512, &(params.parameters->float32_type[4]));  
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "filter_moogff",
		 dsp_filter_moogff,
                 NULL,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_filter_moogff */

void
dsp_filter_moogff(struct dsp_operation *filter_moogff, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;

  insample = dsp_sum_summands(filter_moogff->ins->summands);
  filter_moogff->module->dsp_param.in = insample;

  if( filter_moogff->ins->next->summands != NULL ) {

    /* come back to this -- we need to figure out how to perform input calculations
       with the large coefficient calculations. */
    
    /* dsp_param.filter_moogff- = dsp_sum_summands(filter_moogff->ins->next->summands) * jack_samplerate; */    
  }
  
  outsample = math_modules_filter_moogff(&filter_moogff->module->dsp_param,
                                         jack_samplerate,
                                         pos);
  
  /* drive audio outputs */
  filter_moogff->outs->sample->value = outsample;
  
} /* dsp_filter_moogff */


void dsp_edit_filter_moogff(struct dsp_module *filter_moogff,
                            float frequency,
                            float gain,
                            float reset,
                            float mul,
                            float add) {
  printf("about to assign frequency\n");
  filter_moogff->dsp_param.parameters->float32_type[0] = frequency;
  printf("assigned filter_moogff->dsp_param.parameters->float32_type[0]: %f\n",
	 filter_moogff->dsp_param.parameters->float32_type[0]);
  printf("about to assign gain\n");
  filter_moogff->dsp_param.parameters->float32_type[1] = gain;
  printf("assigned filter_moogff->dsp_param.parameters->float32_type[1]: %f\n",
	 filter_moogff->dsp_param.parameters->float32_type[1]);
  printf("about to assign reset\n");
  filter_moogff->dsp_param.parameters->float32_type[2] = reset;
  printf("assigned filter_moogff->dsp_param.parameters->float32_type[2]: %f\n",
	 filter_moogff->dsp_param.parameters->float32_type[2]);  
  printf("about to assign mul\n");
  filter_moogff->dsp_param.parameters->float32_type[3] = mul;
  printf("assigned filter_moogff->dsp_param.parameters->float32_type[3]: %f\n", filter_moogff->dsp_param.parameters->float32_type[3]);
  printf("about to assign add\n");
  filter_moogff->dsp_param.parameters->float32_type[4] = add;
  printf("assigned filter_moogff->dsp_param.parameters->float32_type[4]: %f\n", filter_moogff->dsp_param.parameters->float32_type[4]);

  printf("returning\n");
  
} /* dsp_edit_filter_moogff */
