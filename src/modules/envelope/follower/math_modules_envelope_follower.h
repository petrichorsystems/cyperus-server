
#ifndef MATH_MODULES_ENVELOPE_FOLLOWER_H
#define MATH_MODULES_ENVELOPE_FOLLOWER_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"

extern
float *math_modules_envelope_follower(dsp_parameter *follower, int samplerate);

#endif
