/* dsp_ops.h
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

#ifndef DSP_OPS_H
#define DSP_OPS_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include <lo/lo.h>

#include "dsp_types.h"
#include "rtqueue.h"
#include "dsp_math.h"
#include "dsp.h"
#include "jackcli.h"

float dsp_sum_input(struct dsp_port_in *in);
float dsp_sum_summands(struct dsp_operation_sample *summands);
void dsp_feed_outputs(char *current_bus_path, char *module_id, struct dsp_port_out *outs);
void dsp_optimize_connections_module(char *module_id, struct dsp_port_out *outs);
void dsp_optimize_connections_main_inputs(struct dsp_port_out *outs);
struct dsp_operation *dsp_optimize_module(char *id, struct dsp_module *module);

void dsp_osc_transmit(struct dsp_operation *osc_transmit, int jack_samplerate, int pos);
int dsp_create_osc_transmit(struct dsp_bus *target_bus, char *host, char *port, char *path, int samplerate_divisor);
void dsp_edit_osc_transmit(struct dsp_module *osc_transmit, char *host, char *port, char *path, int samplerate_divisor);

#endif
