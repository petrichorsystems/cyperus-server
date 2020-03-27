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

#include "dsp_math.h"
#include "rtqueue.h"

#ifndef DSP_TYPES_H
#define DSP_TYPES_H

typedef struct dsp_module_parameter {
  int type;
  float in;
  int pos;
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
    } lowpass;
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
  };
}dsp_parameter;

/* all possible identifiers for module parameters */
typedef enum dsp_parameter_identifiers{
  DSP_NULL_PARAMETER_ID=0,
  DSP_SINE_PARAMETER_ID=1,
  DSP_SQUARE_PARAMETER_ID=2,
  DSP_PINKNOISE_PARAMETER_ID=5,
  DSP_BUTTERWORTH_BIQUAD_LOWPASS_PARAMETER_ID=7,
  DSP_DELAY_PARAMETER_ID=8,
  DSP_VOCODER_PARAMETER_ID=9,
  DSP_BLOCK_PROCESSOR_PARAMETER_ID=10,
  DSP_PITCH_SHIFT_PARAMETER_ID=11,
  DSP_ENVELOPE_FOLLOWER_PARAMETER_ID=12,
  DSP_KARLSEN_LOWPASS_PARAMETER_ID=13,
  DSP_LOWPASS_PARAMETER_ID=14,
  DSP_HIGHPASS_PARAMETER_ID=15,
  DSP_BANDPASS_PARAMETER_ID=16,
  DSP_TRIANGLE_PARAMETER_ID=17
}dsp_param_identifiers;

struct dsp_port_in {
  const char *id;
  const char *name;
  struct dsp_port_in *next;
  struct dsp_port_in *prev;
  rtqueue_t *values;
  int remove; /* boolean remove flag */
};

struct dsp_port_out {
  const char *id;
  const char *name;
  struct dsp_port_out *next;
  struct dsp_port_out *prev;
  float value;
  int audio;
  int remove; /* boolean remove flag */
};

struct dsp_connection {
  const char *id;
  struct dsp_connection *next;
  struct dsp_connection *prev;
  const char *id_out;
  const char *id_in;
  float out_value;
  rtqueue_t *in_values;
  int remove; /* boolean remove flag */
};

struct dsp_module {
  const char *id;
  const char *name;
  struct dsp_module *next;
  struct dsp_module *prev;
  void (*dsp_function) (struct dsp_operation*, int, int);
  struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*);
  dsp_parameter dsp_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  int remove; /* boolean remove flag */
  int bypass; /* boolean bypass flag */
};

struct dsp_bus_port {
  const char *id;
  char *name;
  struct dsp_bus_port    *next;
  struct dsp_bus_port    *prev;
  struct dsp_port_in *in;
  struct dsp_port_out *out;
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
  int remove; /* boolean remove flag */
  int bypass; /* boolean bypass flag */
};

struct dsp_sample {
  const char *id;
  float value;
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

/* pre-optimized */
volatile struct dsp_connection *dsp_global_connection_graph; /* GLOBAL */
volatile struct dsp_bus *dsp_global_bus_head; /* GLOBAL */

/* optimized */
volatile struct dsp_translation_connection *dsp_global_translation_connection_graph_processing; /* GLOBAL */
volatile struct dsp_operation *dsp_global_operation_head_processing; /* GLOBAL */

volatile struct dsp_operation *dsp_global_translation_graph; /* GLOBAL */
volatile struct dsp_operation *dsp_global_operation_head; /* GLOBAL */

struct dsp_port_in* dsp_port_in_init(const char *port_name, int fifo_size);
void dsp_port_in_insert_head(struct dsp_port_in *head_port, struct dsp_port_in *port_in);
void dsp_port_in_insert_tail(struct dsp_port_in *head_port, struct dsp_port_in *port_in);

struct dsp_port_out* dsp_port_out_init(const char *port_name, int audio);
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
void dsp_connection_terminate(struct dsp_connection *connection);

struct dsp_module* dsp_module_init(const char *module_name,
				   void (*dsp_function) (char *, struct dsp_module*, int, int),
				   struct dsp_operation (*dsp_optimize) (char *, struct dsp_module*),
				   dsp_parameter dsp_param,
				   struct dsp_port_in *ins,
				   struct dsp_port_out *outs);
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
struct dsp_sample* dsp_sample_init(float value);

struct dsp_operation_sample* dsp_operation_sample_init(char *dsp_id, float value, int init_sample);
void dsp_operation_sample_insert_head(struct dsp_operation_sample *head_sample, struct dsp_operation_sample *new_sample);
void dsp_operation_sample_insert_tail(struct dsp_operation_sample *head_sample, struct dsp_operation_sample *new_sample);

#endif
