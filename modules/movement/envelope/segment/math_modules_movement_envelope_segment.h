
#ifndef MATH_MODULES_MOVEMENT_ENVELOPE_SEGMENT_H
#define MATH_MODULES_MOVEMENT_ENVELOPE_SEGMENT_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../../jackcli.h"
#include "../../../../dsp_math.h"
#include "../../../../dsp_types.h"
#include "../../../math_utils.h"

typedef struct env_gen_parameters {
  float **envs;
  float gate;
  float level_scale;
  float level_bias;
  float time_scale;
  float done_action;
  float init_level;
  float num_stages;
  int release_node;
  int loop_node;
  int node_offset;
} env_gen_params_t;

typedef struct env_parameters {
  double a1;
  double a2;
  dobule b1;
  double y1;
  double y2;
  double grow;
  double level;
  double end_level;
  int counter;
  int stage;
  int shape;
  int release_node;
  int released;
  float prev_gate;
} env_params_t;

extern void
math_modules_movement_envelope_segment_init(dsp_module_parameters_t *parameters,
                                         float attack_rate,
                                         float decay_rate,
                                         float release_rate,
                                         float sustain_level,
                                         float target_ratio_a,
                                         float target_ratio_dr,
                                         float mul,
                                         float add);

extern void
math_modules_movement_envelope_segment_edit(dsp_module_parameters_t *parameters,
                                         int gate,
                                         float attack_rate,
                                         float decay_rate,
                                         float release_rate,
                                         float sustain_level,
                                         float target_ratio_a,
                                         float target_ratio_dr,
                                         float mul,
                                         float add);

extern float
math_modules_movement_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos);


#endif
