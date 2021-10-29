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

#ifndef ops_modules_dsp_filter_varslope_lowpass_h_
#define ops_modules_dsp_filter_varslope_lowpass_h_

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include <lo/lo.h>
#include "../../../../dsp.h"

#include "math_modules_audio_filter_varslope_lowpass.h"

int
dsp_create_filter_varslope_lowpass(struct dsp_bus *target_bus,
				   float amplitude,
				   float slope,
				   float cutoff_frequency);
void
dsp_filter_varslope_lowpass(struct dsp_operation *filter_varslope_lowpass,
			    int jack_samplerate,
			    int pos);
void
dsp_edit_filter_varslope_lowpass(struct dsp_module *filter_varslope_lowpass,
				 float amplitude,
				 float slope,
				 float cutoff_frequency);
#endif
