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

#include "ops_modules_utils_equals.h"

void*
_utils_equals_thread(void *arg) {
  
} /* _utils_equals_thread */


int
dsp_create_utils_equals(struct dsp_bus *target_bus,
                        float x,
                        float y) {
  dsp_parameter utils_equals_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  utils_equals_param.name = "utils_equals";
  utils_equals_param.pos = 0;
  
  utils_equals_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  utils_equals_param.parameters->float32_type = malloc(sizeof(float) * 4);
  utils_equals_param.parameters->float32_type[0] = x; /* current x */
  utils_equals_param.parameters->float32_type[1] = y; /* current y */
  utils_equals_param.parameters->float32_type[2] = x; /* last x */
  utils_equals_param.parameters->float32_type[3] = y; /* last y */
  
  /* osc listener, sample position tracking */
  utils_equals_param.parameters->int32_type = malloc(sizeof(int) * 2);
  utils_equals_param.parameters->int32_type[0] = 0;
  utils_equals_param.parameters->int32_type[1] = (int)((1.0f / 60.0f) * (float)jackcli_samplerate);
  
  utils_equals_param.parameters->char_type = malloc(sizeof(char*));
  utils_equals_param.parameters->char_type[0] = malloc(sizeof(char) * 37); /* len(uuid4) + len('\n') */
  strcpy(utils_equals_param.parameters->char_type[0], utils_equals_param.parameters->char_type[0]);  

  ins = dsp_port_in_init("x", 512, NULL);
  ins->next = dsp_port_in_init("y", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "utils_equals",
		 dsp_utils_equals,
                 dsp_osc_listener_utils_equals,
		 dsp_optimize_module,
		 utils_equals_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_utils_equals */

void
dsp_utils_equals(struct dsp_operation *utils_equals,
                  int jack_samplerate,
                  int pos) {
  char *path = NULL;
  int path_len = 0;
  float out = 0.0f;
  
  /* x input */
  if( utils_equals->ins->summands != NULL ) {  
     utils_equals->module->dsp_param.parameters->float32_type[0] = dsp_sum_summands(utils_equals->ins->summands);
  }

  /* y input */
  if( utils_equals->ins->next->summands != NULL ) {  
     utils_equals->module->dsp_param.parameters->float32_type[1] = dsp_sum_summands(utils_equals->ins->next->summands);
  }

  if(utils_equals->module->dsp_param.parameters->float32_type[0] ==
     utils_equals->module->dsp_param.parameters->float32_type[1] ) {
    out = 1.0f;
  }

  dsp_osc_listener_utils_equals(utils_equals, jack_samplerate, pos);
  
  /* drive outputs */
  utils_equals->outs->sample->value = out;
} /* dsp_utils_equals */


void dsp_edit_utils_equals(struct dsp_module *utils_equals,
                           float x,
                           float y) {
  printf("about to assign x\n");
  utils_equals->dsp_param.parameters->float32_type[0] = x;
  printf("assigned utils_equals.dsp_param.parameters->float32_type[0]: %f\n",
	 utils_equals->dsp_param.parameters->float32_type[0]);

  printf("about to assign y\n");
  utils_equals->dsp_param.parameters->float32_type[1] = y;
  printf("assigned utils_equals.dsp_param.parameters->float32_type[1]: %f\n",
	 utils_equals->dsp_param.parameters->float32_type[1]);
  
  printf("returning\n");
  
} /* dsp_edit_utils_equals */

void
dsp_osc_listener_utils_equals(struct dsp_operation *utils_equals,
                  int jack_samplerate,
                  int pos) {
  float outsample = 0.0f;
  char *path = NULL;
  int path_len = 0;

  
  /* value input */
  if( utils_equals->ins->summands != NULL ) {  
     /* osc listener, 60hz */
     int samples_waited = utils_equals->module->dsp_param.parameters->int32_type[0];
     int samples_to_wait = utils_equals->module->dsp_param.parameters->int32_type[1];
     if( samples_waited == samples_to_wait - 1) {
       /* if new value is different than old value, send osc messages */
       if( (utils_equals->module->dsp_param.parameters->float32_type[0] != utils_equals->module->dsp_param.parameters->float32_type[2]) ||
           (utils_equals->module->dsp_param.parameters->float32_type[1] != utils_equals->module->dsp_param.parameters->float32_type[3]) ) {
         path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
         path = (char *)malloc(sizeof(char) * path_len);
         snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_equals->module->id);    
         lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
         lo_send(lo_addr_send, path, "ff", utils_equals->module->dsp_param.parameters->float32_type[0], utils_equals->module->dsp_param.parameters->float32_type[1]);
         free(lo_addr_send);
         utils_equals->module->dsp_param.parameters->float32_type[2] = utils_equals->module->dsp_param.parameters->float32_type[0];
         utils_equals->module->dsp_param.parameters->float32_type[3] = utils_equals->module->dsp_param.parameters->float32_type[1];
       }
       utils_equals->module->dsp_param.parameters->int32_type[0] = 0;
     } else {
       utils_equals->module->dsp_param.parameters->int32_type[0] += 1;
     }

  }
} /* dsp_utils_equals */
