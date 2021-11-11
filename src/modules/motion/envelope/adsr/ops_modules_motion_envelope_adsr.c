/* ops_modules_motion_envelope_adsr.c
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

#include "math_modules_motion_envelope_adsr.h"
#include "ops_modules_motion_envelope_adsr.h"

int
dsp_create_motion_envelope_adsr(struct dsp_bus *target_bus,
                                  int gate,
                                  float attack_rate,
                                  float decay_rate,
                                  float release_rate,
                                  float sustain_level,
                                  float target_ratio_a,
                                  float target_ratio_dr,
                                  float mul,
                                  float add) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "envelope_adsr";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));
  printf("malloc() float32\n");
  params.parameters->float32_type = malloc(sizeof(float) * 15);
  printf("malloc() int8\n");
  params.parameters->int8_type = malloc(sizeof(int) * 3);
  
  /* user-facing parameters
  params.parameters->int8_type[0] = gate;
  params.parameters->float32_type[0] = attack_rate; 
  params.parameters->float32_type[1] = decay_rate;
  params.parameters->float32_type[2] = release_rate;
  params.parameters->float32_type[3] = sustain_level;
  params.parameters->float32_type[4] = target_ratio_a;
  params.parameters->float32_type[5] = target_ratio_dr;
  params.parameters->float32_type[6] = mul;
  params.parameters->float32_type[7] = add; */
  
  /* internal parameters
  params.parameters->int8_type[1] = state
  params.parameters->int8_type[2] = gate_state
  params.parameters->float32_type[8] = attack_coeff;
  params.parameters->float32_type[9] = decay_coeff;
  params.parameters->float32_type[10] = release_coeff;
  params.parameters->float32_type[11] = attack_base;
  params.parameters->float32_type[12] = decay_base;
  params.parameters->float32_type[13] = release_base; 
  params.parameters->float32_type[14] = last_output_value; */

  
  math_modules_motion_envelope_adsr_init(params.parameters,
                                           attack_rate * (float)jackcli_samplerate,
                                           decay_rate * (float)jackcli_samplerate,
                                           release_rate * (float)jackcli_samplerate,
                                           sustain_level,
                                           target_ratio_a,
                                           target_ratio_dr,
                                           mul,
                                           add);
  
  ins = dsp_port_in_init("param_frequency", 512);
  ins->next = dsp_port_in_init("param_pulse_width", 512);
  ins->next->next = dsp_port_in_init("param_mul", 512);
  ins->next->next->next = dsp_port_in_init("param_add", 512);  
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "envelope_adsr",
		 dsp_motion_envelope_adsr,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_motion_envelope_adsr */

void
dsp_motion_envelope_adsr(struct dsp_operation *envelope_adsr, int jack_samplerate, int pos) {
  float insample = 0.0f;
  float outsample = 0.0f;

  outsample = math_modules_motion_envelope_adsr(envelope_adsr->module->dsp_param.parameters,
                                                  jack_samplerate,
                                                  pos);
  /* drive audio outputs */
  envelope_adsr->outs->sample->value = outsample;
} /* dsp_motion_envelope_adsr */


void
dsp_edit_motion_envelope_adsr(struct dsp_module *envelope_adsr,
                                int gate,
                                float attack_rate,
                                float decay_rate,
                                float release_rate,
                                float sustain_level,
                                float target_ratio_a,
                                float target_ratio_dr,
                                float mul,
                                float add) {
  
  math_modules_motion_envelope_adsr_edit(envelope_adsr->dsp_param.parameters,
                                           gate,
                                           attack_rate * (float)jackcli_samplerate,
                                           decay_rate * (float)jackcli_samplerate,
                                           release_rate * (float)jackcli_samplerate,
                                           sustain_level,
                                           target_ratio_a,
                                           target_ratio_dr,
                                           mul,
                                           add);
  
  printf("dsp_edit_motion_envelope_adsr::returning\n");
} /* dsp_edit_motion_envelope_adsr */
