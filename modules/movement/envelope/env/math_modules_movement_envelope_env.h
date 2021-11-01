
#ifndef MATH_MODULES_MOVEMENT_ENVELOPE_ENV_H
#define MATH_MODULES_MOVEMENT_ENVELOPE_ENV_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

extern void
math_modules_movement_envelope_env_init(dsp_module_parameters_t *parameters,
                                         float attack_rate,
                                         float decay_rate,
                                         float release_rate,
                                         float sustain_level,
                                         float target_ratio_a,
                                         float target_ratio_dr,
                                         float mul,
                                         float add);

extern void
math_modules_movement_envelope_env_edit(dsp_module_parameters_t *parameters,
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
math_modules_movement_envelope_env(dsp_module_parameters_t *parameters, int samplerate, int pos);


#endif
