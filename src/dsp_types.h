/* dsp_types.h
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

#ifndef DSP_TYPES_H
#define DSP_TYPES_H

#include <stdbool.h>
#include <stdlib.h>
#include <uuid/uuid.h>

#include "dsp_math.h"

struct dsp_global_t {
	unsigned short period;
	float cpu_load;

	bool build_new_optimized_graph;
	bool new_operation_graph;
	bool graph_cleanup_do;
	
	pthread_mutex_t graph_state_mutex;
	pthread_mutex_t optimization_mutex;
	
	pthread_mutex_t optimization_condition_mutex;
	pthread_cond_t optimization_condition_cond;

	pthread_mutex_t graph_cleanup_condition_mutex;
	pthread_cond_t graph_cleanup_condition_cond;	
	
	struct dsp_operation *operation_head_processing;
	struct dsp_operation *operation_head;

	struct dsp_connection *connection_graph;
	struct dsp_connection *connection_graph_processing;
	struct dsp_translation_connection *translation_connection_graph_processing;

	struct dsp_bus *bus_head;

	struct dsp_port_out *main_ins;
	struct dsp_port_in *main_outs;

	struct dsp_operation *optimized_main_ins;
	struct dsp_operation *optimized_main_outs;
	struct dsp_operation *rebuilt_optimized_main_outs;	
};
extern struct dsp_global_t dsp_global;

typedef struct dsp_module_parameter {
  float *in;
  
  int pos;

  char *name;
  dsp_module_parameters_t *parameters;
  
  union {
    struct {
      char *name;
    } null;
    struct {
      char *name;
      struct cyperus_parameters *cyperus_params;
    } block_processor;
    struct {
      char *name;
      struct cyperus_parameters *cyperus_params;
      float freq;
      float amp;
      float phase;
    } sine;
    struct {
      char *name;
      struct cyperus_parameters *cyperus_params;
      float freq;
      float amp;
    } square;
    struct {
      char *name;
      struct cyperus_parameters *cyperus_params;
    } pinknoise;
    struct {
      char *name;
      float amp;
      float freq;
      float res;
      struct cyperus_parameters *cyperus_params;
    } butterworth_biquad_lowpass;
    struct {
      char *name;
      float amt; /* 0-1 */
      float time; /* seconds */
      float feedback;
      struct cyperus_parameters *cyperus_params;
    } delay;
    struct {
      char *name;
      float amp; /* 0-1 */
      float freq; /* hertz */
      struct cyperus_parameters *cyperus_params;
    } vocoder;
    struct {
      char *name;
      float amp; /* 0-1 */
      float mix; /* 0-1, dry/wet mix */
      float shift; /* 1.0 -> 2.0 octave up,  1.0 -> 0.0 octave down */
      struct cyperus_parameters *cyperus_params;
    } pitch_shift;
    struct {
      char *name;
      float attack; /* ms */
      float decay; /* ms */
      float scale;
      struct cyperus_parameters *cyperus_params;
    } envelope_follower;
    struct {
      char *name;
      float amp; /* range 0.0/1.0 */
      float freq; /* range 20/20000hz */
      float res; /* range -25/25db */
      struct cyperus_parameters *cyperus_params;
    } karlsen_lowpass;
    struct {
      char *name;
      float amt;
      float freq; 
      struct cyperus_parameters *cyperus_params;
    } highpass;
    struct {
      char *name;
      float amt;
      float freq;
      float q;
      struct cyperus_parameters *cyperus_params;
    } bandpass;
    struct {
      char *name;
      struct cyperus_parameters *cyperus_params;
      float freq;
      float amp;
    } triangle;
    struct {
      char *name;
      struct cyperus_parameters *cyperus_params;
      float freq;
      float amp;
    } sawtooth;
  };
}dsp_parameter;

struct dsp_port_in {
  const char *id;
  const char *name;
  struct dsp_port_in *next;
  struct dsp_port_in *prev;
  int remove; /* boolean remove flag */
};

