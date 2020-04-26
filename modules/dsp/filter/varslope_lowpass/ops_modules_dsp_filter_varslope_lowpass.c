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

#include "jackcli.h"
#include "cyperus.h"
#include "dsp_math.h"
#include "dsp.h"
#include "dsp_ops.h"

int
dsp_create_filter_varslope_lowpass(struct dsp_bus *target_bus, float amt, float time, float feedback) {
  dsp_parameter filter_varslope_lowpass_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  filter_varslope_lowpass_param.type = DSP_FILTER_VARSLOPE_LOWPASS_PARAMETER_ID;
  filter_varslope_lowpass_param.pos = 0;
  filter_varslope_lowpass_param.filter_varslope_lowpass.name = "filter_varslope_lowpass";
  filter_varslope_lowpass_param.filter_varslope_lowpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filter_varslope_lowpass_param.filter_varslope_lowpass.amt = amt;
  filter_varslope_lowpass_param.filter_varslope_lowpass.time = time * jackcli_samplerate;
  filter_varslope_lowpass_param.filter_varslope_lowpass.feedback = feedback;
  filter_varslope_lowpass_param.filter_varslope_lowpass.cyperus_params[0].signal_buffer = (float *)calloc(time * jackcli_samplerate * 30, sizeof(float));

  filter_varslope_lowpass_param.filter_varslope_lowpass.cyperus_params[0].pos = 0;
  filter_varslope_lowpass_param.filter_varslope_lowpass.cyperus_params[0].filter_varslope_lowpass_pos = 0;
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_time", 512);
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
  dsp_parameter dsp_param = filter_varslope_lowpass->module->dsp_param;

  /* sum audio */
  insample = dsp_sum_summands(filter_varslope_lowpass->ins->summands);
  filter_varslope_lowpass->module->dsp_param.filter_varslope_lowpass.cyperus_params->in = insample;
  
  /* set initial filter_varslope_lowpass amount */

  /* set filter_varslope_lowpass time if we have incoming data for that input */
  if( filter_varslope_lowpass->ins->next->summands != NULL )
    dsp_param.filter_varslope_lowpass.time = dsp_sum_summands(filter_varslope_lowpass->ins->next->summands) * jack_samplerate;

  filter_varslope_lowpass->module->dsp_param.filter_varslope_lowpass.cyperus_params->filter_varslope_lowpass_amt = dsp_param.filter_varslope_lowpass.amt;
  filter_varslope_lowpass->module->dsp_param.filter_varslope_lowpass.cyperus_params->filter_varslope_lowpass_time = dsp_param.filter_varslope_lowpass.time;
  filter_varslope_lowpass->module->dsp_param.filter_varslope_lowpass.cyperus_params->fb = dsp_param.filter_varslope_lowpass.feedback;

  outsample = cyperus_filter_varslope_lowpass(filter_varslope_lowpass->module->dsp_param.filter_varslope_lowpass.cyperus_params,
			    jack_samplerate, pos);

  /* drive audio outputs */
  filter_varslope_lowpass->outs->sample->value = outsample;

  return;
} /* dsp_filter_varslope_lowpass */


void dsp_edit_filter_varslope_lowpass(struct dsp_module *filter_varslope_lowpass, float amt, float time, float feedback) {
  int i = 0;

  dsp_parameter dsp_param = filter_varslope_lowpass->dsp_param;
  
  printf("about to assign amt\n");
  filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.amt = amt;
  printf("assigned filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.amt: %f\n", filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.amt);
  printf("about to assign time\n");
  filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.time = time * jackcli_samplerate;
  printf("assigned filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.time: %f\n", filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.time);
  printf("about to assign feedback\n");
  filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.feedback = feedback;
  printf("assigned filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.feedback: %f\n", filter_varslope_lowpass->dsp_param.filter_varslope_lowpass.feedback);
  
  /*
    dsp_voice_parameters[module_no].filter_varslope_lowpass.cyperus_params[0].pos = 0;
    dsp_voice_parameters[module_no].filter_varslope_lowpass.cyperus_params[0].filter_varslope_lowpass_pos = 0;
  */

  printf("returning\n");
  
} /* dsp_edit_filter_varslope_lowpass */
