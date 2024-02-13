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

#include "ops_modules_utils_float.h"

void*
_utils_float_thread(void *arg) {
  
} /* _utils_float_thread */


int
dsp_create_utils_float(struct dsp_bus *target_bus,
                       float value) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "utils_float";
  params.pos = 0;

  /* audio input */
  params.in = malloc(sizeof(float) * dsp_global_period);

  params.parameters = malloc(sizeof(dsp_module_parameters_t));
  
  params.parameters->float32_type = malloc(sizeof(float));

  /* user-facing parameter allocation */
  params.parameters->float32_arr_type = calloc(dsp_global_period, sizeof(float *));

  for(int p=0; p<dsp_global_period; p++) {
    /* user-facing parameter assignment */
    params.in[p] = value;
  }
  
  /* osc listener parameters */
  params.parameters->float32_type[0] = value;

  ins = dsp_port_in_init("value", 512);
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "utils_float",
		 dsp_utils_float,
                 dsp_osc_listener_utils_float,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_utils_float */

void
dsp_utils_float(struct dsp_operation *utils_float,
                int jack_samplerate) {
  
  /* handle params with connected inputs */
  if( utils_float->ins->summands != NULL ) {  
    dsp_sum_summands(utils_float->module->dsp_param.in, utils_float->ins->summands);
  }

  /* drive outputs */
  memcpy(utils_float->outs->sample->value,
         utils_float->module->dsp_param.in,
         sizeof(float) * dsp_global_period);

} /* dsp_utils_float */


void dsp_edit_utils_float(struct dsp_module *utils_float,
				      float value) {
  for(int p=0; p<dsp_global_period; p++)
    utils_float->dsp_param.in[p] = value;
  
} /* dsp_edit_utils_float */

void
dsp_osc_listener_utils_float(struct dsp_operation *utils_float, int jack_samplerate) {
  unsigned short param_connected = 0;
  if( utils_float->ins->summands != NULL) {
    param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* if new value is different than old value, send osc messages */
    if( utils_float->module->dsp_param.parameters->float32_type[0] != utils_float->module->dsp_param.in[0] ) {
	    int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
	    char *path = (char *)malloc(sizeof(char) * path_len);
	    snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_float->module->id);    
	    
	    lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
	    lo_send(lo_addr_send, path, "f",
		    utils_float->module->dsp_param.in[0]);
	    free(lo_addr_send);

	    /* assign new parameter to last parameter after we're reported the change */
	    utils_float->module->dsp_param.parameters->float32_type[0] = utils_float->module->dsp_param.in[0];
    }
  }
  
  return;
} /* dsp_osc_listener_utils_float */
