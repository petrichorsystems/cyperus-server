
#ifndef MATH_MODULES_MOVEMENT_ENVELOPE_ADSR_H
#define MATH_MODULES_MOVEMENT_ENVELOPE_ADSR_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

float _calc_coeff(float rate, float target_ratio);
void _set_attack_rate(dsp_module_parameters_t *parameters, float rate);
void _set_decay_rate(dsp_module_parameters_t *parameters, float rate);
void _set_release_rate(dsp_module_parameters_t *parameters, float rate);
void _set_sustain_level(dsp_module_parameters_t *parameters, float level);
void _set_target_ratio_a(dsp_module_parameters_t *parameters, float target_ratio);
void _set_target_ratio_dr(dsp_module_parameters_t *parameters, float target_ratio);

extern void
math_modules_movement_envelope_adsr_init(dsp_module_parameters_t *parameters, int samplerate);

extern void
math_modules_movement_envelope_adsr_edit(dsp_module_parameters_t *parameters,
                                         int gate,
                                         float attack_rate,
                                         float decay_rate,
                                         float release_rate,
                                         float sustain_level,
                                         float target_ratio_a,
                                         float target_ratio_dr,
                                         float mul,
                                         float add);

extern float
math_modules_movement_envelope_adsr(dsp_module_parameters_t *parameters, int samplerate, int pos);


#endif
