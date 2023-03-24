/* ops_modules_envelope_segment.c
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

Copyright 2023 murray foster */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "math_modules_envelope_segment.h"
#include "ops_modules_envelope_segment.h"

int
dsp_create_envelope_segment(struct dsp_bus *target_bus,
                            float rate,
                            char *shape,
                            float min,
                            float max,
                            float auto_reset) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "envelope_segment";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));  
  params.parameters->float32_type = malloc(sizeof(float) * 14);
  params.parameters->int32_type = malloc(sizeof(int) * 2);

  /* user-facing parameters */
  params.parameters->float32_type[0] = rate;  
  params.parameters->float32_type[1] = min;
  params.parameters->float32_type[2] = max;
  params.parameters->float32_type[3] = auto_reset;

  /* internal parameters */
  
  /* osc listener parameters */
  params.parameters->int32_type[1] = 0; /* samples waited */
  params.parameters->int32_type[2] = (int)((float)jackcli_samplerate / 60.0f); /* listener hz */

                                            
  /* osc listener param state parameters */                                            
  
  ins = dsp_port_in_init("trigger", 512, NULL);
  /* ins->next = dsp_port_in_init("param_reset", 512, &(params.parameters->float32_type[0])); */
  /* ins->next->next = dsp_port_in_init("param_start", 512, &(params.parameters->float32_type[1])); */
  /* ins->next->next->next = dsp_port_in_init("param_step_size", 512, &(params.parameters->float32_type[2])); */
  /* ins->next->next->next->next = dsp_port_in_init("param_min", 512, &(params.parameters->float32_type[3])); */
  /* ins->next->next->next->next->next = dsp_port_in_init("param_max", 512, &(params.parameters->float32_type[4])); */
  /* ins->next->next->next->next->next->next = dsp_port_in_init("param_direction", 512, &(params.parameters->float32_type[5])); */
  /* ins->next->next->next->next->next->next->next = dsp_port_in_init("param_auto_reset", 512, &(params.parameters->float32_type[6])); */
  
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "envelope_segment",
		 dsp_envelope_segment,
                 dsp_osc_listener_envelope_segment,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_envelope_segment */

void
dsp_envelope_segment(struct dsp_operation *envelope_segment, int jack_samplerate, int pos) {
  float trigger, reset, out = 0.0f;
  
  /* input trigger */
  trigger = dsp_sum_summands(envelope_segment->ins->summands);
  envelope_segment->module->dsp_param.in = trigger;

  /* reset trigger */
  if( envelope_segment->ins->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[0] = dsp_sum_summands(envelope_segment->ins->next->summands);
  }
  
  /* start value */
  if( envelope_segment->ins->next->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[1] = dsp_sum_summands(envelope_segment->ins->next->next->summands);
  }

  /* step_size */
  if( envelope_segment->ins->next->next->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[2] = dsp_sum_summands(envelope_segment->ins->next->next->next->summands);
  }

  /* min */
  if( envelope_segment->ins->next->next->next->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[3] = dsp_sum_summands(envelope_segment->ins->next->next->next->next->summands);
  }

  /* max */
  if( envelope_segment->ins->next->next->next->next->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[4] = dsp_sum_summands(envelope_segment->ins->next->next->next->next->next->summands);
  }   

  /* direction */
  if( envelope_segment->ins->next->next->next->next->next->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[5] = dsp_sum_summands(envelope_segment->ins->next->next->next->next->next->next->summands);
  }

  /* auto_reset */
  if( envelope_segment->ins->next->next->next->next->next->next->next->summands != NULL ) {
     envelope_segment->module->dsp_param.parameters->float32_type[6] = dsp_sum_summands(envelope_segment->ins->next->next->next->next->next->next->next->summands);
  }
  
  dsp_osc_listener_envelope_segment(envelope_segment, jack_samplerate, pos);
  
  out = math_modules_envelope_segment(&envelope_segment->module->dsp_param,
                                   jack_samplerate,
                                   pos);
  
  /* drive audio outputs */
  envelope_segment->outs->sample->value = out;
  
  return;
} /* dsp_envelope_segment */


