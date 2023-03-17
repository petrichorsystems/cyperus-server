
#ifndef MATH_MODULES_ENVELOPE_SEGMENT_H
#define MATH_MODULES_ENVELOPE_SEGMENT_H

#include <float.h>
#include <string.h>
#include <stddef.h>

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"

extern float
math_modules_envelope_segment(dsp_parameter *counter, int samplerate, int pos);

#endif
