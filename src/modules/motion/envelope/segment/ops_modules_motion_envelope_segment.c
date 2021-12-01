/* ops_modules_motion_envelope_segment.c
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

#include "math_modules_motion_envelope_segment.h"
#include "ops_modules_motion_envelope_segment.h"

int
dsp_create_motion_envelope_segment(struct dsp_bus *target_bus,
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
                                     int num_stages) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "envelope_segment";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));
  
  params.parameters->float32_arr_type = malloc(sizeof(float*) * 4);
  params.parameters->float32_type = malloc(sizeof(float) * 8);
  params.parameters->double_type = malloc(sizeof(double) * 7);
  params.parameters->int8_type = malloc(sizeof(int) * 10);

  printf("about to assign envelope parameters\n");
  
  /* envelope parameters */  
  params.parameters->float32_arr_type[PARAM_LEVELS] = malloc(sizeof(*levels));
  memcpy(&params.parameters->float32_arr_type[PARAM_LEVELS], &levels, sizeof(params.parameters->float32_arr_type[PARAM_LEVELS]));
  
  params.parameters->float32_arr_type[PARAM_TIMES] = malloc(sizeof(times));
  memcpy(&params.parameters->float32_arr_type[PARAM_TIMES], &times, sizeof(params.parameters->float32_arr_type[PARAM_TIMES]));

  printf("about to assign optional envelope parameters\n");
  
  if(shape) {
    params.parameters->float32_arr_type[PARAM_SHAPE] = malloc(sizeof(shape));
    memcpy(&params.parameters->float32_arr_type[PARAM_SHAPE], &shape, sizeof(params.parameters->float32_arr_type[PARAM_SHAPE]));
  } else
    params.parameters->float32_arr_type[PARAM_SHAPE] = NULL;
  
  if(curve) {
    params.parameters->float32_arr_type[PARAM_CURVE] = malloc(sizeof(params.parameters->float32_arr_type[PARAM_CURVE]));
    memcpy(&params.parameters->float32_arr_type[PARAM_CURVE], &curve, sizeof(curve));
  } else
    params.parameters->float32_arr_type[PARAM_CURVE] = NULL;
  
  params.parameters->int8_type[PARAM_RELEASE_NODE] = release_node;  
  params.parameters->int8_type[PARAM_LOOP_NODE] = loop_node;
  params.parameters->int8_type[PARAM_OFFSET] = offset;

  /* envelope generator parameters */  
  params.parameters->float32_type[PARAM_GATE]         = gate;
  params.parameters->float32_type[PARAM_LEVEL_SCALE]  = level_scale;
  params.parameters->float32_type[PARAM_LEVEL_BIAS]   = level_bias;
  params.parameters->float32_type[PARAM_TIME_SCALE]   = time_scale;
  params.parameters->float32_type[PARAM_INIT_LEVEL]   = 0.0f; /* init_level */
  params.parameters->int8_type[PARAM_NUM_STAGES]      = num_stages;

  printf("about to assign internal parameters\n");
  
  /* internal parameters */
  params.parameters->double_type[STATE_A1]        = 0.0; /* a1; */
  params.parameters->double_type[STATE_A2]        = 0.0; /* a2; */
  params.parameters->double_type[STATE_B1]        = 0.0; /* b1; */
  params.parameters->double_type[STATE_Y1]        = 0.0; /* y1; */
  params.parameters->double_type[STATE_Y2]        = 0.0; /* y2; */
  params.parameters->double_type[STATE_GROW]      = 0.0; /* grow; */
  params.parameters->double_type[STATE_END_LEVEL] = 0.0; /* end_level; */

  printf("about to assign internal parameters a\n");
  
  params.parameters->int8_type[STATE_COUNTER]      = 0; /* counter; */
  params.parameters->int8_type[STATE_STAGE]        = 0; /* stage; */
  params.parameters->int8_type[STATE_SHAPE]        = 0; /* shape; */
  params.parameters->int8_type[STATE_RELEASED]     = 0; /* released; */
  params.parameters->int8_type[STATE_DONE]         = 0; /* done; */

  printf("about to assign internal parameters b\n");
  
  params.parameters->float32_type[STATE_LEVEL]     = 0.0f; /* level; */
  params.parameters->float32_type[STATE_PREV_GATE] = 0.0f; /* prev_gate; */

  printf("about to call init()\n");
  
  math_modules_motion_envelope_segment_init(params.parameters);

  printf("instantiated\n");
  
  ins = dsp_port_in_init("param_gate", 512, &(params.parameters->float32_type[PARAM_GATE]));
  ins->next = dsp_port_in_init("param_level_scale", 512, &(params.parameters->float32_type[PARAM_LEVEL_SCALE]));
  ins->next->next = dsp_port_in_init("param_level_time_scale", 512, &(params.parameters->float32_type[PARAM_TIME_SCALE]));
  ins->next->next->next = dsp_port_in_init("param_init_level", 512, &(params.parameters->float32_type[PARAM_INIT_LEVEL]));  
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "envelope_segment",
		 dsp_motion_envelope_segment,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);

  printf("dsp_create_motion_envelope_segment::dsp_add_module()\n");
  return 0;
} /* dsp_create_motion_envelope_segment */

void
dsp_motion_envelope_segment(struct dsp_operation *envelope_segment, int jack_samplerate, int pos) {
  float insample = 0.0f;
  float outsample = 0.0f;

  outsample = math_modules_motion_envelope_segment(envelope_segment->module->dsp_param.parameters,
                                                  jack_samplerate,
                                                  pos);
  /* printf("outsample: %f\n", outsample); */
  /* drive audio outputs */
  envelope_segment->outs->sample->value = outsample;
} /* dsp_motion_envelope_segment */


void
dsp_edit_motion_envelope_segment(struct dsp_module *envelope_segment,
                                   int release_node,
                                   int loop_node,
                                   int offset,
                                   float gate,
                                   float level_scale,
                                   float level_bias,
                                   float time_scale) {

  /* printf("ops_modules_motion_envelope_segment.c::dsp_edit_motion_envelope_segment()\n"); */
  /* printf("release_node: %d\n", release_node); */
  /* printf("loop_node: %d\n", loop_node); */
  /* printf("offset: %d\n", offset); */
  /* printf("gate: %f\n", gate); */
  /* printf("level_scale: %f\n", level_scale); */
  /* printf("level_bias: %f\n", level_bias); */
  /* printf("time_scale: %f\n", time_scale); */
  /* printf("\n\n"); */
  math_modules_motion_envelope_segment_edit(envelope_segment->dsp_param.parameters,
                                              release_node,
                                              loop_node,
                                              offset,
                                              gate,
                                              level_scale,
                                              level_bias,
                                              time_scale);

  printf("dsp_edit_motion_envelope_segment::returning\n");
} /* dsp_edit_motion_envelope_segment */
