/* math_modules_movement_envelope_segment.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2021 murray foster */


#include "math_modules_movement_envelope_segment.h"

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

// called by nextSegment and check_gate:
// - counter: num samples to next segment
// - level: current envelope value
// - dur: if supplied and >= 0, stretch segment to last dur seconds (used in forced release)
int _init_segment(int samplerate, dsp_module_parameters_t *parameters, double dur) {
  // Print("stage %d\n", unit->m_stage);
  // Print("initSegment\n");
  // out = unit->m_level;
  float counter;
  float level;

  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
  
  float previous_end_level = envelope_gen->end_level;
  if (envelope_gen->shape == shape_Hold)
    level = previous_end_level;
  double end_level = envelope_gen->envelope->levels[0]  * envelope_gen->level_scale + envelope_gen->level_bias; // scale levels
  if (dur < 0)
    dur = envelope_gen->envelope->times[0] * envelope_gen->time_scale;
  envelope_gen->shape = (int)envelope_gen->shape;
  double curve = (double)envelope_gen->envelope->curve[0];
  envelope_gen->end_level = end_level;

  counter = (int)(dur * samplerate);
  counter = modules_math_sc_max(1, counter);

  // Print("counter %d stageOffset %d   level %g   end_level %g   dur %g   shape %d   curve %g\n", counter,
  // stageOffset, level, end_level, dur, unit->m_shape, curve); Print("SAMPLERATE %g\n", SAMPLERATE);
  if (counter == 1)
    envelope_gen->shape = 1; // shape_Linear
  // Print("new counter = %d  shape = %d\n", counter, unit->m_shape);
  switch (envelope_gen->shape) {
  case shape_Step: {
    level = end_level;
  } break;
  case shape_Hold: {
    level = previous_end_level;
  } break;
  case shape_Linear: {
    envelope_gen->shape = (end_level - level) / counter;
    // Print("grow %g\n", unit->m_grow);
  } break;
  case shape_Exponential: {
    envelope_gen->shape = pow(end_level / level, 1.0 / counter);
  } break;
  case shape_Sine: {
    double w = M_PI / counter;
    
    envelope_gen->a2 = (end_level + level) * 0.5;
    envelope_gen->b1 = 2.0 * cos(w);
    envelope_gen->y1 = (end_level - level) * 0.5;
    envelope_gen->y2 = envelope_gen->y1 * sin(M_PI * 0.5 - w);
    level = envelope_gen->a2 - envelope_gen->y1;
  } break;
  case shape_Welch: {
    double w = (M_PI * 0.5) / counter;
    
    envelope_gen->b1 = 2.0 * cos(w);

    if (end_level >= level) {
      envelope_gen->a2 = level;
      envelope_gen->y1 = 0.;
      envelope_gen->y2 = -sin(w) * (end_level - level);
    } else {
      envelope_gen->a2 = end_level;
      envelope_gen->y1 = level - end_level;
      envelope_gen->y2 = cos(w) * (level - end_level);
    }
    level = envelope_gen->a2 + envelope_gen->y1;
  } break;
  case shape_Curve: {
    if (fabs(curve) < 0.001) {
      envelope_gen->shape = 1; // shape_Linear
      envelope_gen->grow = (end_level - level) / counter;
    } else {
      double a1 = (end_level - level) / (1.0 - exp(curve));
      envelope_gen->a2 = level + a1;
      envelope_gen->b1 = a1;
      envelope_gen->grow = exp(curve / counter);
    }
  } break;
  case shape_Squared: {
    envelope_gen->y1 = sqrt(level);
    envelope_gen->y2 = sqrt(end_level);
    envelope_gen->grow = (envelope_gen->y2 - envelope_gen->y1) / counter;
  } break;
  case shape_Cubed: {
    envelope_gen->y1 = pow(level, 1.0 / 3.0); // 0.33333333);
    envelope_gen->y2 = pow(end_level, 1.0 / 3.0);
    envelope_gen->grow = (envelope_gen->y2 - envelope_gen->y1) / counter;
  } break;
  };

  envelope_gen->counter = counter;
  envelope_gen->level = level;
  return 1;
}

