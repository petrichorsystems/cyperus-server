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

#ifndef OPS_MODULES_MOVEMENT_OSC_METRONOME_H
#define OPS_MODULES_MOVEMENT_OSC_METRONOME_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "../../../../dsp_types.h"
#include "../../../../dsp_ops.h"
#include "../../../../jackcli.h"
#include "../../../../osc.h"

#include <lo/lo.h>



extern int
dsp_create_osc_metronome(struct dsp_bus *target_bus,
                         float beats_per_minute);
extern void
dsp_osc_metronome(struct dsp_operation *osc_metronome,
                  int jack_samplerate,
                  int pos);
extern void
dsp_edit_osc_metronome(struct dsp_module *osc_metronome,
                       float beats_per_minute);
#endif
