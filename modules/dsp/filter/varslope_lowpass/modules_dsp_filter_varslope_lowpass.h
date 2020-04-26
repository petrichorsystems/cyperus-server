/*
 * cyperus_lowpass_module.h
 *
 * Code generation for model "cyperus_lowpass_module".
 *
 * Model version              : 1.2
 * Simulink Coder version : 9.1 (R2019a) 23-Nov-2018
 * C source code generated on : Mon Apr 13 12:16:16 2020
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objective: Execution efficiency
 * Validation result: Not run
 */

#ifndef modules_dsp_filter_varslope_lowpass_h_
#define modules_dsp_filter_varslope_lowpass_h_

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

/* Shared type includes */
#include "../../math_utils.h"

#define RT_PI 3.14159265358979323846

struct cyperus_parameters {
  /* block processing */
  /* rtqueue_t block_fifo; */

  /* generic */
  float freq;
  float amp;
  float amt;
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

  float q;
  
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
  float state3;
  float state4;
  float state5;
  float state6;
  float state7;
  float state8;
  
  float tempval;

  float lastinval;
  float lastinval1;
  float lastinval2;
  float lastinval3;
  float lastoutval;
  float lastoutval1;
  float lastoutval2;
  float lastoutval3;

  float x0;
  float x1;
  float x2;
  float x3;
  float x4;

  float y0;
  float y1;
  float y2;
  float y3;
  float y4;

  int slope;
  float fc;
  float B[12];
  float A[8];

  float W0_FILT_STATES[16];
};

extern void modules_dsp_filter_varslope_lowpass_init(struct cyperus_parameters *filter, int jack_sr);
extern float modules_dsp_filter_varslope_lowpass(struct cyperus_parameters *filter, int samplerate, int pos);
extern void modules_dsp_filter_varslope_lowpass_edit(struct cyperus_parameters *filter);

#endif
