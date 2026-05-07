#ifndef PARAMS_MODULES_ENVELOPE_FOLLOWER_H
#define PARAMS_MODULES_ENVELOPE_FOLLOWER_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_ATTACK_MS 0
#define PARAM_USER_DECAY_MS 1
#define PARAM_USER_SCALE 2

/* internal module parameters */
#define PARAM_INTERNAL_LAST_OUTPUT 0
#define PARAM_INTERNAL_ATTACK_MS 3
#define PARAM_INTERNAL_DECAY_MS 4
#define PARAM_INTERNAL_COEFF_ATTACK 5
#define PARAM_INTERNAL_COEFF_DECAY 6
#define PARAM_INTERNAL_ATTACK_MS_CONNECTED 0
#define PARAM_INTERNAL_DECAY_MS_CONNECTED 1

/* listener-exposed parameters */
#define PARAM_LISTENER_ATTACK_MS 1
#define PARAM_LISTENER_DECAY_MS 2
#define PARAM_LISTENER_SCALE 3

extern void params_modules_envelope_follower_init(dsp_parameter *params,
						  float attack,
						  float decay,
						  float scale);

extern void params_modules_envelope_follower_edit_pending(dsp_parameter *follower,
							  float attack_ms,
							  float decay_ms,
							  float scale);

extern void params_modules_envelope_follower_edit_apply(dsp_parameter *follower);

extern void params_modules_envelope_follower_free(dsp_parameter *follower);

#endif
