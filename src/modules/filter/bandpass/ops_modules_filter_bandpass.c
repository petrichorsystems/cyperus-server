/* ops_modules_filter_bandpass.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
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

#include "../../common.h"
#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_filter_bandpass.h"
#include "math_modules_filter_bandpass.h"
#include "ops_modules_filter_bandpass.h"

int
dsp_create_filter_bandpass(struct dsp_bus *target_bus,
                           float frequency,
                           float q,
                           float amount) {
	
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params_modules_filter_bandpass_init(&params,
				      frequency,
				      q,
				      amount);
  math_modules_filter_bandpass_init(&params);
  
  ins = dsp_port_in_init("in");
  ins->next = dsp_port_in_init("param_frequency");
  ins->next->next = dsp_port_in_init("param_q");
  ins->next->next->next = dsp_port_in_init("param_amount");  
  outs = dsp_port_out_init("out");

  dsp_add_module(target_bus,
		 "filter_bandpass",
		 dsp_filter_bandpass,
		 dsp_destroy_filter_bandpass,
                 dsp_osc_listener_filter_bandpass,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_filter_bandpass */

int dsp_destroy_filter_bandpass(struct dsp_module *target_module) {
	params_modules_filter_bandpass_free(&target_module->dsp_param);
	return 0;
} /* dsp_destroy_filter_bandpass */

void
dsp_filter_bandpass(struct dsp_operation *filter_bandpass, int jack_samplerate) {
	params_modules_filter_bandpass_edit_apply(&filter_bandpass->module->dsp_param);
	dsp_sum_summands(filter_bandpass->module->dsp_param.in, filter_bandpass->ins->summands);

	/* handle params with connected inputs */
	if (filter_bandpass->ins->next->summands != NULL ) { /* frequency */
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY], filter_bandpass->ins->next->summands);
	}
	
	if( filter_bandpass->ins->next->next->summands != NULL ) {
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q], filter_bandpass->ins->next->next->summands);
	}

	if( filter_bandpass->ins->next->next->next->summands != NULL ) {
		dsp_sum_summands(filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT], filter_bandpass->ins->next->next->next->summands);
	}
	
	math_modules_filter_bandpass(&filter_bandpass->module->dsp_param,
				     jack_samplerate);

	memcpy(filter_bandpass->outs->sample->value,
	       filter_bandpass->module->dsp_param.out,
	       sizeof(float) * dsp_global_period);

} /* dsp_filter_bandpass */

void dsp_edit_filter_bandpass(struct dsp_module *filter_bandpass,
                              float frequency,
                              float q,
                              float amount) {
	modules_common_dsp_graph_lock();
	params_modules_filter_bandpass_edit_pending(&filter_bandpass->dsp_param,
						    frequency,
						    q,
						    amount);
	modules_common_dsp_graph_unlock();
} /* dsp_edit_filter_bandpass */

void
dsp_osc_listener_filter_bandpass(struct dsp_operation *filter_bandpass, int jack_samplerate) {
  unsigned short param_connected = 0;
  if( (filter_bandpass->ins->next->summands != NULL) ||
      (filter_bandpass->ins->next->next->summands != NULL) ||
      (filter_bandpass->ins->next->next->next->summands != NULL) ) {
    param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* if new value is different than old value, send osc messages */
    if(
       filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0] ||
       filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_Q] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][0] ||
       filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMOUNT] != filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0]
       ) {
      int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
      char *path = (char *)malloc(sizeof(char) * path_len);
      snprintf(path, path_len, "%s%s", "/cyperus/listener/", filter_bandpass->module->id);    

      osc_send_broadcast( path, "fff",
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0],
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][0],
              filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0]);

      /* assign new parameter to last parameter after we're reported the change */
      filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0];
      filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_Q] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_Q][0];
      filter_bandpass->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMOUNT] = filter_bandpass->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMOUNT][0];
    }
  }
  
  return;
} /* dsp_osc_listener_filter_bandpass */