struct dsp_port_out {
  const char *id;
  const char *name;
  struct dsp_port_out *next;
  struct dsp_port_out *prev;
  float value;
  int remove; /* boolean remove flag */
};

struct dsp_connection {
  const char *id;
  struct dsp_connection *next;
  struct dsp_connection *prev;
  const char *id_out;
  const char *id_in;
  int remove; /* boolean remove flag */
};

struct dsp_module {
  const char *id;
  const char *name;
  struct dsp_module *next;
  struct dsp_module *prev;
  void (*dsp_function) (struct dsp_operation*, int);
  int (*dsp_destroy_function) (struct dsp_module*);	
  void (*dsp_osc_listener_function) (struct dsp_operation*, int);
  struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*);
  dsp_parameter dsp_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  int remove; /* boolean remove flag */
  int bypass; /* boolean bypass flag */
  unsigned short osc_listener; /* boolean osc listener flag */
};

struct dsp_bus_port {
	const char *id;
	char *name;
	struct dsp_bus_port    *next;
	struct dsp_bus_port    *prev;
	struct dsp_port_in *in;
	struct dsp_port_out *out;

	struct dsp_bus *parent_bus;
	
	int output; /* boolean "is output" flag */
	int remove; /* boolean remove flag */
};
  
struct dsp_bus {
  const char *id;
  const char *name;
  struct dsp_bus *next;
  struct dsp_bus *prev;
  struct dsp_bus *down;
  struct dsp_bus *up;
  struct dsp_module *dsp_module_head;
  struct dsp_bus_port *ins;
  struct dsp_bus_port *outs;
  bool remove; /* boolean remove flag */
  bool bypass; /* boolean bypass flag */
};

struct dsp_sample {
  const char *id;
  float *value;
};

struct dsp_operation_sample {
  const char *id;
  const char *dsp_id;
  struct dsp_operation_sample *next;
  struct dsp_operation_sample *prev;
  struct dsp_operation_sample *summands;
  struct dsp_sample *sample;
};

struct dsp_operation {
	const char *id;
	const char *dsp_id;
	struct dsp_operation *next;
	struct dsp_operation *prev;
	struct dsp_module *module;
	struct dsp_operation_sample *ins;
	struct dsp_operation_sample *outs;
};

struct dsp_translation_connection {
  char *id_out;
  char *id_in;
  struct dsp_connection *connection;  
  struct dsp_operation *operation_out;
  struct dsp_operation *operation_in;
  struct dsp_operation_sample *sample_out;
  struct dsp_operation_sample *sample_in;
  struct dsp_translation_connection *next;
  struct dsp_translation_connection *prev;
};

struct dsp_translation_sample {
  char *id_in;
  char *id_out;
  struct dsp_operation_sample *sample_in;
  struct dsp_operation_sample *sample_out;
};

char *dsp_generate_object_id();

struct dsp_port_in* dsp_port_in_init(const char *port_name);
void dsp_port_in_insert_head(struct dsp_port_in *head_port, struct dsp_port_in *port_in);
void dsp_port_in_insert_tail(struct dsp_port_in *head_port, struct dsp_port_in *port_in);

struct dsp_port_out* dsp_port_out_init(const char *port_name);
void dsp_port_out_insert_head(struct dsp_port_out *head_port, struct dsp_port_out *port_out);
void dsp_port_out_insert_tail(struct dsp_port_out *head_port, struct dsp_port_out *port_out);

struct dsp_connection* dsp_connection_init(const char *id_out,
					   const char *id_in,
					   struct dsp_port_out *port_out,
					   struct dsp_port_in *port_in);

void dsp_connection_insert_head(struct dsp_connection *head_connection, struct dsp_connection *connection);
void dsp_connection_insert_tail(struct dsp_connection *head_connection, struct dsp_connection *connection);
void dsp_connection_list(struct dsp_connection *head_connection, void (*func)(struct dsp_connection*) );
void dsp_connection_list_reverse(struct dsp_connection *head_connection, void (*func)(struct dsp_connection*) );
void dsp_connection_printf(struct dsp_connection *connection);
void dsp_connection_free(struct dsp_connection *connection);

