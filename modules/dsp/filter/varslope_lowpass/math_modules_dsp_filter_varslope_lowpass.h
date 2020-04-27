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

#ifndef math_modules_dsp_filter_varslope_lowpass_h_
#define math_modules_dsp_filter_varslope_lowpass_h_

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"
#include "../../math_utils.h"

#define RT_PI 3.14159265358979323846

extern void
math_modules_dsp_filter_varslope_lowpass_init(dsp_module_parameters_t *parameters, int jack_sr);

extern void
math_modules_dsp_filter_varslope_lowpass_edit(dsp_module_parameters_t *parameters);

extern float
math_modules_dsp_filter_varslope_lowpass(dsp_parameter *filter, int samplerate, int pos);

#endif
