/* ops_modules_oscillator_clock.c
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

#include "math_modules_oscillator_clock.h"
#include "ops_modules_oscillator_clock.h"

int
dsp_create_oscillator_clock(struct dsp_bus *target_bus,
                            float frequency,
                            float amplitude) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "oscillator_clock";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));  
  params.parameters->float32_type = malloc(sizeof(float) * 5);
  params.parameters->int32_type = malloc(sizeof(int) * 2);

  /* user-facing parameters */
  params.parameters->float32_type[0] = frequency;
  params.parameters->float32_type[1] = amplitude;

  /* internal parameters */
  params.parameters->int32_type[0] = 0;
  
  /* osc listener parameters */
  params.parameters->int32_type[1] = 0; /* samples waited */
  params.parameters->int32_type[2] = (int)((float)jackcli_samplerate / 60.0f); /* listener hz */

                                            
  /* osc listener param state parameters */                                            
  params.parameters->float32_type[2] = frequency; /* old frequency */
  params.parameters->float32_type[3] = amplitude; /* old amplitude */
  
  
  ins = dsp_port_in_init("param_frequency", 512, &(params.parameters->float32_type[0]));
  ins->next = dsp_port_in_init("param_amplitude", 512, &(params.parameters->float32_type[1]));
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "oscillator_clock",
		 dsp_oscillator_clock,
                 dsp_osc_listener_oscillator_clock,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_oscillator_clock */

void
dsp_oscillator_clock(struct dsp_operation *oscillator_clock, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  
  /* frequency input */
  if( oscillator_clock->ins->summands != NULL ) {
     oscillator_clock->module->dsp_param.parameters->float32_type[0] = dsp_sum_summands(oscillator_clock->ins->summands);
  }

  /* amplitude input */
  if( oscillator_clock->ins->next->summands != NULL ) {
     oscillator_clock->module->dsp_param.parameters->float32_type[1] = dsp_sum_summands(oscillator_clock->ins->next->summands);
  }
  
  dsp_osc_listener_oscillator_clock(oscillator_clock, jack_samplerate, pos);
  
  outsample = math_modules_oscillator_clock(oscillator_clock->module->dsp_param.parameters,
                                                  jack_samplerate,
                                                  pos);
  
  /* drive audio outputs */
  oscillator_clock->outs->sample->value = outsample;
  
  return;
} /* dsp_oscillator_clock */


void dsp_edit_oscillator_clock(struct dsp_module *oscillator_clock,
                               float frequency,
                               float amplitude) {
  printf("about to assign frequency\n");
  oscillator_clock->dsp_param.parameters->float32_type[0] = frequency;
  printf("assigned oscillator_clock->dsp_param.parameters->float32_type[0]: %f\n",
	 oscillator_clock->dsp_param.parameters->float32_type[0]);
  printf("about to assign amplitude\n");
  oscillator_clock->dsp_param.parameters->float32_type[1] = amplitude;
  printf("assigned oscillator_clock->dsp_param.parameters->float32_type[1]: %f\n",
	 oscillator_clock->dsp_param.parameters->float32_type[1]);

  printf("returning\n");
  
} /* dsp_edit_oscillator_clock */


void
dsp_osc_listener_oscillator_clock(struct dsp_operation *oscillator_clock, int jack_samplerate, int pos) {

  unsigned short param_connected = 0;
  if( (oscillator_clock->ins->summands != NULL) ||
      (oscillator_clock->ins->next->summands != NULL) ) {
     param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* osc listener, 60hz */
     int samples_waited = oscillator_clock->module->dsp_param.parameters->int32_type[1];
     int samples_to_wait = oscillator_clock->module->dsp_param.parameters->int32_type[2];
     if( samples_waited == samples_to_wait - 1) {
       /* if new value is different than old value, send osc messages */
       if(
          oscillator_clock->module->dsp_param.parameters->float32_type[0] != oscillator_clock->module->dsp_param.parameters->float32_type[2] ||
          oscillator_clock->module->dsp_param.parameters->float32_type[1] != oscillator_clock->module->dsp_param.parameters->float32_type[3]
          ) {
         int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
         char *path = (char *)malloc(sizeof(char) * path_len);
         snprintf(path, path_len, "%s%s", "/cyperus/listener/", oscillator_clock->module->id);    
         lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
         lo_send(lo_addr_send, path, "ff",
                 oscillator_clock->module->dsp_param.parameters->float32_type[0],
                 oscillator_clock->module->dsp_param.parameters->float32_type[1]);
         free(lo_addr_send);

         /* assign new parameter to last parameter after we're reported the change */
         oscillator_clock->module->dsp_param.parameters->float32_type[2] = oscillator_clock->module->dsp_param.parameters->float32_type[0];
         oscillator_clock->module->dsp_param.parameters->float32_type[3] = oscillator_clock->module->dsp_param.parameters->float32_type[1];
       }
       oscillator_clock->module->dsp_param.parameters->int32_type[1] = 0;
     } else {
       oscillator_clock->module->dsp_param.parameters->int32_type[1] += 1;
     } 
  }
  
  return;
} /* dsp_osc_listener_oscillator_clock */
