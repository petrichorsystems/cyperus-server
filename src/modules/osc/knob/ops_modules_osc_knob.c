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

#include "ops_modules_osc_knob.h"

void*
_osc_knob_thread(void *arg) {
  
} /* _osc_knob_thread */


int
dsp_create_osc_knob(struct dsp_bus *target_bus,
                         float beats_per_minute) {
  dsp_parameter osc_knob_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  osc_knob_param.name = "osc_knob";
  osc_knob_param.pos = 0;
  
  osc_knob_param.parameters = malloc(sizeof(dsp_module_parameters_t));
  osc_knob_param.parameters->float32_type = malloc(sizeof(float) * 2);
  osc_knob_param.parameters->float32_type[0] = beats_per_minute;

  /* sample position tracking */
  osc_knob_param.parameters->int32_type = malloc(sizeof(int) * 2);
  osc_knob_param.parameters->int32_type[0] = 0;
  osc_knob_param.parameters->int32_type[1] = (int)((float)1.0 / (osc_knob_param.parameters->float32_type[0] / 60.0f) * (float)jackcli_samplerate);
  
  osc_knob_param.parameters->char_type = malloc(sizeof(char*));
  osc_knob_param.parameters->char_type[0] = malloc(sizeof(char) * 37); /* len(uuid4) + len('\n') */
  strcpy(osc_knob_param.parameters->char_type[0], osc_knob_param.parameters->char_type[0]);  

  ins = dsp_port_in_init("beats_per_minute", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "osc_knob",
		 dsp_osc_knob,
		 dsp_optimize_module,
		 osc_knob_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_osc_knob */

void
dsp_osc_knob(struct dsp_operation *osc_knob,
                  int jack_samplerate,
                  int pos) {
  float outsample = 0.0f;
  char *path = NULL;
  int path_len = 0;

  int samples_waited = osc_knob->module->dsp_param.parameters->int32_type[0];
  int samples_to_wait = osc_knob->module->dsp_param.parameters->int32_type[1];
  if( samples_waited == samples_to_wait - 1) {
    path_len = 18 + 36 + 1; /* len('/cyperus/listener/') + len(uuid4) + len('\n') */
    path = (char *)malloc(sizeof(char) * path_len);
    snprintf(path, path_len, "%s%s", "/cyperus/listener/", osc_knob->module->id);    
    lo_address lo_addr_send = lo_address_new(send_host_out, send_port_out);
    lo_send(lo_addr_send, path, "f", 1.0f);
    free(lo_addr_send);
    outsample = 1.0f;
    osc_knob->module->dsp_param.parameters->int32_type[0] = 0;
  } else {
    osc_knob->module->dsp_param.parameters->int32_type[0] += 1;
  }

} /* dsp_osc_knob */


void dsp_edit_osc_knob(struct dsp_module *osc_knob,
				      float beats_per_minute) {
  printf("about to assign beats_per_minute\n");
  osc_knob->dsp_param.parameters->float32_type[0] = beats_per_minute;
  printf("assigned osc_knob.dsp_param.parameters->float32_type[0]: %f\n",
	 osc_knob->dsp_param.parameters->float32_type[0]);
  osc_knob->dsp_param.parameters->int32_type[0] = 0;
  osc_knob->dsp_param.parameters->int32_type[1] = (int)(1.0f / (osc_knob->dsp_param.parameters->float32_type[0] / 60.0f) * (float)jackcli_samplerate);
  printf("returning\n");
  
} /* dsp_edit_osc_knob */
