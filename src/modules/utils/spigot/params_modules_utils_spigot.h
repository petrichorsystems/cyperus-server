#ifndef PARAMS_MODULES_UTILS_SPIGOT_H
#define PARAMS_MODULES_UTILS_SPIGOT_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_OPEN 0

/* internal module parameters */
#define PARAM_INTERNAL_LAST_OPEN 0 /* unused for now, probably will be used in listener or something */

/* listener-exposed parameters */

extern void params_modules_utils_spigot_init(dsp_parameter *params,
					     float open);

extern void params_modules_utils_spigot_edit_pending(dsp_parameter *utils_spigot,
						     float open);

extern void params_modules_utils_spigot_edit_apply(dsp_parameter *utils_spigot);

extern void params_modules_utils_spigot_free(dsp_parameter *utils_spigot);

#endif
