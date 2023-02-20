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

#include "ops_modules_osc_float.h"

void*
_osc_float_thread(void *arg) {
  
} /* _osc_float_thread */


int
dsp_create_osc_float(struct dsp_bus *target_bus,
                         float value) {
  dsp_parameter osc_float_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  osc_float_param.name = "osc_float";
  osc_float_param.pos = 0;
  
  osc_float_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  osc_float_param.parameters->float32_type = malloc(sizeof(float) * 2);
  osc_float_param.parameters->float32_type[0] = value; /* current value */
  osc_float_param.parameters->float32_type[1] = value; /* last value */
  
  /* osc listener, sample position tracking */
  osc_float_param.parameters->int32_type = malloc(sizeof(int) * 2);
  osc_float_param.parameters->int32_type[0] = 0;
  osc_float_param.parameters->int32_type[1] = (int)((1.0f / 60.0f) * (float)jackcli_samplerate);
  
  osc_float_param.parameters->char_type = malloc(sizeof(char*));
  osc_float_param.parameters->char_type[0] = malloc(sizeof(char) * 37); /* len(uuid4) + len('\n') */
  strcpy(osc_float_param.parameters->char_type[0], osc_float_param.parameters->char_type[0]);  

  ins = dsp_port_in_init("value", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "osc_float",
		 dsp_osc_float,
                 dsp_osc_listener_osc_float,
		 dsp_optimize_module,
		 osc_float_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_osc_float */

void
dsp_osc_float(struct dsp_operation *osc_float,
                  int jack_samplerate,
                  int pos) {
  float outsample = 0.0f;
  char *path = NULL;
  int path_len = 0;

  
  /* value input */
  if( osc_float->ins->summands != NULL ) {  
     osc_float->module->dsp_param.parameters->float32_type[0] = dsp_sum_summands(osc_float->ins->summands);
  }
  
  outsample = osc_float->module->dsp_param.parameters->float32_type[0];
  osc_float->outs->sample->value = outsample; 
  
} /* dsp_osc_float */


void dsp_edit_osc_float(struct dsp_module *osc_float,
				      float value) {
  printf("about to assign value\n");
  osc_float->dsp_param.parameters->float32_type[0] = value;
  printf("assigned osc_float.dsp_param.parameters->float32_type[0]: %f\n",
	 osc_float->dsp_param.parameters->float32_type[0]);
  printf("returning\n");
  
} /* dsp_edit_osc_float */

void
dsp_osc_listener_osc_float(struct dsp_operation *osc_float,
                  int jack_samplerate,
                  int pos) {
  float outsample = 0.0f;
  char *path = NULL;
  int path_len = 0;

  
  /* value input */
  if( osc_float->ins->summands != NULL ) {  
     /* osc listener, 60hz */
     int samples_waited = osc_float->module->dsp_param.parameters->int32_type[0];
     int samples_to_wait = osc_float->module->dsp_param.parameters->int32_type[1];
     if( samples_waited == samples_to_wait - 1) {
       /* if new value is different than old value, send osc messages */
       if(osc_float->module->dsp_param.parameters->float32_type[0] != osc_float->module->dsp_param.parameters->float32_type[1]) {
         path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
         path = (char *)malloc(sizeof(char) * path_len);
         snprintf(path, path_len, "%s%s", "/cyperus/listener/", osc_float->module->id);    
         lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
         lo_send(lo_addr_send, path, "f", osc_float->module->dsp_param.parameters->float32_type[0]);
         free(lo_addr_send);
         osc_float->module->dsp_param.parameters->float32_type[1] = osc_float->module->dsp_param.parameters->float32_type[0];
       }
       osc_float->module->dsp_param.parameters->int32_type[0] = 0;
     } else {
       osc_float->module->dsp_param.parameters->int32_type[0] += 1;
     }

  }
} /* dsp_osc_float */
