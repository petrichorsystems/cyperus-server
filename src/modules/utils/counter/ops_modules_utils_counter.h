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

#ifndef OPS_MODULES_UTILS_COUNTER_H
#define OPS_MODULES_UTILS_COUNTER_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "../../../dsp.h"
#include "../../../dsp_ops.h"
#include "../../../dsp_types.h"
#include "../../../dsp_math.h"
#include "../../../osc.h"
#include "../../../osc_handlers.h"

#include "../../../jackcli.h"

int
dsp_create_utils_counter(struct dsp_bus *target_bus,
                         float start,
                         float step_size,
                         float min,
                         float max,
                         float direction,
                         float auto_reset);
void
dsp_utils_counter(struct dsp_operation *utils_counter,
			    int jack_samplerate,
			    int pos);
void
dsp_edit_utils_counter(struct dsp_module *utils_counter,
                       float start,
                       float step_size,
                       float min,
                       float max,
                       float direction,
                       float auto_reset);

void
dsp_osc_listener_utils_counter(struct dsp_operation *utils_counter,
                                 int jack_samplerate,
                                 int pos);
#endif
