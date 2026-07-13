#ifndef PARAMS_MODULES_OSCILLATOR_TRIANGLE_H
#define PARAMS_MODULES_OSCILLATOR_TRIANGLE_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_FREQUENCY 0
#define PARAM_USER_AMPLITUDE 1

/* internal module parameters */
#define PARAM_INTERNAL_X 0

/* listener-exposed parameters */
#define PARAM_LISTENER_FREQUENCY 0
#define PARAM_LISTENER_AMPLITUDE 1

extern void params_modules_oscillator_triangle_init(dsp_parameter *params,
						    float frequency,
						    float amplitude);

extern void params_modules_oscillator_triangle_edit_pending(dsp_parameter *oscillator_triangle,
							    float frequency,
							    float amplitude);

extern void params_modules_oscillator_triangle_edit_apply(dsp_parameter *oscillator_triangle);

extern void params_modules_oscillator_triangle_free(dsp_parameter *oscillator_triangle);

#endif
