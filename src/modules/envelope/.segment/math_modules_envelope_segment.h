
#ifndef MATH_MODULES_ENVELOPE_SEGMENT_H
#define MATH_MODULES_ENVELOPE_SEGMENT_H

#include <float.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <limits.h>

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"
#include "../../math_utils.h"

#define ENVGEN_NOT_STARTED 1000000000

#define PARAM_LEVELS        0
#define PARAM_TIMES         1
#define PARAM_SHAPE         2
#define PARAM_CURVE         3
#define PARAM_RELEASE_NODE  0
#define PARAM_LOOP_NODE     1
#define PARAM_OFFSET        2
#define PARAM_GATE          0
#define PARAM_LEVEL_SCALE   1
#define PARAM_LEVEL_BIAS    2
#define PARAM_TIME_SCALE    3
#define PARAM_INIT_LEVEL    5
#define PARAM_NUM_STAGES    3

#define STATE_A1           0
#define STATE_A2           1
#define STATE_B1           2
#define STATE_Y1           3
#define STATE_Y2           4
#define STATE_GROW         5
#define STATE_END_LEVEL    6
#define STATE_COUNTER      4
#define STATE_STAGE        5
#define STATE_SHAPE        6
#define STATE_RELEASE_NODE 7
#define STATE_RELEASED     8
#define STATE_DONE         9
#define STATE_LEVEL        6
#define STATE_PREV_GATE    7

enum {
    shape_Step,
    shape_Linear,
    shape_Exponential,
    shape_Sine,
    shape_Welch,
    shape_Curve,
    shape_Squared,
    shape_Cubed,
    shape_Hold,
    shape_Sustain = 9999
};

extern void
math_modules_envelope_segment_init(dsp_module_parameters_t *parameters);

extern void
math_modules_envelope_segment_edit(dsp_module_parameters_t *parameters,
                                   int release_node,
                                   int loop_node,
                                   int offset,
                                   float gate,
                                   float level_scale,
                                   float level_bias,
                                   float time_scale);

extern float
math_modules_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos);


#endif
