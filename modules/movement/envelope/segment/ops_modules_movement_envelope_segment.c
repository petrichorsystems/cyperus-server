/* ops_modules_movement_envelope_segment.c
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

#include "math_modules_movement_envelope_segment.h"
#include "ops_modules_movement_envelope_segment.h"

int
dsp_create_movement_envelope_segment(struct dsp_bus *target_bus,
                                     int gate,
                                     float level,
                                     float time,
                                     float curve,
                                     float mul,
                                     float add) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "envelope_segment";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));
    printf("malloc() int8\n");
  params.parameters->int8_type = malloc(sizeof(int) * 5);
  printf("malloc() float32\n");
  params.parameters->float32_type = malloc(sizeof(float) * 8);
  
  /* user-facing parameters
  params.parameters->float32_types[0] = gate
  params.parameters->float32_types[1] = level_scale;
  params.parameters->float32_types[2] = time_scale;
  params.parameters->float32_types[3] = done_action;
  params.parameters->float32_types[4] = init_level;
  params.parameters->float32_types[5] = num_stages;

  params.parameters->int8_types[0] = release_node;
  params.parameters->int8_types[1] = loop_node;
  params.parameters->int8_types[2] = node_offset;

  params.parameters->float32_type[6] = mul;
  params.parameters->float32_type[7] = add; */
  
  /* internal parameters
  params.parameters->double_type[0] = a1;
  params.parameters->double_type[1] = a2;
  params.parameters->double_type[2] = b1;
  params.parameters->double_type[3] = y1;
  params.parameters->double_type[4] = y2;
  params.parameters->double_type[5] = grow;
  params.parameters->double_type[6] = end_level;

  params.parameters->int8_type[8] = counter;
  params.parameters->int8_type[9] = stage;
  params.parameters->int8_type[10] = shape;
  params.parameters->int8_type[11] = release_node;

  params.parameters->float32_type[8] = prev_gate;
    
   */

  
  math_modules_movement_envelope_segment_init(params.parameters,
                                              gate,
                                              level,
                                              time * (float)jackcli_samplerate,                                              
                                              curve,
                                              mul,
                                              add);
  
  ins = dsp_port_in_init("param_frequency", 512);
  ins->next = dsp_port_in_init("param_pulse_width", 512);
  ins->next->next = dsp_port_in_init("param_mul", 512);
  ins->next->next->next = dsp_port_in_init("param_add", 512);  
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "envelope_segment",
		 dsp_movement_envelope_segment,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_movement_envelope_segment */

void
dsp_movement_envelope_segment(struct dsp_operation *envelope_segment, int jack_samplerate, int pos) {
  float insample = 0.0f;
  float outsample = 0.0f;

  outsample = math_modules_movement_envelope_segment(envelope_segment->module->dsp_param.parameters,
                                                  jack_samplerate,
                                                  pos);
  /* drive audio outputs */
  envelope_segment->outs->sample->value = outsample;
} /* dsp_movement_envelope_segment */


void
dsp_edit_movement_envelope_segment(struct dsp_module *envelope_segment,
                                   int gate,
                                   float level,
                                   float time,
                                   float curve,
                                   float mul,
                                   float add) {
  
  math_modules_movement_envelope_segment_edit(envelope_segment->dsp_param.parameters,
                                              gate,
                                              level
                                              time * (float)jackcli_samplerate,
                                              curve,
                                              mul,
                                              add);
  
  printf("dsp_edit_movement_envelope_segment::returning\n");
} /* dsp_edit_movement_envelope_segment */
