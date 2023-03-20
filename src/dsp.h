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

#include "dsp_math.h"
#include "dsp_types.h"
#include "dsp_ops.h"

#include "dsp_graph_id.h"
#include "threadsync.h"

struct dsp_bus_port*
dsp_search_bus_port(struct dsp_bus *head_bus, char *id);

struct dsp_bus_port*
dsp_find_bus_port(char *id);

struct dsp_module*
dsp_search_module(struct dsp_bus *head_bus, char *id);

struct dsp_module*
dsp_find_module(char *id);

struct dsp_port_out*
dsp_search_port_out(struct dsp_bus *head_bus, char *id);

struct dsp_port_out*
dsp_find_port_out(char *id);

struct dsp_port_in*
dsp_search_port_in(struct dsp_bus *head_bus, char *id);

struct dsp_port_in*
dsp_find_port_in(char *id);

struct dsp_port_out*
dsp_find_main_in_port_out(char *id)
  
struct dsp_port_in*
dsp_find_main_out_port_in(char *id);

struct dsp_port_out*
dsp_search_module_port_out(struct dsp_bus *head_bus, char *id);

struct dsp_port_out*
dsp_find_module_port_out(char *id)

struct dsp_port_in*
dsp_search_module_port_in(struct dsp_bus *head_bus, char *id);

struct dsp_port_in*
dsp_find_module_port_in(char *id);

struct dsp_port_out*
dsp_search_bus_port_port_out(struct dsp_bus *head_bus, char *id);

struct dsp_port_out*
dsp_find_bus_port_port_out(char *id)

struct dsp_port_in*
dsp_search_bus_port_port_in(struct dsp_bus *head_bus, char *id);

struct dsp_port_in*
dsp_find_bus_port_port_in(char *id);

struct dsp_module*
dsp_search_module_from_port(struct dsp_bus *head_bus, char *id);

struct dsp_module*
dsp_get_module_from_port(char *id);

struct dsp_bus_port*
dsp_build_bus_ports(struct dsp_bus_port *head_port, char *bus_ports, int out);

void
dsp_remove_module(struct dsp_module *module, int remove);

void
dsp_bypass_module(struct dsp_module *module, int bypass);

struct dsp_module*
dsp_add_module(struct dsp_bus *target_bus,
	       char *name,
	       void (*dsp_function) (struct dsp_operation*, int, int),
               void (*dsp_osc_listener_function) (struct dsp_operation*, int, int),
	       struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*),
	       dsp_parameter dsp_param,
	       struct dsp_port_in *ins,
	       struct dsp_port_out *outs);
void
dsp_add_bus(char *bus_id, struct dsp_bus *new_bus, char *ins, char *outs);

int
dsp_add_connection(char *id_out, char *id_in);

int
dsp_remove_connection(char *id_out, char *id_in);

struct dsp_bus*
dsp_search_bus(struct dsp_bus *head_bus, char *id);

struct dsp_bus*
dsp_find_bus(char *id);

void
dsp_optimize_connections_bus(char *current_bus_path, struct dsp_bus_port *ports);

void
dsp_optimize_connections_input(struct dsp_connection *connection);

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
