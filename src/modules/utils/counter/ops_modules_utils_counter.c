/* ops_modules_utils_counter.c
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

Copyright 2023 murray foster */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "math_modules_utils_counter.h"
#include "ops_modules_utils_counter.h"

int
dsp_create_utils_counter(struct dsp_bus *target_bus,
                         float reset,
                         float start,
                         float step_size,
                         float min,
                         float max,
                         float direction,
                         float auto_reset) {
	dsp_parameter params;
	struct dsp_port_in *ins;
	struct dsp_port_out *outs;

	params.name = "utils_counter";  
	params.parameters = malloc(sizeof(dsp_module_parameters_t));  

	params.parameters->float32_arr_type = malloc(sizeof(float *) * 7);
	params.parameters->float32_type = malloc(sizeof(float) * 8);

	/* user-facing parameter allocation */
	params.parameters->float32_arr_type[0] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[1] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[2] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[3] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[4] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[5] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[6] = calloc(dsp_global_period, sizeof(float));

	/* parameter assignment */
	for (int p=0; p<dsp_global_period; p++) {
		params.parameters->float32_arr_type[0][p] = reset;  
		params.parameters->float32_arr_type[1][p] = start;
		params.parameters->float32_arr_type[2][p] = step_size;
		params.parameters->float32_arr_type[3][p] = min;
		params.parameters->float32_arr_type[4][p] = max;
		params.parameters->float32_arr_type[5][p] = direction;
		params.parameters->float32_arr_type[6][p] = auto_reset;
	}

	/* internal parameters */
	params.parameters->float32_type[0] = start; /* current_value */
                                            
	/* osc listener param state parameters */
	params.parameters->float32_type[1] = reset; /* old reset */
	params.parameters->float32_type[2] = start;       /* old start */
	params.parameters->float32_type[3] = step_size;   /* old step_size */
	params.parameters->float32_type[4] = min;         /* old min */
	params.parameters->float32_type[5] = max;        /* old max */
	params.parameters->float32_type[6] = direction;  /* old direction */
	params.parameters->float32_type[7] = auto_reset; /* old auto_reset */
  
	ins = dsp_port_in_init("trigger", 512);
	ins->next = dsp_port_in_init("param_reset", 512);
	ins->next->next = dsp_port_in_init("param_start", 512);
	ins->next->next->next = dsp_port_in_init("param_step_size", 512);
	ins->next->next->next->next = dsp_port_in_init("param_min", 512);
	ins->next->next->next->next->next = dsp_port_in_init("param_max", 512);
	ins->next->next->next->next->next->next = dsp_port_in_init("param_direction", 512);
	ins->next->next->next->next->next->next->next = dsp_port_in_init("param_auto_reset", 512);
	
	outs = dsp_port_out_init("out", 1);

	dsp_add_module(target_bus,
		       "utils_counter",
		       dsp_utils_counter,
		       dsp_osc_listener_utils_counter,
		       dsp_optimize_module,
		       params,
		       ins,
		       outs);
	return 0;
} /* dsp_create_utils_counter */

void
dsp_utils_counter(struct dsp_operation *utils_counter, int jack_samplerate) {
	float *outsamples;
  
	/* input trigger */
	dsp_sum_summands(utils_counter->module->dsp_param.in, utils_counter->ins->summands);
  
	/* reset trigger */
	if( utils_counter->ins->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[0], utils_counter->ins->next->summands);
	}
  
	/* start value */
	if( utils_counter->ins->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[1], utils_counter->ins->next->next->summands);
	}

	/* step_size */
	if( utils_counter->ins->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[2], utils_counter->ins->next->next->next->summands);
	}

	/* min */
	if( utils_counter->ins->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[3], utils_counter->ins->next->next->next->next->summands);
	}

	/* max */
	if( utils_counter->ins->next->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[4], utils_counter->ins->next->next->next->next->next->summands);
	}   

	/* direction */
	if( utils_counter->ins->next->next->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[5], utils_counter->ins->next->next->next->next->next->next->summands);
	}

	/* auto_reset */
	if( utils_counter->ins->next->next->next->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[6], utils_counter->ins->next->next->next->next->next->next->next->summands);
	}
  
	outsamples = math_modules_utils_counter(&utils_counter->module->dsp_param,
						jack_samplerate);
  
	/* drive audio outputs */
	memcpy(utils_counter->outs->sample->value,
	       outsamples,
	       sizeof(float) * dsp_global_period);
	
	free(outsamples);
} /* dsp_utils_counter */

