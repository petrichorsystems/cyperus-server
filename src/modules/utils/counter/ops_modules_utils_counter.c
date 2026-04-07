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

#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_utils_counter.h"
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

	/* audio input+output */
	params.in = malloc(sizeof(float) * dsp_global_period);
	params.out = malloc(sizeof(float) * dsp_global_period);	
	
	params.parameters = malloc(sizeof(dsp_module_parameters_t));  

	params.parameters->float32_arr_type = malloc(sizeof(float *) * 7);
	params.parameters->float32_type = malloc(sizeof(float) * 8);

	/* user-facing parameter allocation */
	params.parameters->float32_arr_type[PARAM_USER_RESET] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[PARAM_USER_START] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[PARAM_USER_STEP_SIZE] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[PARAM_USER_MIN] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[PARAM_USER_MAX] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[PARAM_USER_DIRECTION] = calloc(dsp_global_period, sizeof(float));
	params.parameters->float32_arr_type[PARAM_USER_AUTO_RESET] = calloc(dsp_global_period, sizeof(float));

	/* parameter assignment */
	for (int p=0; p<dsp_global_period; p++) {
		params.parameters->float32_arr_type[PARAM_USER_RESET][p] = reset;  
		params.parameters->float32_arr_type[PARAM_USER_START][p] = start;
		params.parameters->float32_arr_type[PARAM_USER_STEP_SIZE][p] = step_size;
		params.parameters->float32_arr_type[PARAM_USER_MIN][p] = min;
		params.parameters->float32_arr_type[PARAM_USER_MAX][p] = max;
		params.parameters->float32_arr_type[PARAM_USER_DIRECTION][p] = direction;
		params.parameters->float32_arr_type[PARAM_USER_AUTO_RESET][p] = auto_reset;
	}

	/* internal parameters */
	params.parameters->float32_type[PARAM_INTERNAL_START] = start; /* current_value */
                                            
	/* osc listener param state parameters */
	params.parameters->float32_type[PARAM_LISTENER_RESET] = reset; /* old reset */
	params.parameters->float32_type[PARAM_LISTENER_START] = start;       /* old start */
	params.parameters->float32_type[PARAM_LISTENER_STEP_SIZE] = step_size;   /* old step_size */
	params.parameters->float32_type[PARAM_LISTENER_MIN] = min;         /* old min */
	params.parameters->float32_type[PARAM_LISTENER_MAX] = max;        /* old max */
	params.parameters->float32_type[PARAM_LISTENER_DIRECTION] = direction;  /* old direction */
	params.parameters->float32_type[PARAM_LISTENER_AUTO_RESET] = auto_reset; /* old auto_reset */
  
	ins = dsp_port_in_init("trigger");
	ins->next = dsp_port_in_init("param_reset");
	ins->next->next = dsp_port_in_init("param_start");
	ins->next->next->next = dsp_port_in_init("param_step_size");
	ins->next->next->next->next = dsp_port_in_init("param_min");
	ins->next->next->next->next->next = dsp_port_in_init("param_max");
	ins->next->next->next->next->next->next = dsp_port_in_init("param_direction");
	ins->next->next->next->next->next->next->next = dsp_port_in_init("param_auto_reset");
	
	outs = dsp_port_out_init("out");

	dsp_add_module(target_bus,
		       "utils_counter",
		       dsp_utils_counter,
		       dsp_destroy_utils_counter,
		       NULL, /* dsp_osc_listener_utils_counter, */
		       dsp_optimize_module,
		       params,
		       ins,
		       outs);
	return 0;
} /* dsp_create_utils_counter */

int
dsp_destroy_utils_counter(struct dsp_module *target_module) {
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_RESET]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_START]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_STEP_SIZE]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_MIN]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_MAX]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_DIRECTION]);
	free(target_module->dsp_param.parameters->float32_arr_type[PARAM_USER_AUTO_RESET]);	     
	free(target_module->dsp_param.parameters->float32_arr_type);
	free(target_module->dsp_param.parameters->float32_type);
	free(target_module->dsp_param.parameters);
	free(target_module->dsp_param.out);
	free(target_module->dsp_param.in);	
  return 0;
} /* dsp_destroy_utils_counter */

void
dsp_utils_counter(struct dsp_operation *utils_counter, int jack_samplerate) {  
	/* input trigger */
	dsp_sum_summands(utils_counter->module->dsp_param.in, utils_counter->ins->summands);
  
	/* reset trigger */
	if( utils_counter->ins->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_RESET], utils_counter->ins->next->summands);
	}
  
	/* start value */
	if( utils_counter->ins->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_START], utils_counter->ins->next->next->summands);
	}

	/* step_size */
	if( utils_counter->ins->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_STEP_SIZE], utils_counter->ins->next->next->next->summands);
	}

	/* min */
	if( utils_counter->ins->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_MIN], utils_counter->ins->next->next->next->next->summands);
	}

	/* max */
	if( utils_counter->ins->next->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_MAX], utils_counter->ins->next->next->next->next->next->summands);
	}

	/* direction */
	if( utils_counter->ins->next->next->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_DIRECTION], utils_counter->ins->next->next->next->next->next->next->summands);
	}

	/* auto_reset */
	if( utils_counter->ins->next->next->next->next->next->next->next->summands != NULL ) {
		dsp_sum_summands(utils_counter->module->dsp_param.parameters->float32_arr_type[PARAM_USER_AUTO_RESET], utils_counter->ins->next->next->next->next->next->next->next->summands);
	}
	
	math_modules_utils_counter(&utils_counter->module->dsp_param,
				   jack_samplerate);
  
	/* drive audio outputs */
	memcpy(utils_counter->outs->sample->value,
	       utils_counter->module->dsp_param.out,
	       sizeof(float) * dsp_global_period);
	
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
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_RESET][p] = reset;
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_START][p] = start;
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_STEP_SIZE][p] = step_size;
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_MIN][p] = min;
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_MAX][p] = max;
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_DIRECTION][p] = direction;
		utils_counter->dsp_param.parameters->float32_arr_type[PARAM_USER_AUTO_RESET][p] = auto_reset;
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
	/* 		osc_send_broadcast( path, "ff", */
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