struct dsp_module* dsp_module_init(const char *module_name,
				   void (*dsp_function) (struct dsp_operation*, int),
				   int (*dsp_destroy_function) (struct dsp_module*),
                                   void (*dsp_osc_listener_function) (struct dsp_operation*, int),
				   struct dsp_operation *(*dsp_optimize) (char *, struct dsp_module*),
				   dsp_parameter dsp_param,
				   struct dsp_port_in *ins,
				   struct dsp_port_out *outs);

int dsp_module_free(struct dsp_module *target_module);

void dsp_module_insert_head(struct dsp_module *head_module, struct dsp_module *new_module);
void dsp_module_insert_tail(struct dsp_module *head_module, struct dsp_module *new_module);

struct dsp_bus_port* dsp_bus_port_init(char *port_name, int output);
void dsp_bus_port_insert_head(struct dsp_bus_port *head_port, struct dsp_bus_port *bus_port);
void dsp_bus_port_insert_tail(struct dsp_bus_port *head_port, struct dsp_bus_port *bus_port);

struct dsp_bus* dsp_bus_init(const char *bus_name);
void dsp_bus_insert_head(struct dsp_bus *head_bus, struct dsp_bus *new_bus);
void dsp_bus_insert_tail(struct dsp_bus *head_bus, struct dsp_bus *new_bus);
void dsp_bus_insert_tail_deep(struct dsp_bus *head_bus, struct dsp_bus *new_bus);

struct dsp_translation_connection* dsp_translation_connection_init(struct dsp_connection *connection,
								   char *id_out,
								   char *id_in,
								   struct dsp_operation *op_out,
								   struct dsp_operation *op_in,
								   struct dsp_operation_sample *sample_out,
								   struct dsp_operation_sample *sample_in);

void dsp_translation_connection_insert_tail(struct dsp_translation_connection *head_translation_connection, struct dsp_translation_connection *new_translation_connection);

struct dsp_operation* dsp_operation_init();
void dsp_operation_insert_head(struct dsp_operation *head_operation, struct dsp_operation *new_operation);
void dsp_operation_insert_tail(struct dsp_operation *head_operation, struct dsp_operation *new_operation);
void dsp_operation_insert_behind(struct dsp_operation *existing_operation, struct dsp_operation *new_operation);
void dsp_operation_insert_ahead(struct dsp_operation *existing_operation, struct dsp_operation *new_operation);
struct dsp_sample* dsp_sample_init(unsigned short blocksize, float value);

struct dsp_operation_sample* dsp_operation_sample_init(char *dsp_id, unsigned short blocksize, float value, int init_sample);
void dsp_operation_sample_insert_head(struct dsp_operation_sample *head_sample, struct dsp_operation_sample *new_sample);
void dsp_operation_sample_insert_tail(struct dsp_operation_sample *head_sample, struct dsp_operation_sample *new_sample);

struct dsp_bus_port*
dsp_find_bus_port(char *id);

struct dsp_bus_port*
dsp_find_bus_port_out(char *id);

struct dsp_bus_port*
dsp_find_bus_port_in(char *id);

struct dsp_module*
dsp_find_module(char *id);

struct dsp_bus*
dsp_find_bus(char *id);

struct dsp_port_out*
dsp_find_port_out(char *id);

struct dsp_port_in*
dsp_find_port_in(char *id);

struct dsp_port_out*
dsp_find_main_in_port_out(char *id);
  
struct dsp_port_in*
dsp_find_main_out_port_in(char *id);

struct dsp_port_out*
dsp_find_module_port_out(char *id);

struct dsp_port_in*
dsp_find_module_port_in(char *id);

struct dsp_port_out*
dsp_find_bus_port_port_out(char *id);

struct dsp_port_in*
dsp_find_bus_port_port_in(char *id);

struct dsp_module*
dsp_get_module_from_port(char *id);

#endif
