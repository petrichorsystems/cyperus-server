/* dsp.h
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

Copyright 2015 murray foster */

#ifndef DSP_H
#define DSP_H

#include <stdbool.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

#include "dsp_math.h"
#include "dsp_types.h"
#include "dsp_ops.h"

#include "dsp_graph_id.h"

#include "errno.h"

extern unsigned short dsp_global_period;
extern bool dsp_global_new_operation_graph;

struct dsp_bus_port*
dsp_build_bus_ports(struct dsp_bus *parent_bus, struct dsp_bus_port *head_port, char *bus_ports, int out);

int
dsp_free_module(struct dsp_bus *parent_bus, struct dsp_module *target_module, bool mutex);

int
dsp_remove_module(struct dsp_module *target_module);

void
dsp_bypass_module(struct dsp_module *module, int bypass);

struct dsp_module*
dsp_add_module(struct dsp_bus *target_bus,
	       char *name,
	       void (*dsp_function) (struct dsp_operation*, int),
	       int (*dsp_destroy_function) (struct dsp_module*),	       
               void (*dsp_osc_listener_function) (struct dsp_operation*, int),
	       struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*),
	       dsp_parameter dsp_param,
	       struct dsp_port_in *ins,
	       struct dsp_port_out *outs);
int
dsp_add_bus(char *bus_id, struct dsp_bus *new_bus, char *ins, char *outs);

int
dsp_add_connection(char *id_out, char *id_in, char **new_connection_id);

int
dsp_remove_connection(char *connection_id, bool mutex);

void
dsp_optimize_connections_bus(struct dsp_bus_port *ports);

void
dsp_optimize_connections_input(struct dsp_connection *connection);

void
dsp_optimize_bus(struct dsp_bus *head_bus);

int
dsp_free_bus_descendants_recursive(struct dsp_bus *head_bus);

int
dsp_free_bus(struct dsp_bus *target_bus, bool recursive, bool mutex);

int
dsp_remove_bus(struct dsp_bus *target_bus);

void
dsp_build_mains(int channels_in, int channels_out);

void
dsp_build_optimized_main_outs();

void
*dsp_build_optimized_graph(void *arg);

void
dsp_signal_graph_cleanup();

void
dsp_process(struct dsp_operation *head_op, int jack_sr, int pos);

void
dsp_setup(unsigned short period, unsigned short channels_in, unsigned short channels_out);

#endif
