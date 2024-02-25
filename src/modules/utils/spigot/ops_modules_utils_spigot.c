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
                        float open)
{
	dsp_parameter params;
	struct dsp_port_in *ins;
	struct dsp_port_out *outs;
	
	params.name = "utils_spigot";

	/* input */
	params.in = malloc(sizeof(float) * dsp_global_period);
	
	params.parameters = malloc(sizeof(dsp_module_parameters_t));

	params.parameters->float32_arr_type = malloc(sizeof(float*));
	params.parameters->float32_type = malloc(sizeof(float));

	/* user-facing parameter allocation */
	params.parameters->float32_arr_type[0] = calloc(dsp_global_period, sizeof(float)); /* current open state */

	/* parameter assignment */
	for (int p=0; p<dsp_global_period; p++)
		params.parameters->float32_arr_type[0][p] = open;

	/* internal parameter assignment */
	params.parameters->float32_type[0] = open; /* last open state */
  
	ins = dsp_port_in_init("in", 512);
	ins->next = dsp_port_in_init("open", 512);

	outs = dsp_port_out_init("out", 1);
	
	dsp_add_module(target_bus,
		       "utils_spigot",
		       dsp_utils_spigot,
		       dsp_osc_listener_utils_spigot,
		       dsp_optimize_module,
		       params,
		       ins,
		       outs);
	return 0;
} /* dsp_create_utils_spigot */

void
dsp_utils_spigot(struct dsp_operation *utils_spigot,
		 int jack_samplerate) {
	float *outsamples;
  
	outsamples = calloc(dsp_global_period, sizeof(float));

	dsp_sum_summands(utils_spigot->module->dsp_param.in, utils_spigot->ins->summands);
  
	/* open input */
	if( utils_spigot->ins->next->summands != NULL )
		dsp_sum_summands(utils_spigot->module->dsp_param.parameters->float32_arr_type[0], utils_spigot->ins->next->summands);
	
	for(int p=0; p<dsp_global_period; p++) {
		if (utils_spigot->module->dsp_param.parameters->float32_arr_type[0][p])
			outsamples[p] = utils_spigot->module->dsp_param.in[p];
		else
			outsamples[p] = 0.0f;
	}
	
	/* drive outputs */
	memcpy(utils_spigot->outs->sample->value,
	       outsamples,
	       sizeof(float) * dsp_global_period);

	free(outsamples);
} /* dsp_utils_spigot */


void dsp_edit_utils_spigot(struct dsp_module *utils_spigot,
                           float open) {
	for (int p=0; p<dsp_global_period; p++)
		utils_spigot->dsp_param.parameters->float32_arr_type[0][p] = open;
} /* dsp_edit_utils_spigot */

void
dsp_osc_listener_utils_spigot(struct dsp_operation *utils_spigot,
			      int jack_samplerate) {
  /* float outsample = 0.0f; */
  /* char *path = NULL; */
  /* int path_len = 0; */

  
  /* /\* value input *\/ */
  /* if( utils_spigot->ins->summands != NULL ) { */
  /*      /\* if new value is different than old value, send osc messages *\/ */
  /*      if( (utils_spigot->module->dsp_param.parameters->float32_type[0] != utils_spigot->module->dsp_param.parameters->float32_type[1]) ) { */
  /*        path_len = 18 + 36 + 1; /\* len('/cyperus/listener/') + len(uuid4) + len('\n') *\/ */
  /*        path = (char *)malloc(sizeof(char) * path_len); */
  /*        snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_spigot->module->id); */
  /*        lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out); */
  /*        lo_send(lo_addr_send, path, "f", utils_spigot->module->dsp_param.parameters->float32_type[0]); */
  /*        free(lo_addr_send); */
  /*        utils_spigot->module->dsp_param.parameters->float32_type[1] = utils_spigot->module->dsp_param.parameters->float32_type[0]; */
  /*      } */
  /*      utils_spigot->module->dsp_param.parameters->int32_type[0] = 0; */
  /*    } else { */
  /*      utils_spigot->module->dsp_param.parameters->int32_type[0] += 1; */
  /*    } */
} /* dsp_utils_spigot */
