 /* ops_modules_motion_envelope_follower.c
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

#include "math_modules_motion_envelope_follower.h"
#include "ops_modules_motion_envelope_follower.h"

int
dsp_create_motion_envelope_follower(struct dsp_bus *target_bus,
                                    float attack,
                                    float decay,
                                    float scale) {
  dsp_parameter envelope_follower_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  envelope_follower_param.name = "envelope_follower";  
  envelope_follower_param.pos = 0;  
  envelope_follower_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  printf("malloc() float32\n");
  envelope_follower_param.parameters->float32_type = malloc(sizeof(float) * 4);
  
  /* user-facing parameters */
  envelope_follower_param.parameters->float32_type[0] = attack; 
  envelope_follower_param.parameters->float32_type[1] = decay;
  envelope_follower_param.parameters->float32_type[2] = scale;
  
  /* internal parameters */
  envelope_follower_param.parameters->float32_type[3] = 0.0f; /* last output sample */
  
  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "envelope_follower",
		 dsp_motion_envelope_follower,
		 dsp_optimize_module,
		 envelope_follower_param,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_motion_envelope_follower */

void
dsp_motion_envelope_follower(struct dsp_operation *envelope_follower, int jack_samplerate, int pos) {
  float insample = 0.0f;
  float outsample = 0.0f;

  insample = dsp_sum_summands(envelope_follower->ins->summands);
  envelope_follower->module->dsp_param.in = insample;

  outsample = math_modules_motion_envelope_follower(&envelope_follower->module->dsp_param,
                                                  jack_samplerate,
                                                  pos);
  /* drive audio outputs */
  envelope_follower->outs->sample->value = outsample;
} /* dsp_motion_envelope_follower */


void
dsp_edit_motion_envelope_follower(struct dsp_module *envelope_follower,
                                  float attack,
                                  float decay,
                                  float scale) {
  envelope_follower->dsp_param.parameters->float32_type[0] = attack;
  printf("assigned envelope_follower->dsp_param.parameters->float32_type[0]: %f\n",
	 envelope_follower->dsp_param.parameters->float32_type[0]);
  
  envelope_follower->dsp_param.parameters->float32_type[1] = decay;
  printf("assigned envelope_follower->dsp_param.parameters->float32_type[1]: %f\n",
	 envelope_follower->dsp_param.parameters->float32_type[1]);
  
  envelope_follower->dsp_param.parameters->float32_type[2] = scale;
  printf("assigned envelope_follower->dsp_param.parameters->float32_type[2]: %f\n",
	 envelope_follower->dsp_param.parameters->float32_type[2]);
    
  printf("dsp_edit_motion_envelope_follower::returning\n");
} /* dsp_edit_motion_envelope_follower */
