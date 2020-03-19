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

#include "dsp_types.h"
#include "dsp_math.h"

#ifndef DSP_H
#define DSP_H

extern struct dsp_port_out *dsp_main_ins;
extern struct dsp_port_in *dsp_main_outs;

extern struct dsp_operation *dsp_optimized_main_ins;
extern struct dsp_operation *dsp_optimized_main_outs;

struct dsp_module*
dsp_find_module(struct dsp_module *head_module, char *name);

struct dsp_bus_port*
dsp_find_bus_port(struct dsp_bus_port *target_bus_port, char *name);

struct dsp_port_out*
dsp_find_port_out(struct dsp_port_out *port_out_head, char *name);

struct dsp_port_in*
dsp_find_port_in(struct dsp_port_in *port_in_head, char *name);

struct dsp_bus_port*
dsp_build_bus_ports(struct dsp_bus_port *head_port, char *bus_ports, int out);

void
dsp_remove_module(struct dsp_module *module, int remove);

void
dsp_bypass_module(struct dsp_module *module, int bypass);

void
dsp_add_module(struct dsp_bus *target_bus,
	       char *name,
	       void (*dsp_function) (char*, struct dsp_module*, int, int),
	       struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*),
	       dsp_parameter dsp_param,
	       struct dsp_port_in *ins,
	       struct dsp_port_out *outs);
void
dsp_add_bus(char *target_path, struct dsp_bus *new_bus, char *ins, char *outs);

int
dsp_add_connection(char *id_out, char *id_in);

int
dsp_remove_connection(char *id_out, char *id_in);

struct dsp_bus*
dsp_parse_bus_path(char *target_bus_path);

void
dsp_parse_path(char *result[], char *path);

void
dsp_optimize_connections_bus(char *current_bus_path, struct dsp_bus_port *ports);

void
dsp_optimize_connections_input(char *current_path, struct dsp_connection *connection);

void
dsp_optimize_graph(struct dsp_bus *head_bus, char *parent_path);

void
dsp_build_mains(int channels_in, int channels_out);

void
*dsp_build_optimized_graph(void *arg);

void
dsp_process(struct dsp_operation *head_op, int jack_sr, int pos);

void
*dsp_thread(void *arg);
#endif
