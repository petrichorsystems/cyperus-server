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

#include "../../common.h"
#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_utils_float.h"
#include "ops_modules_utils_float.h"

int
dsp_create_utils_float(struct dsp_bus *target_bus,
                       float value) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params_modules_utils_float_init(&params,
				  value);

  ins = dsp_port_in_init("value");
  outs = dsp_port_out_init("out");

  dsp_add_module(target_bus,
		 "utils_float",
		 dsp_utils_float,
		 dsp_destroy_utils_float,
                 dsp_osc_listener_utils_float,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_utils_float */

int
dsp_destroy_utils_float(struct dsp_module *target_module) {
	params_modules_utils_float_free(&target_module->dsp_param);
	return 0;
} /* dsp_destroy_utils_float */

void
dsp_utils_float(struct dsp_operation *utils_float,
                int jack_samplerate) {

	params_modules_utils_float_edit_apply(&utils_float->module->dsp_param);
	
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
	modules_common_dsp_graph_lock();
	params_modules_utils_float_edit_pending(&utils_float->dsp_param,
						value);
	modules_common_dsp_graph_unlock();
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
    if( utils_float->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FLOAT_VAL] != utils_float->module->dsp_param.in[0] ) {
	    int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
	    char *path = (char *)malloc(sizeof(char) * path_len);
	    snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_float->module->id);    
	    
	    osc_send_broadcast(path, "f",
			       utils_float->module->dsp_param.in[0]);

	    /* assign new parameter to last parameter after we're reported the change */
	    utils_float->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FLOAT_VAL] = utils_float->module->dsp_param.in[0];
    }
  }
  
  return;
} /* dsp_osc_listener_utils_float */
