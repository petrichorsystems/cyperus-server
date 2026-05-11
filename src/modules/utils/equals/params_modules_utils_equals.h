#ifndef PARAMS_MODULES_UTILS_EQUALS_H
#define PARAMS_MODULES_UTILS_EQUALS_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_X 0
#define PARAM_USER_Y 1

/* internal module parameters */

/* listener-exposed parameters */

extern void params_modules_utils_equals_init(dsp_parameter *params,
					     float x,
					     float y);

extern void params_modules_utils_equals_edit_pending(dsp_parameter *utils_equals,
						     float x,
						     float y);

extern void params_modules_utils_equals_edit_apply(dsp_parameter *utils_equals);

extern void params_modules_utils_equals_free(dsp_parameter *utils_equals);

#endif