void dsp_edit_utils_counter(struct dsp_module *utils_counter,
                            float reset,
                            float start,
                            float step_size,
                            float min,
                            float max,
                            float direction,
                            float auto_reset) {
	for (int p=0; p<dsp_global_period; p++) {
		utils_counter->dsp_param.parameters->float32_arr_type[0][p] = reset;
		utils_counter->dsp_param.parameters->float32_arr_type[1][p] = start;
		utils_counter->dsp_param.parameters->float32_arr_type[2][p] = step_size;
		utils_counter->dsp_param.parameters->float32_arr_type[3][p] = min;
		utils_counter->dsp_param.parameters->float32_arr_type[4][p] = max;
		utils_counter->dsp_param.parameters->float32_arr_type[5][p] = direction;
		utils_counter->dsp_param.parameters->float32_arr_type[6][p] = auto_reset;
	}
} /* dsp_edit_utils_counter */

void
dsp_osc_listener_utils_counter(struct dsp_operation *utils_counter, int jack_samplerate) {
	/* unsigned short param_connected = 0; */
	/* if( (utils_counter->ins->next->summands != NULL) || */
	/*     (utils_counter->ins->next->next->summands != NULL) || */
	/*     (utils_counter->ins->next->next->next->summands != NULL) || */
	/*     (utils_counter->ins->next->next->next->next->summands != NULL) || */
	/*     (utils_counter->ins->next->next->next->next->next->summands != NULL) || */
	/*     (utils_counter->ins->next->next->next->next->next->next->summands != NULL) || */
	/*     (utils_counter->ins->next->next->next->next->next->next->next->summands != NULL) ) { */
	/* 	param_connected = 1; */
	/* } */

	/* /\* if param_connected, activate osc listener *\/ */
	/* if(param_connected) { */
	/* 	/\* if new value is different than old value, send osc messages *\/ */
	/* 	if (utils_counter->module->dsp_param.parameters->float32_type[0] != utils_counter->module->dsp_param.parameters->float32_type[2] || */
	/* 	    utils_counter->module->dsp_param.parameters->float32_type[1] != utils_counter->module->dsp_param.parameters->float32_type[3]) { */
	/* 		int path_len = 18 + 36 + 1; /\* len('/cyperus/listener/') + len(uuid4) + len('\n') *\/ */
	/* 		char *path = (char *)malloc(sizeof(char) * path_len); */
	/* 		snprintf(path, path_len, "%s%s", "/cyperus/listener/", utils_counter->module->id); */
	/* 		lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out); */
	/* 		lo_send(lo_addr_send, path, "ff", */
	/* 			utils_counter->module->dsp_param.parameters->float32_type[0], */
	/* 			utils_counter->module->dsp_param.parameters->float32_type[1]); */
	/* 		free(lo_addr_send); */
			
	/* 		/\* assign new parameter to last parameter after we're reported the change *\/ */
	/* 		utils_counter->module->dsp_param.parameters->float32_type[2] = utils_counter->module->dsp_param.parameters->float32_type[0]; */
	/* 		utils_counter->module->dsp_param.parameters->float32_type[3] = utils_counter->module->dsp_param.parameters->float32_type[1]; */
	/* 	} */
	/* 	utils_counter->module->dsp_param.parameters->int32_type[1] = 0; */
	/* } else { */
	/* 	utils_counter->module->dsp_param.parameters->int32_type[1] += 1; */
	/* } */
	
	return;
} /* dsp_osc_listener_utils_counter */
