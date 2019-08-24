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

#include "rtqueue.h"
#include "dsp_math.h"
#include "dsp.h"

#ifndef DSP_OPS_H
#define DSP_OPS_H

float dsp_sum_input(struct dsp_port_in *in);
float dsp_sum_summands(struct dsp_operation_sample *summands);
void dsp_feed_outputs(char *current_bus_path, char *module_id, struct dsp_port_out *outs);
void dsp_optimize_connections_module(char *current_bus_path, char *module_id, struct dsp_port_out *outs);
void dsp_optimize_connections_main_inputs(struct dsp_port_out *outs);
struct dsp_operation *dsp_optimize_module(char *bus_path, struct dsp_module *module);


void dsp_create_block_processor(struct dsp_bus *target_bus);
void dsp_block_processor(struct dsp_operation *block_processor, int jack_samplerate, int pos);

int dsp_create_delay(struct dsp_bus *target_bus, float amt, float time, float feedback);
void dsp_edit_delay(struct dsp_module *delay, float amt, float time, float feedback);
void dsp_delay(struct dsp_operation *delay, int jack_samplerate, int pos);

int dsp_create_sine(struct dsp_bus *target_bus, float freq, float amp, float phase);
void dsp_edit_sine(struct dsp_module *sine, float freq, float amp, float phase);
void dsp_sine(struct dsp_operation *sine, int jack_samplerate, int pos);

int dsp_create_square(struct dsp_bus *target_bus, float freq, float amp);
void dsp_edit_square(struct dsp_module *square, float freq, float amp);
void dsp_square(struct dsp_operation *square, int jack_samplerate, int pos);

int dsp_create_envelope_follower(struct dsp_bus *target_bus, float attack, float decay, float scale);
void dsp_edit_envelope_follower(struct dsp_module *envelope_follower, float attack, float decay, float scale);
void dsp_envelope_follower(struct dsp_operation *envelope_follower, int jack_samplerate, int pos);

int dsp_create_lowpass(struct dsp_bus *target_bus, float amt, float freq, float res);
void dsp_edit_lowpass(struct dsp_module *lowpass, float amt, float freq, float res);
void dsp_lowpass(struct dsp_operation *lowpass, int jack_samplerate, int pos);

int dsp_create_pitch_shift(struct dsp_bus *target_bus, float amp, float shift, float mix);
int dsp_edit_pitch_shift(struct dsp_module *pitch_shift, float amp, float shift, float mix);
float dsp_pitch_shift(struct dsp_operation *pitch_shift, int jack_samplerate, int pos);

int dsp_create_karlsen_lowpass(struct dsp_bus *target_bus, float amp, float freq, float res);
int dsp_edit_karlsen_lowpass(struct dsp_module *karlsen_lowpass, float amp, float freq, float res);
float dsp_karlsen_lowpass(struct dsp_operation *karlsen_lowpass, int jack_samplerate, int pos);

/* ================= FUNCTIONS BELOW NEED TO BE CONVERTED TO USE dsp_* OBJECTS ==================== */
 
int dsp_create_pinknoise();
float dsp_pinknoise(dsp_parameter noise_param, int jack_samplerate, int pos);

#endif