void dsp_edit_envelope_segment(struct dsp_module *envelope_segment,
                            float reset,
                            float start,
                            float step_size,
                            float min,
                            float max,
                            float direction,
                            float auto_reset) {

  printf("about to assign reset\n");
  envelope_segment->dsp_param.parameters->float32_type[0] = reset;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[0]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[0]);       
  
  printf("about to assign start\n");
  envelope_segment->dsp_param.parameters->float32_type[1] = start;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[1]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[1]);       

  printf("about to assign step_size\n");
  envelope_segment->dsp_param.parameters->float32_type[2] = step_size;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[2]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[2]);

  printf("about to assign min\n");
  envelope_segment->dsp_param.parameters->float32_type[3] = min;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[3]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[3]);

  printf("about to assign max\n");
  envelope_segment->dsp_param.parameters->float32_type[4] = max;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[4]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[4]);

  printf("about to assign direction\n");
  envelope_segment->dsp_param.parameters->float32_type[5] = direction;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[5]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[5]);

  printf("about to assign auto_reset\n");
  envelope_segment->dsp_param.parameters->float32_type[6] = auto_reset;
  printf("assigned envelope_segment->dsp_param.parameters->float32_type[6]: %f\n",
         envelope_segment->dsp_param.parameters->float32_type[6]);

  printf("returning\n");
  
} /* dsp_edit_envelope_segment */


void
dsp_osc_listener_envelope_segment(struct dsp_operation *envelope_segment, int jack_samplerate, int pos) {

  unsigned short param_connected = 0;
  if( (envelope_segment->ins->next->summands != NULL) ||
      (envelope_segment->ins->next->next->summands != NULL) ||
      (envelope_segment->ins->next->next->next->summands != NULL) ||
      (envelope_segment->ins->next->next->next->next->summands != NULL) ||
      (envelope_segment->ins->next->next->next->next->next->summands != NULL) ||
      (envelope_segment->ins->next->next->next->next->next->next->summands != NULL) ||
      (envelope_segment->ins->next->next->next->next->next->next->next->summands != NULL) ) {
     param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* osc listener, 60hz */
     int samples_waited = envelope_segment->module->dsp_param.parameters->int32_type[1];
     int samples_to_wait = envelope_segment->module->dsp_param.parameters->int32_type[2];
     if( samples_waited == samples_to_wait - 1) {
       /* if new value is different than old value, send osc messages */
       if(
          envelope_segment->module->dsp_param.parameters->float32_type[0] != envelope_segment->module->dsp_param.parameters->float32_type[2] ||
          envelope_segment->module->dsp_param.parameters->float32_type[1] != envelope_segment->module->dsp_param.parameters->float32_type[3]
          ) {
         int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
         char *path = (char *)malloc(sizeof(char) * path_len);
         snprintf(path, path_len, "%s%s", "/cyperus/listener/", envelope_segment->module->id);    
         lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
         lo_send(lo_addr_send, path, "ff",
                 envelope_segment->module->dsp_param.parameters->float32_type[0],
                 envelope_segment->module->dsp_param.parameters->float32_type[1]);
         free(lo_addr_send);

         /* assign new parameter to last parameter after we're reported the change */
         envelope_segment->module->dsp_param.parameters->float32_type[2] = envelope_segment->module->dsp_param.parameters->float32_type[0];
         envelope_segment->module->dsp_param.parameters->float32_type[3] = envelope_segment->module->dsp_param.parameters->float32_type[1];
       }
       envelope_segment->module->dsp_param.parameters->int32_type[1] = 0;
     } else {
       envelope_segment->module->dsp_param.parameters->int32_type[1] += 1;
     } 
  }
  
  return;
} /* dsp_osc_listener_envelope_segment */