int _check_gate_passthru(int samplerate, dsp_module_parameters_t *parameters) {
    return 1;
}

int _check_gate(int samplerate, dsp_module_parameters_t *parameters) {
  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
  float prev_gate = envelope_gen->prev_gate;
  float gate = envelope_gen->gate;
  float offset = envelope_gen->envelope->offset;
  float counter;
    if (prev_gate <= 0.0f && gate > 0.0f) {
        envelope_gen->stage = -1;
        envelope_gen->released = 0;
        envelope_gen->done = 0;
        envelope_gen->counter = offset;
        return 0;
    } else if (gate <= -1.0f && prev_gate > -1.0f) {
        // forced release: jump to last segment overriding its duration
        double dur = -gate - 1.0f;
        envelope_gen->counter = (int)(dur * samplerate);
        envelope_gen->counter = modules_math_sc_max(1, envelope_gen->counter) + offset;
        envelope_gen->stage = envelope_gen->num_stages - 1;
        envelope_gen->released = 1;
        _init_segment(samplerate, parameters, dur);
        return 0;
    } else if (prev_gate > 0.0f && gate <= 0.0f && envelope_gen->release_node >= 0 && !envelope_gen->released) {
        envelope_gen->counter = offset;
        envelope_gen->stage = envelope_gen->release_node - 1;
        envelope_gen->released = 1;
        return 0;
    }
    return 1;
}

int _check_gate_ar(int samplerate, dsp_module_parameters_t *parameters) {
  const int result = _check_gate(samplerate, parameters);
  if (!result) {
    env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
    --(envelope_gen->gate);
  }
  return result;
}

int _next_segment(int samplerate, dsp_module_parameters_t *parameters) {
  // Print("stage %d rel %d\n", parameters->bytes_type->stage, (int)ZIN0(kEnvGen_release_node));
  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
  
  int numstages = (int)envelope_gen->num_stages;

  // Print("stage %d   numstages %d\n", envelope_gen->stage, numstages);
  if (envelope_gen->stage + 1 >= numstages) { // num stages
    // Print("stage+1 > num stages\n");
    envelope_gen->counter = INT_MAX;
    envelope_gen->shape = 0;
    envelope_gen->level = envelope_gen->end_level;
    envelope_gen->done = 1;
    
    /* done logic here */
    
  } else if (envelope_gen->stage == ENVGEN_NOT_STARTED) {
    envelope_gen->counter = INT_MAX;
    return 1;
  } else if (envelope_gen->stage + 1 == (envelope_gen->release_node) && !envelope_gen->released) { // sustain stage
    int loop_node = (int)envelope_gen->envelope->loop_node;
    if (loop_node >= 0 && loop_node < numstages) {
      envelope_gen->stage = loop_node;
      return _init_segment(samplerate, parameters, -1);
    } else {
      envelope_gen->counter = INT_MAX;
      envelope_gen->shape = shape_Sustain;
      envelope_gen->level = envelope_gen->end_level;
    }
    // Print("sustain\n");
  } else {
    envelope_gen->stage++;
    return _init_segment(samplerate, parameters, -1);
  }
  return 1;
}

