
#ifndef MATH_MODULES_MOVEMENT_ENVELOPE_ADSR_H
#define MATH_MODULES_MOVEMENT_ENVELOPE_ADSR_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"

extern
float math_modules_motion_envelope_follower(dsp_parameter *follower, int samplerate, int pos);

#endif
