/* ops_modules_oscillator_sine.c
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

#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_oscillator_sine.h"
#include "math_modules_oscillator_sine.h"
#include "ops_modules_oscillator_sine.h"

int
dsp_create_oscillator_sine(struct dsp_bus *target_bus,
			   float frequency,
			   float amplitude,
			   float phase) {

  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  params_modules_oscillator_sine_init(&params,
				      frequency,
				      amplitude,
				      phase);
  
  ins = dsp_port_in_init("param_frequency");
  ins->next = dsp_port_in_init("param_amplitude");
  ins->next->next = dsp_port_in_init("param_phase");
  outs = dsp_port_out_init("out");

  dsp_add_module(target_bus,
		 "oscillator_sine",
		 dsp_oscillator_sine,
		 dsp_destroy_oscillator_sine,
                 dsp_osc_listener_oscillator_sine,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_oscillator_sine */

int
dsp_destroy_oscillator_sine(struct dsp_module *target_module) {
	params_modules_oscillator_sine_free(&target_module->dsp_param);
  return 0;
} /* dsp_destroy_oscillator_sine */

void
dsp_oscillator_sine(struct dsp_operation *oscillator_sine, int jack_samplerate) {
	params_modules_oscillator_sine_edit_apply(&oscillator_sine->module->dsp_param);
	
	/* handle params with connected inputs */
	if( oscillator_sine->ins->summands != NULL )
		dsp_sum_summands(oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY],
				 oscillator_sine->ins->summands);
	if( oscillator_sine->ins->next->summands != NULL )
		dsp_sum_summands(oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMPLITUDE],
				 oscillator_sine->ins->next->summands);
	if( oscillator_sine->ins->next->next->summands != NULL )
		dsp_sum_summands(oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_PHASE],
				 oscillator_sine->ins->next->next->summands);

	math_modules_oscillator_sine(&oscillator_sine->module->dsp_param,
				     jack_samplerate);
  
	/* drive audio outputs */
	memcpy(oscillator_sine->outs->sample->value,
	       oscillator_sine->module->dsp_param.out,
	       sizeof(float) * dsp_global_period);
} /* dsp_oscillator_sine */


void dsp_edit_oscillator_sine(struct dsp_module *oscillator_sine,
                               float frequency,
                               float amplitude,
                               float phase) {
	pthread_spin_lock(&dsp_global.optimization_spinlock);
	pthread_mutex_lock(&dsp_global.graph_state_mutex);

	params_modules_oscillator_sine_edit_pending(&oscillator_sine->dsp_param,
						    frequency,
						    amplitude,
						    phase);
	
	pthread_spin_unlock(&dsp_global.optimization_spinlock);
	pthread_mutex_unlock(&dsp_global.graph_state_mutex);	
} /* dsp_edit_oscillator_sine */


void
dsp_osc_listener_oscillator_sine(struct dsp_operation *oscillator_sine, int jack_samplerate) {
  unsigned short param_connected = 0;
  if( (oscillator_sine->ins->summands != NULL) ||
      (oscillator_sine->ins->next->summands != NULL) ||
      (oscillator_sine->ins->next->next->summands != NULL) ) {
    param_connected = 1;
  }

  /* if param_connected, activate osc listener */
  if(param_connected) {
    /* if new value is different than old value, send osc messages */
    if(
       oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] != oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0] ||
       oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMPLITUDE] != oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMPLITUDE][0] ||
       oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_PHASE] != oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_PHASE][0]
       ) {
      int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
      char *path = (char *)malloc(sizeof(char) * path_len);
      snprintf(path, path_len, "%s%s", "/cyperus/listener/", oscillator_sine->module->id);    
      osc_send_broadcast( path, "fff",
              oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY],
              oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMPLITUDE],
              oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_PHASE]);
      free(path);
      
      /* assign new parameter to last parameter after we're reported the change */
      oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_FREQUENCY] = oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_FREQUENCY][0];
      oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_AMPLITUDE] = oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AMPLITUDE][0];
      oscillator_sine->module->dsp_param.parameters->float32_type[PARAM_LISTENER_PHASE] = oscillator_sine->module->dsp_param.parameters->float32_arr_type[PARAM_USER_PHASE][0];
    }
  }
  
  return;
} /* dsp_osc_listener_oscillator_sine */
