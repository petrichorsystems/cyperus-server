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

#ifndef ops_modules_audio_analysis_transient_detector_h_
#define ops_modules_audio_analysis_transient_detector_h_

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include <lo/lo.h>

#include "../../../../dsp_types.h"
#include "../../../../jackcli.h"
#include "../../../../dsp.h"
#include "../../../../dsp_ops.h"
#include "../../../threadsync.h"


extern int
dsp_create_transient_detector(struct dsp_bus *target_bus,
			      float sensitivity,
			      float attack_ms,
			      float decay_ms,
			      float scale);
extern void
dsp_transient_detector(struct dsp_operation *transient_detector,
		       int jack_samplerate,
		       int pos);
extern void
dsp_edit_transient_detector(struct dsp_module *transient_detector,
			    float sensitivity,
			    float attack_ms,
			    float decay_ms,
			    float scale);
#endif
