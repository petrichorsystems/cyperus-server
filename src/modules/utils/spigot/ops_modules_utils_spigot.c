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

#include "ops_modules_utils_spigot.h"

void*
_utils_spigot_thread(void *arg) {
  
} /* _utils_spigot_thread */


int
dsp_create_utils_spigot(struct dsp_bus *target_bus,
                        float open) {
  dsp_parameter utils_spigot_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  utils_spigot_param.name = "utils_spigot";
  utils_spigot_param.pos = 0;
  
  utils_spigot_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  utils_spigot_param.parameters->float32_type = malloc(sizeof(float) * 2);
  utils_spigot_param.parameters->float32_type[0] = open; /* current open state */
  utils_spigot_param.parameters->float32_type[1] = open; /* last open state */
  
  /* osc listener, sample position tracking */
  utils_spigot_param.parameters->int32_type = malloc(sizeof(int) * 2);
  utils_spigot_param.parameters->int32_type[0] = 0;
  utils_spigot_param.parameters->int32_type[1] = (int)((1.0f / 60.0f) * (float)jackcli_samplerate);
  
  utils_spigot_param.parameters->char_type = malloc(sizeof(char*));
  utils_spigot_param.parameters->char_type[0] = malloc(sizeof(char) * 37); /* len(uuid4) + len('\n') */
  strcpy(utils_spigot_param.parameters->char_type[0], utils_spigot_param.parameters->char_type[0]);  

  ins = dsp_port_in_init("in", 512, NULL);
  ins->next = dsp_port_in_init("open", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "utils_spigot",
		 dsp_utils_spigot,
                 dsp_osc_listener_utils_spigot,
		 dsp_optimize_module,
		 utils_spigot_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_utils_spigot */

void
dsp_utils_spigot(struct dsp_operation *utils_spigot,
                  int jack_samplerate,
                  int pos) {
  char *path = NULL;
  int path_len = 0;
  float out = 0.0f;
  
  /* open input */
  if( utils_spigot->ins->next->summands != NULL ) {  
     utils_spigot->module->dsp_param.parameters->float32_type[0] = dsp_sum_summands(utils_spigot->ins->next->summands);
  }

  if(utils_spigot->module->dsp_param.parameters->float32_type[0]) {
    out = dsp_sum_summands(utils_spigot->ins->summands);
  } else {
    out = 0.0f;
  }

  dsp_osc_listener_utils_spigot(utils_spigot, jack_samplerate, pos);
  
  /* drive outputs */
  utils_spigot->outs->sample->value = out;
} /* dsp_utils_spigot */


void dsp_edit_utils_spigot(struct dsp_module *utils_spigot,
                           float open) {
  printf("about to assign open\n");
  utils_spigot->dsp_param.parameters->float32_type[0] = open;
  printf("assigned utils_spigot.dsp_param.parameters->float32_type[0]: %f\n",
	 utils_spigot->dsp_param.parameters->float32_type[0]);

  printf("returning\n");
  
} /* dsp_edit_utils_spigot */

void
dsp_osc_listener_utils_spigot(struct dsp_operation *utils_spigot,
                  int jack_samplerate,
                  int pos) {
  float outsample = 0.0f;
  char *path = NULL;
  int path_len = 0;

  
  /* value input */
  if( utils_spigot->ins->summands != NULL ) {  
     /* osc listener, 60hz */
     int samples_waited = utils_spigot->module->dsp_param.parameters->int32_type[0];
     int samples_to_wait = utils_spigot->module->dsp_param.parameters->int32_type[1];
     if( samples_waited == samples_to_wait - 1) {
       /* if new value is different than old value, send osc messages */
       if( (utils_spigot->module->dsp_param.parameters->float32_type[0] != utils_spigot->module->dsp_param.parameters->float32_type[1]) ) {
         path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
         path = (char *)malloc(sizeof(char) * path_len);
         snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_spigot->module->id);    
         lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
         lo_send(lo_addr_send, path, "f", utils_spigot->module->dsp_param.parameters->float32_type[0]);
         free(lo_addr_send);
         utils_spigot->module->dsp_param.parameters->float32_type[1] = utils_spigot->module->dsp_param.parameters->float32_type[0];
       }
       utils_spigot->module->dsp_param.parameters->int32_type[0] = 0;
     } else {
       utils_spigot->module->dsp_param.parameters->int32_type[0] += 1;
     }

  }
} /* dsp_utils_spigot */