float _perform(int samplerate, dsp_module_parameters_t *parameters, int (*gate_check_func) (int, dsp_module_parameters_t*), int check_gate_on_sustain) {
  float out;
  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;

  double grow, a2, b1, y0, y1, y2;
  float level = envelope_gen->envelope->levels[envelope_gen->stage];
  switch (envelope_gen->shape) {
  case shape_Step:
    break;
  case shape_Hold:
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    break;
  case shape_Linear:
    grow = envelope_gen->grow;
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    level += grow;
    break;
  case shape_Exponential:
    grow = envelope_gen->grow;      
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    level *= grow;
    break;
  case shape_Sine:
    a2 = envelope_gen->a2;
    b1 = envelope_gen->b1;
    y2 = envelope_gen->y2;
    y1 = envelope_gen->y1;
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y0 = b1 * y1 - y2;
    level = a2 - y0;
    y2 = y1;
    y1 = y0;
    envelope_gen->y1 = y1;
    envelope_gen->y2 = y2;
    break;
  case shape_Welch:
    a2 = envelope_gen->a2;
    b1 = envelope_gen->b1;
    y2 = envelope_gen->y2;
    y1 = envelope_gen->y1;
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y0 = b1 * y1 - y2;
    level = a2 + y0;
    y2 = y1;
    y1 = y0;
    envelope_gen->y1 = y1;
    envelope_gen->y2 = y2;
    break;
  case shape_Curve:
    a2 = envelope_gen->a2;
    b1 = envelope_gen->b1;
    grow = envelope_gen->grow;
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    b1 *= grow;
    level = a2 - b1;
    envelope_gen->b1 = b1;
    break;
  case shape_Squared:
    grow = envelope_gen->grow;
    y1 = envelope_gen->y1;
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y1 += grow;
    level = y1 * y1;
    envelope_gen->y1 = y1;
    break;
  case shape_Cubed:
    grow = envelope_gen->grow;
    y1 = envelope_gen->y1;
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y1 += grow;
    y1 = modules_math_sc_max(y1, 0);
    level = y1 * y1 * y1;
    envelope_gen->y1 = y1;
    break;
  case shape_Sustain:
    if (check_gate_on_sustain) {
      if (gate_check_func(samplerate, parameters))
        out = level;
    } else
      out = level;
    break;
  }
  
  envelope_gen->level = level;
  
  return out;
}

float _next_k(int samplerate, dsp_module_parameters_t *parameters) {
  float out;

  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
  
  float gate = envelope_gen->gate;
  // Print("->EnvGen_next_k gate %g\n", gate);
  int counter = envelope_gen->counter;
  double level = envelope_gen->level;

  _check_gate(samplerate, parameters);
  envelope_gen->prev_gate = gate;

  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, release_node, loop_node,
  // [level, dur, shape, curve]

  if (counter <= 0) {
    int success = _next_segment(samplerate, parameters);
    if (!success)
      return envelope_gen->level;
  }

  out = _perform(samplerate, parameters, &_check_gate_passthru, 0);

  // Print("x %d %d %d %g\n", envelope_gen->stage, counter, envelope_gen->shape, *out);
  envelope_gen->level = level;
  envelope_gen->counter = counter - 1;

  return out;
}

float _next_aa(int samplerate, dsp_module_parameters_t *parameters) {
  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
  if (envelope_gen->counter <= 0) {
    int success = _next_segment(samplerate, parameters);
    if (!success)
      return envelope_gen->level;
  }
  return _perform(samplerate, parameters, &_check_gate_ar, 1);
}

extern
void math_modules_movement_envelope_segment_init(dsp_module_parameters_t *parameters) {
  
  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, release_node, loop_node,
  // [level, dur, shape, curve]

  env_gen_params_t *envelope_gen = (env_gen_params_t*)parameters->bytes_type;
  
  envelope_gen->end_level = \
    envelope_gen->level = \
    envelope_gen->init_level * \
    envelope_gen->level_scale + envelope_gen->level_bias;
  
  envelope_gen->counter = 0;
  envelope_gen->stage = ENVGEN_NOT_STARTED;
  envelope_gen->shape = shape_Hold;
  envelope_gen->prev_gate = 0.0f;
  envelope_gen->released = 0;
  
  const int initial_shape = (int)*envelope_gen->envelope->shape;
  if (initial_shape == shape_Hold)
    envelope_gen->level = envelope_gen->envelope->levels[0]; // we start at the end level;

  /* calculate first sample */
  _next_k(jackcli_samplerate, parameters);

} /* math_modules_movement_envelope_segment_init */

extern
void math_modules_movement_envelope_segment_edit(dsp_module_parameters_t *parameters,
                                                 float gate,
                                                 float level_scale,
                                                 float level_bias,
                                                 float time_scale,
                                                 float init_level,
                                                 int num_stages) {

} /* math_modules_movement_envelope_segment_edit */

extern
float math_modules_movement_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  return _next_aa(samplerate, parameters);
} /* math_modules_movement_envelope_segment */
