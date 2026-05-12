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

#include "../../common.h"
#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_oscillator_clock.h"
#include "math_modules_oscillator_clock.h"
#include "ops_modules_oscillator_clock.h"

int
dsp_create_oscillator_clock(struct dsp_bus *target_bus,
                            float frequency,
                            float amplitude) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params_modules_oscillator_clock_init(&params,
				       frequency,
				       amplitude);

  ins = dsp_port_in_init("param_frequency");
  ins->next = dsp_port_in_init("param_amplitude");
  outs = dsp_port_out_init("out");

  dsp_add_module(target_bus,
		 "oscillator_clock",
		 dsp_oscillator_clock,
		 dsp_destroy_oscillator_clock,
                 dsp_osc_listener_oscillator_clock,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_oscillator_clock */

int
dsp_destroy_oscillator_clock(struct dsp_module *target_module) {
	params_modules_oscillator_clock_free(&target_module->dsp_param);
	return 0;
} /* dsp_destroy_oscillator_clock */

void
dsp_oscillator_clock(struct dsp_operation *oscillator_clock, int jack_samplerate) {
	params_modules_oscillator_clock_edit_apply(&oscillator_clock->module->dsp_param);

	/* handle params with connected inputs */
	if( oscillator_clock->ins->summands != NULL ) /* frequency */
		dsp_sum_summands(oscillator_clock->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY], oscillator_clock->ins->summands);
	if( oscillator_clock->ins->next->summands != NULL ) /* amplitude */
		dsp_sum_summands(oscillator_clock->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMPLITUDE], oscillator_clock->ins->next->summands);
    
	math_modules_oscillator_clock(&oscillator_clock->module->dsp_param,
				      jack_samplerate);
  
	/* drive audio outputs */
	memcpy(oscillator_clock->outs->sample->value,
	       oscillator_clock->module->dsp_param.out,
	       sizeof(float) * dsp_global_period);
} /* dsp_oscillator_clock */


void dsp_edit_oscillator_clock(struct dsp_module *oscillator_clock,
                               float frequency,
                               float amplitude)
{
	modules_common_dsp_graph_lock();
	params_modules_oscillator_clock_edit_pending(&oscillator_clock->dsp_param,
						     frequency,
						     amplitude);
	modules_common_dsp_graph_unlock();
} /* dsp_edit_oscillator_clock */


void
dsp_osc_listener_oscillator_clock(struct dsp_operation *oscillator_clock, int jack_samplerate)
{
  unsigned short param_connected = 0;
  if( (oscillator_clock->ins->summands != NULL) ||
      (oscillator_clock->ins->next->summands != NULL) ) {
     param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
       /* if new value is different than old value, send osc messages */
	  if (oscillator_clock->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] != oscillator_clock->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0] ||
	      oscillator_clock->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMPLITUDE] != oscillator_clock->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMPLITUDE][0]) {
		  int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
		  char *path = (char *)malloc(sizeof(char) * path_len);
		  snprintf(path, path_len, "%s%s", "/cyperus/listener/", oscillator_clock->module->id);    
		  osc_send_broadcast(path, "ff",
			  oscillator_clock->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY],
			  oscillator_clock->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMPLITUDE]);
		  
		  /* assign new parameter to last parameter after we're reported the change */
		  oscillator_clock->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] = oscillator_clock->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0];
		  oscillator_clock->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMPLITUDE] = oscillator_clock->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMPLITUDE][0];
	  }
  }
  
  return;
} /* dsp_osc_listener_oscillator_clock */
