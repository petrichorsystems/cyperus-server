/* dsp_ops.c
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

Copyright 2018 murray foster */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include <lo/lo.h>

#include "../../../../jackcli.h"
#include "../../../../cyperus.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp.h"
#include "../../../../dsp_ops.h"

#include "math_modules_audio_filter_varslope_lowpass.h"
#include "ops_modules_audio_filter_varslope_lowpass.h"

int
dsp_create_filter_varslope_lowpass(struct dsp_bus *target_bus,
				   float amplitude,
				   float slope,
				   float cutoff_frequency) {
  dsp_parameter filter_varslope_lowpass_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  filter_varslope_lowpass_param.name = "filter_varslope_lowpass";
  filter_varslope_lowpass_param.pos = 0;
  filter_varslope_lowpass_param.in = 0.0f;
  filter_varslope_lowpass_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  filter_varslope_lowpass_param.parameters->float32_type = malloc(sizeof(float) * 40);

  filter_varslope_lowpass_param.parameters->float32_type[0] = amplitude;
  filter_varslope_lowpass_param.parameters->float32_type[1] = slope;
  filter_varslope_lowpass_param.parameters->float32_type[2] = cutoff_frequency;
  filter_varslope_lowpass_param.parameters->float32_type[3] = cutoff_frequency / (jackcli_samplerate / 2.0f);

  math_modules_dsp_filter_varslope_lowpass_init(filter_varslope_lowpass_param.parameters, jackcli_samplerate);
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_cutoff", 512);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "filter_varslope_lowpass",
		 dsp_filter_varslope_lowpass,
		 dsp_optimize_module,
		 filter_varslope_lowpass_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_filter_varslope_lowpass */

void
dsp_filter_varslope_lowpass(struct dsp_operation *filter_varslope_lowpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;

  insample = dsp_sum_summands(filter_varslope_lowpass->ins->summands);
  filter_varslope_lowpass->module->dsp_param.in = insample;

  if( filter_varslope_lowpass->ins->next->summands != NULL ) {

    /* come back to this -- we need to figure out how to perform input calculations
       with the large coefficient calculations. */
    
    /* dsp_param.filter_varslope_lowpass- = dsp_sum_summands(filter_varslope_lowpass->ins->next->summands) * jack_samplerate; */
    
  }

  outsample = math_modules_dsp_filter_varslope_lowpass(&filter_varslope_lowpass->module->dsp_param,
						       jack_samplerate,
						       pos);
  
  /* drive audio outputs */
  filter_varslope_lowpass->outs->sample->value = outsample;

  return;
} /* dsp_filter_varslope_lowpass */


void dsp_edit_filter_varslope_lowpass(struct dsp_module *filter_varslope_lowpass,
				      float amplitude,
				      float slope,
				      float cutoff_frequency) {
  printf("about to assign amplitude\n");
  filter_varslope_lowpass->dsp_param.parameters->float32_type[0] = amplitude;
  printf("assigned filter_varslope_lowpass->dsp_param.parameters->float32_type[0]: %f\n",
	 filter_varslope_lowpass->dsp_param.parameters->float32_type[0]);
  printf("about to assign slope\n");
  filter_varslope_lowpass->dsp_param.parameters->float32_type[1] = slope;
  printf("assigned filter_varslope_lowpass->dsp_param.parameters->float32_type[1]: %f\n",
	 filter_varslope_lowpass->dsp_param.parameters->float32_type[1]);
  printf("about to assign cutoff_frequency\n");
  filter_varslope_lowpass->dsp_param.parameters->float32_type[2] = cutoff_frequency;
  printf("assigned filter_varslope_lowpass->dsp_param.parameters->float32_type[2]: %f\n", filter_varslope_lowpass->dsp_param.parameters->float32_type[2]);
  printf("about to assign fc\n");
  filter_varslope_lowpass->dsp_param.parameters->float32_type[3] = cutoff_frequency / (jackcli_samplerate / 2.0f);
  printf("assigned filter_varslope_lowpass->dsp_param.parameters->float32_type[3]: %f\n", filter_varslope_lowpass->dsp_param.parameters->float32_type[3]);

  math_modules_dsp_filter_varslope_lowpass_edit(filter_varslope_lowpass->dsp_param.parameters);

  printf("returning\n");
  
} /* dsp_edit_filter_varslope_lowpass */
