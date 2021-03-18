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

#include <lo/lo.h>

#include "../../../../jackcli.h"
#include "../../../../cyperus.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp.h"
#include "../../../../dsp_ops.h"
#include "../../../threadsync.h"

#include "ops_modules_movement_osc_osc_metronome.h"

void*
_osc_metronome_thread(void *arg) {
  
} /* _osc_metronome_thread */


int
dsp_create_osc_metronome(struct dsp_bus *target_bus,
				   float frequency) {
  dsp_parameter osc_metronome_param;
  struct dsp_port_in *ins;

  osc_metronome_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  osc_metronome_param.parameters->float32_type = malloc(sizeof(float));

  osc_metronome_param.parameters->char_type = malloc(sizeof(char*));
  osc_metronome_param.parameters->char_type[0] = malloc(sizeof(char) * 37); /* uuid4 36 bytes + '\n' */
  
  
  osc_metronome_param.name = "osc_metronome";
  osc_metronome_param.pos = 0;

  osc_metronome_param.parameters->float32_type[0] = frequency;
  strcpy(osc_metronome_param.parameters->char_type[0], osc_metronome_param.parameters->char_type[0]);

  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("frequency", 512);
  dsp_add_module(target_bus,
		 "osc_metronome",
		 dsp_osc_metronome,
		 dsp_optimize_module,
		 osc_metronome_param,
		 ins,
		 NULL);
  return 0;
} /* dsp_create_osc_metronome */

void
dsp_osc_metronome(struct dsp_operation *osc_metronome, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  
  insample = dsp_sum_summands(osc_metronome->ins->summands);
  osc_metronome->module->dsp_param.in = insample;

  if( osc_metronome->ins->next->summands != NULL ) {

    /* come back to this -- we need to figure out how to perform input calculations
       with the large coefficient calculations. */
    
    /* dsp_param.osc_metronome- = dsp_sum_summands(osc_metronome->ins->next->summands) * jack_samplerate; */
    
  }
  
  /* drive audio outputs */
  osc_metronome->outs->sample->value = outsample;

  return;
} /* dsp_osc_metronome */


void dsp_edit_osc_metronome(struct dsp_module *osc_metronome,
				      float frequency) {
  printf("about to assign frequency\n");
  osc_metronome->dsp_param.parameters->float32_type[0] = frequency;
  printf("assigned osc_metronome->dsp_param.parameters->float32_type[0]: %f\n",
	 osc_metronome->dsp_param.parameters->float32_type[0]);

  printf("returning\n");
  
} /* dsp_edit_osc_metronome */
