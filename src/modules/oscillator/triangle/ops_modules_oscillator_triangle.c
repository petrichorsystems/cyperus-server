/* ops_modules_oscillator_triangle.c
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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "math_modules_oscillator_triangle.h"
#include "ops_modules_oscillator_triangle.h"

int
dsp_create_oscillator_triangle(struct dsp_bus *target_bus,
			       float frequency,
			       float amplitude)
{
	dsp_parameter params;
	struct dsp_port_in *ins;
	struct dsp_port_out *outs;
	int p;
  
	params.name = "oscillator_triangle";
  
	params.parameters = malloc(sizeof(dsp_module_parameters_t));  

	params.parameters->float32_arr_type = malloc(sizeof(float *) * 2);
	params.parameters->float32_type = malloc(sizeof(float) * 2);
	params.parameters->int32_type = malloc(sizeof(int) * 1);
	
	/* user-facing parameter allocation */
	params.parameters->float32_arr_type[0] = calloc(dsp_global_period, sizeof(float)); /* frequency */
	params.parameters->float32_arr_type[1] = calloc(dsp_global_period, sizeof(float)); /* amplitude */

	
	/* user-facing parameter assigment */
	for (p=0; p<dsp_global_period; p++) {
		params.parameters->float32_arr_type[0][p] = frequency;
		params.parameters->float32_arr_type[1][p] = amplitude;	  
	}

	/* internal parameter assignment */
	params.parameters->int32_type[0] = 0;
	
	/* osc listener param state parameters */
	params.parameters->float32_type[0] = frequency;
	params.parameters->float32_type[1] = amplitude;

	ins = dsp_port_in_init("param_frequency");
	ins->next = dsp_port_in_init("param_amplitude");
	outs = dsp_port_out_init("out");

	dsp_add_module(target_bus,
		       "oscillator_triangle",
		       dsp_oscillator_triangle,
		       dsp_osc_listener_oscillator_triangle,
		       dsp_optimize_module,
		       params,
		       ins,
		       outs);
	return 0;
} /* dsp_create_oscillator_triangle */

void
dsp_oscillator_triangle(struct dsp_operation *oscillator_triangle, int jack_samplerate)
{
	float *outsamples;

	/* handle params with connected inputs */
	if( oscillator_triangle->ins->summands != NULL ) /* frequency */
		dsp_sum_summands(oscillator_triangle->module->dsp_param.parameters->float32_arr_type[0], oscillator_triangle->ins->summands);
	if( oscillator_triangle->ins->next->summands != NULL ) /* amplitude */
		dsp_sum_summands(oscillator_triangle->module->dsp_param.parameters->float32_arr_type[1], oscillator_triangle->ins->next->summands);
	
	outsamples = math_modules_oscillator_triangle(oscillator_triangle->module->dsp_param.parameters,
						      jack_samplerate);  
	/* drive audio outputs */
	memcpy(oscillator_triangle->outs->sample->value,
	       outsamples,
	       sizeof(float) * dsp_global_period);
  
	free(outsamples);
} /* dsp_oscillator_triangle */


void dsp_edit_oscillator_triangle(struct dsp_module *oscillator_triangle,
                               float frequency,
                               float amplitude) {
  for(int p=0; p<dsp_global_period; p++) {
    oscillator_triangle->dsp_param.parameters->float32_arr_type[0][p] = frequency;
    oscillator_triangle->dsp_param.parameters->float32_arr_type[1][p] = amplitude;
  }
  
} /* dsp_edit_oscillator_triangle */

void
dsp_osc_listener_oscillator_triangle(struct dsp_operation *oscillator_triangle, int jack_samplerate) {
	unsigned short param_connected = 0;
	if( (oscillator_triangle->ins->summands != NULL) ||
	    (oscillator_triangle->ins->next->summands != NULL)) {
		param_connected = 1;
	}

	/* if param_connected, activate osc listener */
	if(param_connected) {
		/* if new value is different than old value, send osc messages */
		if(oscillator_triangle->module->dsp_param.parameters->float32_type[0] != oscillator_triangle->module->dsp_param.parameters->float32_arr_type[0][0] ||
		   oscillator_triangle->module->dsp_param.parameters->float32_type[1] != oscillator_triangle->module->dsp_param.parameters->float32_arr_type[1][0]) {
			int path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
			char *path = (char *)malloc(sizeof(char) * path_len);
			snprintf(path, path_len, "%s%s", "/cyperus/listener/", oscillator_triangle->module->id);    
			osc_send_broadcast( path, "ff",
				oscillator_triangle->module->dsp_param.parameters->float32_type[0],
				oscillator_triangle->module->dsp_param.parameters->float32_type[1]);
			
			/* assign new parameter to last parameter after we're reported the change */
			oscillator_triangle->module->dsp_param.parameters->float32_type[0] = oscillator_triangle->module->dsp_param.parameters->float32_arr_type[0][0];
			oscillator_triangle->module->dsp_param.parameters->float32_type[1] = oscillator_triangle->module->dsp_param.parameters->float32_arr_type[1][0];
		}
	}
	
	return;
} /* dsp_osc_listener_oscillator_triangle */
