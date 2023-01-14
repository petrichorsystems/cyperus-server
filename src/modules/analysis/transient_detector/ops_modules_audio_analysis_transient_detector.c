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

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp.h"
#include "../../../dsp_ops.h"
#include "../../threadsync.h"
#include "../../../osc.h"

#include "ops_modules_audio_analysis_transient_detector.h"

void*
_transient_detector_thread(void *arg) {
  
} /* _transient_detector_thread */


int
dsp_create_transient_detector(struct dsp_bus *target_bus,
			      float sensitivity,
			      float attack_ms,
			      float decay_ms,
			      float scale) {
  dsp_parameter transient_detector_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  transient_detector_param.name = "transient_detector";
  transient_detector_param.pos = 0;

  transient_detector_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  
  /* external parameters */
  transient_detector_param.parameters->float32_type = malloc(sizeof(float) * 5);
  transient_detector_param.parameters->float32_type[0] = sensitivity;
  transient_detector_param.parameters->float32_type[1] = attack_ms;
  transient_detector_param.parameters->float32_type[2] = decay_ms;
  transient_detector_param.parameters->float32_type[3] = scale;
  
  /* internal state tracking */
  transient_detector_param.parameters->float32_type[4] = 0.0f; /* last sample storage */
  transient_detector_param.parameters->float32_type[5] = 0.0f; /* last last sample storage */

  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "transient_detector",
		 dsp_transient_detector,
		 dsp_optimize_module,
		 transient_detector_param,
		 ins,
		 outs
		 );
  return 0;
} /* dsp_create_transient_detector */

void
dsp_transient_detector(struct dsp_operation *transient_detector,
			 int jack_samplerate,
		       int pos) {
  float sensitivity = transient_detector->module->dsp_param.parameters->float32_type[0];
  float attack_ms = transient_detector->module->dsp_param.parameters->float32_type[1];
  float decay_ms = transient_detector->module->dsp_param.parameters->float32_type[2];
  float scale = transient_detector->module->dsp_param.parameters->float32_type[3];
  
  float coeff_attack = exp(log(0.01f) / (attack_ms * jack_samplerate * 0.001f));
  float coeff_decay = exp(log(0.01f) / (decay_ms * jack_samplerate * 0.001f));
  
  float insample = dsp_sum_summands(transient_detector->ins->summands);
  float odf_output, absin = 0.0f;

  int is_onset = 0;

  int path_len = 0;
  char *path = NULL;
  
  absin = fabs(insample);

  if(absin > transient_detector->module->dsp_param.parameters->float32_type[4])
    odf_output = coeff_attack * (transient_detector->module->dsp_param.parameters->float32_type[4] - absin) +  transient_detector->module->dsp_param.parameters->float32_type[4];
  else
    odf_output = coeff_decay * (transient_detector->module->dsp_param.parameters->float32_type[4] - absin) +  transient_detector->module->dsp_param.parameters->float32_type[4];

  /* if( (transient_detector->module->dsp_param.parameters->float32_type[4] > fabs(odf_output)) && */
  /*     (transient_detector->module->dsp_param.parameters->float32_type[4] > transient_detector->module->dsp_param.parameters->float32_type[5]) ) { */
  /*   if( transient_detector->module->dsp_param.parameters->float32_type[4] > _calculate_threshold() ) { */
  /*     is_onset = 1; */
  /*   } */
  /* } */
  
  transient_detector->module->dsp_param.parameters->float32_type[5] = transient_detector->module->dsp_param.parameters->float32_type[4];
  transient_detector->module->dsp_param.parameters->float32_type[4] = odf_output;

  if( is_onset ) {
    transient_detector->outs->sample->value = 1.0f;    
    path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
    path = (char *)malloc(sizeof(char) * path_len);
    snprintf(path, path_len, "%s%s", "/cyperus/listener/", transient_detector->module->id);
    lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
    lo_send(lo_addr_send, path, "f", 1.0f);
    free(lo_addr_send);
  } else
    transient_detector->outs->sample->value = 0.0f;
} /* dsp_transient_detector */


void dsp_edit_transient_detector(struct dsp_module *transient_detector,
				 float sensitivity,
				 float attack_ms,
				 float decay_ms,
				 float scale) {

  printf("about to assign sensitivity\n");
  transient_detector->dsp_param.parameters->float32_type[0] = sensitivity;
  printf("assigned transient_detector.dsp_param.parameters->float32_type[0]: %f\n",
	 transient_detector->dsp_param.parameters->float32_type[0]);
  
  printf("about to assign attack_ms\n");
  transient_detector->dsp_param.parameters->float32_type[1] = attack_ms;
  printf("assigned transient_detector.dsp_param.parameters->float32_type[1]: %f\n",
	 transient_detector->dsp_param.parameters->float32_type[1]);
  
  printf("about to assign decay_ms\n");
  transient_detector->dsp_param.parameters->float32_type[2] = decay_ms;
  printf("assigned transient_detector.dsp_param.parameters->float32_type[2]: %f\n",
	 transient_detector->dsp_param.parameters->float32_type[2]);
  
  printf("about to assign scale\n");
  transient_detector->dsp_param.parameters->float32_type[3] = scale;
  printf("assigned transient_detector.dsp_param.parameters->float32_type[3]: %f\n",
	 transient_detector->dsp_param.parameters->float32_type[3]);

  printf("returning\n");
  
} /* dsp_edit_transient_detector */
