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

#include "libcyperus.h"
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
      float cutoff;
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
  DSP_PITCH_SHIFT_PARAMETER_ID=11
}dsp_param_identifiers;

struct dsp_port_in {
  const char *name;
  struct dsp_port_in *next;
  struct dsp_port_in *prev;
  rtqueue_t *values;
  int remove; /* boolean remove flag */
};

struct dsp_port_out {
  const char *name;
  struct dsp_port_out *next;
  struct dsp_port_out *prev;
  float value;
  int audio;
  int remove; /* boolean remove flag */
};

struct dsp_connection {
  struct dsp_connection *next;
  struct dsp_connection *prev;
  const char *id_out;
  const char *id_in;
  float out_value;
  rtqueue_t *in_values;
  int remove; /* boolean remove flag */
};

struct dsp_module {
  const char *name;
  struct dsp_module *next;
  struct dsp_module *prev;
  void (*dsp_function) (struct dsp_module*, int, int);
  dsp_parameter dsp_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  int remove; /* boolean remove flag */
  int bypass; /* boolean bypass flag */
};

struct dsp_bus_port {
  char *name;
  struct dsp_bus_port    *next;
  struct dsp_bus_port    *prev;
  struct dsp_port_in *in;
  struct dsp_port_out *out;
  int output; /* boolean "is output" flag */
  int remove; /* boolean remove flag */
};
  
struct dsp_bus {
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

struct dsp_bus *dsp_global_bus_head; /* GLOBAL */
struct dsp_connection *dsp_global_connection_graph; /* GLOBAL */

struct dsp_port_in* dsp_port_in_init(const char *port_name, int fifo_size);
void dsp_port_in_insert_head(struct dsp_port_in *head_port, struct dsp_port_in *port_in);
void dsp_port_in_insert_tail(struct dsp_port_in *head_port, struct dsp_port_in *port_in);

struct dsp_port_out* dsp_port_out_init(const char *port_name, int audio);
void dsp_port_out_insert_head(struct dsp_port_out *head_port, struct dsp_port_out *port_out);
void dsp_port_out_insert_tail(struct dsp_port_out *head_port, struct dsp_port_out *port_out);

struct
dsp_connection* dsp_connection_init(const char *id_out,
				    const char *id_in,
				    struct dsp_port_out *port_out,
				    struct dsp_port_in *port_in);

void
dsp_connection_insert_head(struct dsp_connection *head_connection, struct dsp_connection *connection);
void
dsp_connection_insert_tail(struct dsp_connection *head_connection, struct dsp_connection *connection);
void
dsp_connection_list(struct dsp_connection *head_connection);
void
dsp_connection_list_reverse(struct dsp_connection *head_connection);

struct dsp_module* dsp_module_init(const char *module_name,
				   void (*dsp_function) (struct dsp_module*, int, int),
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

#endif
