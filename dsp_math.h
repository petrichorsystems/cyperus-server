/* libcyperus.h
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

#include "rtqueue.h"

#ifndef LIBCYPERUS_H
#define LIBCYPERUS_H

#define VOCODERSIGNALSIZE 2048
#define VOCODERHOPSIZE 256
#define VOCODERFRAMESIZE 2048
#define VOCODEROVERSAMPLE 32

struct cyperus_parameters {
  /* block processing */
  rtqueue_t block_fifo;

  /* generic */
  float freq;
  float amp;
  float delta;
  float mix;
  float res; /* resonance */
  float shift;
  float fb; /* feedback */
  float delay_amt; /* delay amount, 0-1 */
  float delay_time; /* init this with
		       = seconds * sample_rate */
  float attack;
  float decay;
  float scale;
  
  int pos;
  int delay_pos;
  float avg;
  float max;
  int vocoder_pos;

  /* sine */
  int exp;
  int skip;
  int skip_count;
  float last_freq;
  float hold_freq;
  float phase_delta;
  float phase;
  
  /* phase vocoders */
  float *fft_buffer;
  float *last_phase;
  float *sum_phase;
  float *output_accumulator;
  
  float *signal_buffer;
  float *signal_buffer_out;
  
  float in;
  float state0;
  float state1;
  float state2;
  float tempval;
  float lastinval;
  float lastinval1;
  float lastinval2;
  float lastinval3;
  float lastoutval;
  float lastoutval1;
  float lastoutval2;
  float lastoutval3;
};
#endif

float cyperus_block_processor(struct cyperus_parameters *block_processor, int jack_sr, int pos);
float cyperus_envelope_follower(struct cyperus_parameters *envelope_follower, int jack_sr, int pos);

/* signal generators */
float cyperus_sine(struct cyperus_parameters *sinewav, int jack_sr, int pos);
float cyperus_square(struct cyperus_parameters *squarewav, int jack_sr, int pos);
float cyperus_triangle(struct cyperus_parameters *triwav, int jack_sr, int pos);
float cyperus_whitenoise(struct cyperus_parameters *noiz, int jack_sr, int pos);
float cyperus_pinknoise(struct cyperus_parameters *noiz, int jack_sr, int pos);

/* signal processors */
float cyperus_karlsen_lowpass(struct cyperus_parameters *filtr, int jack_sr, int pos);
float cyperus_butterworth_biquad_lowpass(struct cyperus_parameters *filtr, int jack_sr, int pos);

float cyperus_butterworth_biquad_hipass(struct cyperus_parameters *filtr, int jack_sr, int pos);
float cyperus_moog_vcf(struct cyperus_parameters *filtr, int jack_sr, int pos);

float cyperus_delay(struct cyperus_parameters *effect, int jack_sr, int pos);

float cyperus_pitch_shift(struct cyperus_parameters *pitch_shift, int jack_sr, int pos);