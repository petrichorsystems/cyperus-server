#ifndef PARAMS_MODULES_OSCILLATOR_CLOCK_H
#define PARAMS_MODULES_OSCILLATOR_CLOCK_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_FREQUENCY 0
#define PARAM_USER_AMPLITUDE 1

/* internal module parameters */
#define PARAM_INTERNAL_SAMPLES_COUNT 0

/* listener-exposed parameters */
#define PARAM_LISTENER_FREQUENCY 0
#define PARAM_LISTENER_AMPLITUDE 1

extern void params_modules_oscillator_clock_init(dsp_parameter *params,
						 float frequency,
						 float amplitude);

extern void params_modules_oscillator_clock_edit_pending(dsp_parameter *oscillator_clock,
							 float frequency,
							 float amplitude);

extern void params_modules_oscillator_clock_edit_apply(dsp_parameter *oscillator_clock);

extern void params_modules_oscillator_clock_free(dsp_parameter *oscillator_clock);

#endif
