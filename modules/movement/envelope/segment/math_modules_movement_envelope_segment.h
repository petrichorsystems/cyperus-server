
#ifndef MATH_MODULES_MOVEMENT_ENVELOPE_SEGMENT_H
#define MATH_MODULES_MOVEMENT_ENVELOPE_SEGMENT_H

#include <float.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include <limits.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"
#include "../../../math_utils.h"

#define ENVGEN_NOT_STARTED 1000000000

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

typedef struct env_parameters {
  float *levels;
  float *times;
  float *shape;
  float *curve;
  int release_node;
  int loop_node;
  int offset;
} env_params_t;

typedef struct env_gen_parameters {
  env_params_t *envelope;
  float gate;
  float level_scale;
  float level_bias;
  float time_scale;
  float init_level;  
  int num_stages;
  
  double a1;
  double a2;
  double b1;
  double y1;
  double y2;
  double grow;
  double end_level;

  int counter;
  int stage;
  int shape;
  int release_node;
  int released;
  int done;
  
  float level;
  float prev_gate;
} env_gen_params_t;

extern void
math_modules_movement_envelope_segment_init(dsp_module_parameters_t *parameters);

extern void
math_modules_movement_envelope_segment_edit(dsp_module_parameters_t *parameters,
                                            int release_node,
                                            int loop_node,
                                            int offset,
                                            float gate,
                                            float level_scale,
                                            float level_bias,
                                            float time_scale,
                                            float init_level,
                                            int num_stages);

extern float
math_modules_movement_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos);


#endif
