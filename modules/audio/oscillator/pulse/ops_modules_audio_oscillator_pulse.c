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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "math_modules_audio_oscillator_pulse.h"
#include "ops_modules_audio_oscillator_pulse.h"

int
dsp_create_oscillator_pulse(struct dsp_bus *target_bus,
                            float frequency,
                            float pulse_width,
                            float mul,
                            float add
                            ) {
  dsp_parameter params;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  printf("about to assign params\n");
  
  params.name = "oscillator_pulse";

  printf("about to set position\n");
  
  params.pos = 0;

  printf("about to malloc() parameters\n");
  
  params.parameters = malloc(sizeof(dsp_module_parameters_t));

  printf("about to assign float33_type\n");
  
  params.parameters->float32_type = malloc(sizeof(float) * 11);

  printf("about to assign user-facing params\n");
  
  /* user-facing parameters */
  params.parameters->float32_type[0] = frequency; /* ZIN0(0) */
  params.parameters->float32_type[1] = pulse_width; /* ZIN0(1) */
  params.parameters->float32_type[2] = mul;
  params.parameters->float32_type[3] = add;

  /* internal parameters */
  params.parameters->float32_type[4] = 8192 * (1.0 / jackcli_samplerate) * 65536.0 * 0.5; /* m_cpstoinc */
  params.parameters->float32_type[5] = (int)((jackcli_samplerate * 0.5) / frequency); /* m_N */
  params.parameters->float32_type[6] = frequency; /* m_freqIn */
  params.parameters->float32_type[7] = 0.5 / params.parameters->float32_type[5]; /* m_scale */
  params.parameters->float32_type[8] = 0; /* m_phase */
  params.parameters->float32_type[9] = 0.0f; /* m_phaseoff */
  params.parameters->float32_type[10] = 0.0f; /* m_y1 */

  /* allocate internal buffers */
  params.parameters->float32_arr_type = malloc(sizeof(float*)*2);
  params.parameters->float32_arr_type[0] = malloc(sizeof(float) * 8192 + 1); /* ft->mSine */
  params.parameters->float32_arr_type[1] = malloc(sizeof(float) * 8192 + 1); /* ft->mCosecant */


  printf("about to init module\n");
  math_modules_audio_oscillator_pulse_init(params.parameters);
  printf("finished init module\n");
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_freq", 512);
  ins->next->next = dsp_port_in_init("param_pulse_width", 512);
  ins->next->next->next = dsp_port_in_init("mul", 512);
  ins->next->next->next->next = dsp_port_in_init("add", 512);  
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
		 "oscillator_pulse",
		 dsp_oscillator_pulse,
		 dsp_optimize_module,
		 params,
		 ins,
		 outs);
  return 0;
} /* dsp_create_oscillator_pulse */

void
dsp_oscillator_pulse(struct dsp_operation *oscillator_pulse, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;

  insample = dsp_sum_summands(oscillator_pulse->ins->summands);
  if( oscillator_pulse->ins->next->summands != NULL ) {

    /* come back to this -- we need to figure out how to perform input calculations
       with the large coefficient calculations. */
    
    /* dsp_param.oscillator_pulse- = dsp_sum_summands(oscillator_pulse->ins->next->summands) * jack_samplerate; */
    
  }

  outsample = math_modules_audio_oscillator_pulse(oscillator_pulse->module->dsp_param.parameters,
                                                  jack_samplerate,
                                                  pos);
  
  /* drive audio outputs */
  oscillator_pulse->outs->sample->value = outsample;

  return;
} /* dsp_oscillator_pulse */


void dsp_edit_oscillator_pulse(struct dsp_module *oscillator_pulse,
                               float frequency,
                               float pulse_width,
                               float mul,
                               float add) {
  printf("about to assign frequency\n");
  oscillator_pulse->dsp_param.parameters->float32_type[0] = frequency;
  printf("assigned oscillator_pulse->dsp_param.parameters->float32_type[0]: %f\n",
	 oscillator_pulse->dsp_param.parameters->float32_type[0]);
  printf("about to assign pulse_width\n");
  oscillator_pulse->dsp_param.parameters->float32_type[1] = pulse_width;
  printf("assigned oscillator_pulse->dsp_param.parameters->float32_type[1]: %f\n",
	 oscillator_pulse->dsp_param.parameters->float32_type[1]);
  printf("about to assign mul\n");
  oscillator_pulse->dsp_param.parameters->float32_type[2] = mul;
  printf("assigned oscillator_pulse->dsp_param.parameters->float32_type[2]: %f\n", oscillator_pulse->dsp_param.parameters->float32_type[2]);
  printf("about to assign add\n");
  oscillator_pulse->dsp_param.parameters->float32_type[3] = add;
  printf("assigned oscillator_pulse->dsp_param.parameters->float32_type[3]: %f\n", oscillator_pulse->dsp_param.parameters->float32_type[3]);

  printf("returning\n");
  
} /* dsp_edit_oscillator_pulse */
