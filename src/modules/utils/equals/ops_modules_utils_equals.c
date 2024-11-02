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
                        float y)
{
	dsp_parameter params;
	struct dsp_port_in *ins;
	struct dsp_port_out *outs;

	params.name = "utils_equals";  
	params.parameters = malloc(sizeof(dsp_module_parameters_t));

	params.parameters->float32_arr_type = malloc(sizeof(float *) * 2);
	params.parameters->float32_type = malloc(sizeof(float) * 2);

	/* user-facing parameter allocation */
	params.parameters->float32_arr_type[0] = calloc(dsp_global_period, sizeof(float)); /* current x */
	params.parameters->float32_arr_type[1] = calloc(dsp_global_period, sizeof(float)); /* current y */  
	
	/* parameter assignment */
	for (int p=0; p<dsp_global_period; p++) {
		params.parameters->float32_arr_type[0][p] = x;
		params.parameters->float32_arr_type[1][p] = y;
	}

	/* internal parameter assignment */
	params.parameters->float32_type[2] = x; /* last x */
	params.parameters->float32_type[3] = y; /* last y */

	ins = dsp_port_in_init("x", 512);
	ins->next = dsp_port_in_init("y", 512);
  
	outs = dsp_port_out_init("out", 1);
  
	dsp_add_module(target_bus,
		       "utils_equals",
		       dsp_utils_equals,
		       dsp_osc_listener_utils_equals,
		       dsp_optimize_module,
		       params,
		       ins,
		       outs);
	return 0;
} /* dsp_create_utils_equals */

void
dsp_utils_equals(struct dsp_operation *utils_equals,
		 int jack_samplerate)
{
	float *outsamples;;

	outsamples = calloc(dsp_global_period, sizeof(float));
	
	/* x input */
	if( utils_equals->ins->summands != NULL )
		dsp_sum_summands(utils_equals->module->dsp_param.parameters->float32_arr_type[0], utils_equals->ins->summands);

	/* y input */
	if( utils_equals->ins->next->summands != NULL )
		dsp_sum_summands(utils_equals->module->dsp_param.parameters->float32_arr_type[1], utils_equals->ins->next->summands);

	for(int p=0; p<dsp_global_period; p++) {
		if (utils_equals->module->dsp_param.parameters->float32_arr_type[0][p] ==
		   utils_equals->module->dsp_param.parameters->float32_arr_type[1][p] ) {
			outsamples[p] = 1.0f;
		} else {
			outsamples[p] = 0.0f;
		}
	}
  
	/* drive outputs */
	memcpy(utils_equals->outs->sample->value,
	       outsamples,
	       sizeof(float) * dsp_global_period);

	free(outsamples);
} /* dsp_utils_equals */


void dsp_edit_utils_equals(struct dsp_module *utils_equals,
                           float x,
                           float y)
{
	for(int p=0; p<dsp_global_period; p++) {
		utils_equals->dsp_param.parameters->float32_arr_type[0][p] = x;
		utils_equals->dsp_param.parameters->float32_arr_type[1][p] = y;
	}
} /* dsp_edit_utils_equals */

void
dsp_osc_listener_utils_equals(struct dsp_operation *utils_equals,
                  int jack_samplerate)
{
  /* float outsample = 0.0f; */
  /* char *path = NULL; */
  /* int path_len = 0; */
  
  /* /\* value input *\/ */
  /* if( utils_equals->ins->summands != NULL ) {   */
  /*      /\* if new value is different than old value, send osc messages *\/ */
  /*      if( (utils_equals->module->dsp_param.parameters->float32_type[0] != utils_equals->module->dsp_param.parameters->float32_type[2]) || */
  /*          (utils_equals->module->dsp_param.parameters->float32_type[1] != utils_equals->module->dsp_param.parameters->float32_type[3]) ) { */
  /*        path_len = 18 + 36 + 1; /\* len('/cyperus/listener/') + len(uuid4) + len('\n') *\/ */
  /*        path = (char *)malloc(sizeof(char) * path_len); */
  /*        snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_equals->module->id);     */
  /*        osc_send_broadcast(path, "ff", utils_equals->module->dsp_param.parameters->float32_type[0], utils_equals->module->dsp_param.parameters->float32_type[1]); */
  /*        utils_equals->module->dsp_param.parameters->float32_type[2] = utils_equals->module->dsp_param.parameters->float32_type[0]; */
  /*        utils_equals->module->dsp_param.parameters->float32_type[3] = utils_equals->module->dsp_param.parameters->float32_type[1]; */
  /*      } */
  /*      utils_equals->module->dsp_param.parameters->int32_type[0] = 0; */
  /*    } else { */
  /*      utils_equals->module->dsp_param.parameters->int32_type[0] += 1; */
  /*    } */
} /* dsp_utils_equals */
