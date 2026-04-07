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

#ifndef OPS_MODULES_DELAY_SIMPLE_H
#define OPS_MODULES_DELAY_SIMPLE_H

#include "../../../dsp_types.h"

int
dsp_create_delay_simple(struct dsp_bus *target_bus,
                         float amount,
                         float time,
                         float feedback);

int
dsp_destroy_delay_simple(struct dsp_module *target_module);

void
dsp_delay_simple(struct dsp_operation *delay_simple,
                  int jack_samplerate);
void
dsp_edit_delay_simple(struct dsp_module *delay_simple,
                       float amount,
                       float time,
                       float feedback);                          

void
dsp_osc_listener_delay_simple(struct dsp_operation *delay_simple,
                              int jack_samplerate);

#endif
