#ifndef PARAMS_MODULES_UTILS_FLOAT_H
#define PARAMS_MODULES_UTILS_FLOAT_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_FLOAT_VAL 0

/* listener-exposed parameters */
#define PARAM_LISTENER_FLOAT_VAL 0

extern void params_modules_utils_float_init(dsp_parameter *params,
					    float value);

extern void params_modules_utils_float_edit_pending(dsp_parameter *utils_float,
						    float value);

extern void params_modules_utils_float_edit_apply(dsp_parameter *utils_float);

extern void params_modules_utils_float_free(dsp_parameter *utils_float);

#endif
