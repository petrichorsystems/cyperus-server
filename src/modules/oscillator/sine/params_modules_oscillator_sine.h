#ifndef PARAMS_MODULES_OSCILLATOR_SINE_H
#define PARAMS_MODULES_OSCILLATOR_SINE_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_FREQUENCY 0
#define PARAM_USER_AMPLITUDE 1
#define PARAM_USER_PHASE 2

/* internal module parameters */
#define PARAM_INTERNAL_PHASE_DELTA 0

/* listener-exposed parameters */
#define PARAM_LISTENER_FREQUENCY 1
#define PARAM_LISTENER_AMPLITUDE 2
#define PARAM_LISTENER_PHASE 3

extern void params_modules_oscillator_sine_init(dsp_parameter *params,
						float frequency,
						float amplitude,
						float phase);

extern void params_modules_oscillator_sine_edit_pending(dsp_parameter *sine,
							float frequency,
							float amplitude,
							float phase);

extern void params_modules_oscillator_sine_edit_apply(dsp_parameter *sine);

extern void params_modules_oscillator_sine_free(dsp_parameter *sine);

#endif
