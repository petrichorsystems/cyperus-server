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
                            float amplitude) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  params.name = "oscillator_triangle";  
  params.pos = 0;  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));  
  params.parameters->float32_type = malloc(sizeof(float) * 2);

  /* user-facing parameters */
  params.parameters->float32_type[0] = frequency;
  params.parameters->float32_type[1] = amplitude;

  ins = dsp_port_in_init("param_frequency", 512, &(params.parameters->float32_type[0]));
  ins->next = dsp_port_in_init("param_amplitude", 512, &(params.parameters->float32_type[1]));
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "oscillator_triangle",
		 dsp_oscillator_triangle,
                 NULL,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_oscillator_triangle */

void
dsp_oscillator_triangle(struct dsp_operation *oscillator_triangle, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  
  outsample = math_modules_oscillator_triangle(oscillator_triangle->module->dsp_param.parameters,
                                                  jack_samplerate,
                                                  pos);  
  /* drive audio outputs */
  oscillator_triangle->outs->sample->value = outsample;
  
  return;
} /* dsp_oscillator_triangle */


void dsp_edit_oscillator_triangle(struct dsp_module *oscillator_triangle,
                               float frequency,
                               float amplitude) {
  printf("about to assign frequency\n");
  oscillator_triangle->dsp_param.parameters->float32_type[0] = frequency;
  printf("assigned oscillator_triangle->dsp_param.parameters->float32_type[0]: %f\n",
	 oscillator_triangle->dsp_param.parameters->float32_type[0]);
  printf("about to assign amplitude\n");
  oscillator_triangle->dsp_param.parameters->float32_type[1] = amplitude;
  printf("assigned oscillator_triangle->dsp_param.parameters->float32_type[1]: %f\n",
	 oscillator_triangle->dsp_param.parameters->float32_type[1]);

  printf("returning\n");
  
} /* dsp_edit_oscillator_triangle */
