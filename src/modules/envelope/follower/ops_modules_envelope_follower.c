 /* ops_modules_envelope_follower.c
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

#include "math_modules_envelope_follower.h"
#include "ops_modules_envelope_follower.h"

int
dsp_create_envelope_follower(struct dsp_bus *target_bus,
                                    float attack,
                                    float decay,
                                    float scale) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "envelope_follower";  
  params.pos = 0;

  /* signal input */
  params.in = malloc(sizeof(float) * dsp_global_period);
  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));

  params.parameters->float32_type = malloc(sizeof(float) * 4);
  params.parameters->float32_arr_type = malloc(sizeof(float *) * 3);  

  /* user-facing parameter allocation */
  params.parameters->float32_arr_type[0] = calloc(dsp_global_period, sizeof(float));
  params.parameters->float32_arr_type[1] = calloc(dsp_global_period, sizeof(float));
  params.parameters->float32_arr_type[2] = calloc(dsp_global_period, sizeof(float));

  for(int p=0; p<dsp_global_period; p++) {
    /* user-facing parameter assignment */
    params.parameters->float32_arr_type[0][p] = attack; 
    params.parameters->float32_arr_type[1][p] = decay;
    params.parameters->float32_arr_type[2][p] = scale;
  }
  
  /* internal parameter assignment */
  params.parameters->float32_type[0] = 0.0f; /* last output sample */

  /* osc listener parameters */
  params.parameters->float32_type[1] = attack;
  params.parameters->float32_type[2] = decay;
  params.parameters->float32_type[3] = scale;
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_attack", 512);
  ins->next->next = dsp_port_in_init("param_decay", 512);
  ins->next->next->next = dsp_port_in_init("param_scale", 512);
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "envelope_follower",
		 dsp_envelope_follower,
                 dsp_osc_listener_envelope_follower,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);

  return 0;
} /* dsp_create_envelope_follower */

void
dsp_envelope_follower(struct dsp_operation *envelope_follower, int jack_samplerate) {
  float *outsamples;
  int p;

  dsp_sum_summands(envelope_follower->module->dsp_param.in, envelope_follower->ins->summands);

  /* handle params with connected inputs */
  if( envelope_follower->ins->next->summands != NULL ) { /* attack */
    dsp_sum_summands(envelope_follower->module->dsp_param.parameters->float32_arr_type[0], envelope_follower->ins->next->summands);
  }
  if( envelope_follower->ins->next->next->summands != NULL ) { /* decay */
    dsp_sum_summands(envelope_follower->module->dsp_param.parameters->float32_arr_type[1], envelope_follower->ins->next->next->summands);
  }
  if( envelope_follower->ins->next->next->next->summands != NULL ) { /* scale */
    dsp_sum_summands(envelope_follower->module->dsp_param.parameters->float32_arr_type[2], envelope_follower->ins->next->next->next->summands);
  }  

  outsamples = math_modules_envelope_follower(&envelope_follower->module->dsp_param,
                                             jack_samplerate);
  /* drive audio outputs */
  memcpy(envelope_follower->outs->sample->value,
         outsamples,
         sizeof(float) * dsp_global_period);

  free(outsamples);
} /* dsp_envelope_follower */


void
dsp_edit_envelope_follower(struct dsp_module *envelope_follower,
                           float attack,
                           float decay,
                           float scale) {
  for(int p=0; p<dsp_global_period; p++) {
    envelope_follower->dsp_param.parameters->float32_arr_type[0][p] = attack;  
    envelope_follower->dsp_param.parameters->float32_arr_type[1][p] = decay; 
    envelope_follower->dsp_param.parameters->float32_arr_type[2][p] = scale;
  }
} /* dsp_edit_envelope_follower */

void
dsp_osc_listener_envelope_follower(struct dsp_operation *envelope_follower, int jack_samplerate) {
  unsigned short param_connected = 0;
  if( (envelope_follower->ins->summands != NULL) ||
      (envelope_follower->ins->next->summands != NULL) ||
      (envelope_follower->ins->next->next->summands != NULL) ) {
    param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* if new value is different than old value, send osc messages */
    if(
       envelope_follower->module->dsp_param.parameters->float32_type[1] != envelope_follower->module->dsp_param.parameters->float32_arr_type[0][0] ||
       envelope_follower->module->dsp_param.parameters->float32_type[2] != envelope_follower->module->dsp_param.parameters->float32_arr_type[1][0] ||
       envelope_follower->module->dsp_param.parameters->float32_type[3] != envelope_follower->module->dsp_param.parameters->float32_arr_type[2][0]
       ) {
      int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
      char *path = (char *)malloc(sizeof(char) * path_len);
      snprintf(path, path_len, "%s%s", "/cyperus/listener/", envelope_follower->module->id);    

      lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
      lo_send(lo_addr_send, path, "fff",
              envelope_follower->module->dsp_param.parameters->float32_arr_type[0][0],
              envelope_follower->module->dsp_param.parameters->float32_arr_type[1][0],
              envelope_follower->module->dsp_param.parameters->float32_arr_type[2][0]);
      free(lo_addr_send);

      /* assign new parameter to last parameter after we're reported the change */
      envelope_follower->module->dsp_param.parameters->float32_type[1] = envelope_follower->module->dsp_param.parameters->float32_arr_type[0][0];
      envelope_follower->module->dsp_param.parameters->float32_type[2] = envelope_follower->module->dsp_param.parameters->float32_arr_type[1][0];
      envelope_follower->module->dsp_param.parameters->float32_type[3] = envelope_follower->module->dsp_param.parameters->float32_arr_type[2][0];
    }
  }
  
  return;
} /* dsp_osc_listener_envelope_follower */

