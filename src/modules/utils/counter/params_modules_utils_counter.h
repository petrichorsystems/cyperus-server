#ifndef PARAMS_MODULES_UTILS_COUNTER_H
#define PARAMS_MODULES_UTILS_COUNTER_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_RESET 0
#define PARAM_USER_START 1
#define PARAM_USER_STEP_SIZE 2
#define PARAM_USER_MIN 3
#define PARAM_USER_MAX 4
#define PARAM_USER_DIRECTION 5
#define PARAM_USER_AUTO_RESET 6

/* internal module parameters */
#define PARAM_INTERNAL_START 0

/* listener-exposed parameters */
#define PARAM_LISTENER_RESET 1
#define PARAM_LISTENER_START 2
#define PARAM_LISTENER_STEP_SIZE 3
#define PARAM_LISTENER_MIN 4
#define PARAM_LISTENER_MAX 5
#define PARAM_LISTENER_DIRECTION 6
#define PARAM_LISTENER_AUTO_RESET 7

extern void params_modules_utils_counter_init(dsp_parameter *params,
					      float reset,
					      float start,
					      float step_size,
					      float min,
					      float max,
					      float direction,
					      float auto_reset);

extern void params_modules_utils_counter_edit_pending(dsp_parameter *utils_counter,
						      float reset,
						      float start,
						      float step_size,
						      float min,
						      float max,
						      float direction,
						      float auto_reset);

extern void params_modules_utils_counter_edit_apply(dsp_parameter *utils_counter);

extern void params_modules_utils_counter_free(dsp_parameter *utils_counter);

#endif
