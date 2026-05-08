#ifndef PARAMS_MODULES_DELAY_SIMPLE_H
#define PARAMS_MODULES_DELAY_SIMPLE_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_AMOUNT 0
#define PARAM_USER_TIME 1
#define PARAM_USER_FEEDBACK 2

/* internal module parameters */
#define PARAM_INTERNAL_TIME_SAMPLES 0
#define PARAM_INTERNAL_DELAY_POS 0
#define PARAM_INTERNAL_DELAY_TIME_POS 1

/* listener-exposed parameters */
#define PARAM_LISTENER_AMOUNT 0
#define PARAM_LISTENER_TIME 1
#define PARAM_LISTENER_FEEDBACK 2

/* internal sample buffer */
#define PARAM_INTERNAL_SAMPLE_BUFFER 3

extern void params_modules_delay_simple_init(dsp_parameter *params,
					     float amount,
					     float time,
					     float feedback);

extern void params_modules_delay_simple_edit_pending(dsp_parameter *delay_simple,
						     float amount,
						     float time,
						     float feedback);

extern void params_modules_delay_simple_edit_apply(dsp_parameter *delay_simple);

extern void params_modules_delay_simple_free(dsp_parameter *delay_simple);

#endif
