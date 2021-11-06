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
                                     float *levels,
                                     float *times,
                                     float *shape,
                                     float *curve,
                                     int release_node,
                                     int loop_node,
                                     int offset,
                                     float gate,
                                     float level_scale,
                                     float level_bias,
                                     float time_scale,
                                     float init_level,
                                     int num_stages) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "envelope_segment";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));

  printf("hi0\n");

  printf("hi1\n");
  
  env_gen_params_t *envelope_gen = (env_gen_params_t*)malloc(sizeof(env_gen_params_t));
  
  
  printf("hi2\n");
  
  envelope_gen->envelope = malloc(sizeof(env_params_t));

  printf("hello 0\n");
  
  /* envelope parameters */
  envelope_gen->envelope->levels = malloc(sizeof(levels)); //malloc(sizeof(levels));

  printf("hello 1\n");
  envelope_gen->envelope->levels=malloc(sizeof(levels));
  memcpy(envelope_gen->envelope->levels, &levels, sizeof(levels));
  envelope_gen->envelope->times = malloc(sizeof(times));
  memcpy(envelope_gen->envelope->times, &times, sizeof(times));

  printf("about to hit conditionals\n");
  
  if(shape) {
    envelope_gen->envelope->shape = malloc(sizeof(shape));
    memcpy(envelope_gen->envelope->shape, &shape, sizeof(shape));
  } else
    envelope_gen->envelope->shape = NULL;
  
  if(curve) {
    envelope_gen->envelope->curve = malloc(sizeof(curve));
    memcpy(envelope_gen->envelope->curve, &curve, sizeof(curve));
  } else
    envelope_gen->envelope->curve = NULL;

  printf("hello2\n");
  
  envelope_gen->envelope->release_node = release_node;  
  envelope_gen->envelope->loop_node = loop_node;
  envelope_gen->envelope->offset = offset;

  printf("hello3\n");

  /* envelope generator parameters */  
  envelope_gen->gate = gate;
  envelope_gen->level_scale = level_scale;
  envelope_gen->level_bias = level_bias;
  envelope_gen->time_scale = time_scale;
  envelope_gen->release_node = release_node;
  envelope_gen->init_level = init_level;  
  envelope_gen->num_stages = num_stages;

  printf("dsp_create_movement_envelope_segment::assigned envelope\n");
  
  params.parameters->bytes_type = (void *)malloc(sizeof(env_gen_params_t));
  /* copy envelope_gen */
  math_modules_movement_envelope_segment_init(params.parameters);
  
  ins = dsp_port_in_init("param_gate", 512);
  ins->next = dsp_port_in_init("param_level_scale", 512);
  ins->next->next = dsp_port_in_init("param_level_time_scale", 512);
  ins->next->next->next = dsp_port_in_init("param_init_level", 512);  
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
                                   float gate,
                                   float level_scale,
                                   float level_bias,
                                   float time_scale,
                                   float init_level,
                                   int num_stages) {
  
  math_modules_movement_envelope_segment_edit(envelope_segment->dsp_param.parameters,
                                              gate,
                                              level_scale,
                                              level_bias,
                                              time_scale,
                                              init_level,
                                              num_stages);  
  printf("dsp_edit_movement_envelope_segment::returning\n");
} /* dsp_edit_movement_envelope_segment */
