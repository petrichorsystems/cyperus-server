#ifndef PARAMS_MODULES_FILTER_BANDPASS_H
#define PARAMS_MODULES_FILTER_BANDPASS_H

#include "../../../dsp_types.h"

/* user-exposed parameters */
#define PARAM_USER_FREQUENCY 0
#define PARAM_USER_Q 1
#define PARAM_USER_AMOUNT 2

/* internal module parameters */
#define PARAM_INTERNAL_FREQUENCY_OLD 1
#define PARAM_INTERNAL_Q_OLD 2
#define PARAM_INTERNAL_LAST 3
#define PARAM_INTERNAL_PREV 4
#define PARAM_INTERNAL_COEF1 5
#define PARAM_INTERNAL_COEF2 6
#define PARAM_INTERNAL_GAIN 7

/* listener-exposed parameters */
#define PARAM_LISTENER_FREQUENCY 0
#define PARAM_LISTENER_Q 1
#define PARAM_LISTENER_AMOUNT 2

extern void params_modules_filter_bandpass_init(dsp_parameter *params,
					     float amount,
					     float time,
					     float feedback);

extern void params_modules_filter_bandpass_edit_pending(dsp_parameter *filter_bandpass,
						     float amount,
						     float time,
						     float feedback);

extern void params_modules_filter_bandpass_edit_apply(dsp_parameter *filter_bandpass);

extern void params_modules_filter_bandpass_free(dsp_parameter *filter_bandpass);

#endif
