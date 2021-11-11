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

// called by nextSegment and check_gate:
// - counter: num samples to next segment
// - level: current envelope value
// - dur: if supplied and >= 0, stretch segment to last dur seconds (used in forced release)
int _init_segment(int samplerate, dsp_module_parameters_t *parameters, double dur) {
  // Print("stage %d\n", unit->m_stage);
  // Print("initSegment\n");
  // out = unit->m_level;
  int stage = parameters->int8_type[STATE_STAGE];
  float counter;
  double level  = parameters->float32_arr_type[PARAM_LEVELS][stage - 1];
  float shape;
  float a1, a2, b1, y1, y2, grow;
  float previous_end_level = parameters->double_type[STATE_END_LEVEL];
  if (parameters->int8_type[STATE_SHAPE] == shape_Hold)
    level = previous_end_level;
  double end_level = parameters->float32_arr_type[PARAM_LEVELS][stage] * parameters->float32_type[PARAM_LEVEL_SCALE] + parameters->float32_type[PARAM_LEVEL_BIAS]; // scale levels
  
  if (dur < 0) {
    dur = parameters->float32_arr_type[PARAM_TIMES][stage - 1] * parameters->float32_type[PARAM_TIME_SCALE];
  }

  shape = (int)parameters->float32_arr_type[PARAM_SHAPE][0];
    
  double curve = (double)parameters->float32_arr_type[PARAM_CURVE][0];
  parameters->double_type[STATE_END_LEVEL] = end_level;

  counter = (int)(dur * samplerate);
  counter = modules_math_sc_max(1, counter);

  /* printf("math_modules_movement_envelope_segment.c::_init_segment(), level: %f\n", level); */
  
  // Print("counter %d stageOffset %d   level %g   end_level %g   dur %g   shape %d   curve %g\n", counter,
  // stageOffset, level, end_level, dur, unit->m_shape, curve); Print("SAMPLERATE %g\n", SAMPLERATE);
  if (counter == 1)
    shape = 1; // shape_Linear
  // Print("new counter = %d  shape = %d\n", counter, unit->m_shape);
  switch ((int)shape) {
  case shape_Step: {
    level = end_level;
  } break;
  case shape_Hold: {
    level = previous_end_level;
  } break;
  case shape_Linear: {
    shape = (end_level - level) / counter;
    // Print("grow %g\n", unit->m_grow);
  } break;
  case shape_Exponential: {
    shape = pow(end_level / level, 1.0 / counter);
  } break;
  case shape_Sine: {
    double w = M_PI / counter;
    
    a2 = (end_level + level) * 0.5;
    b1 = 2.0 * cos(w);
    y1 = (end_level - level) * 0.5;
    y2 = y1 * sin(M_PI * 0.5 - w);
    level = a2 - y1;
  } break;
  case shape_Welch: {
    double w = (M_PI * 0.5) / counter;
    
    b1 = 2.0 * cos(w);

    if (end_level >= level) {
      a2 = level;
      y1 = 0.0;
      y2 = -sin(w) * (end_level - level);
    } else {
      a2 = end_level;
      y1 = level - end_level;
      y2 = cos(w) * (level - end_level);
    }
    level = a2 + y1;
  } break;
  case shape_Curve: {    
    if (fabs(curve) < 0.001) {
      shape = 1; // shape_Linear
      grow = (end_level - level) / counter;
    } else {
      double a1 = (end_level - level) / (1.0 - exp(curve));
      a2 = level + a1;
      b1 = a1;
      grow = exp(curve / counter);
    }  
    /* printf("math_modules_movement_envelope_segment.c::_init_segment(), parameters->float32_type[STATE_LEVEL]: %f, end_level: %f, level: %f, a2: %f, a1: %f, b1: %f, grow: %f\n", parameters->float32_type[STATE_LEVEL], end_level, level, a2, a1, b1, grow); */
  } break;
  case shape_Squared: {
    y1 = sqrt(level);
    y2 = sqrt(end_level);
    grow = (y2 - y1) / counter;
  } break;
  case shape_Cubed: {
    y1 = pow(level, 1.0 / 3.0); // 0.33333333);
    y2 = pow(end_level, 1.0 / 3.0);
    grow = (y2 - y1) / counter;
  } break;
  };

  parameters->double_type[STATE_A1] = a1;
  parameters->double_type[STATE_A2] = a2;
  parameters->double_type[STATE_B1] = b1;
  parameters->double_type[STATE_Y1] = y1;
  parameters->double_type[STATE_Y2] = y2;
  parameters->double_type[STATE_GROW] = grow;
  parameters->int8_type[STATE_COUNTER] = counter;  
  parameters->int8_type[STATE_SHAPE] = shape;
  parameters->float32_type[STATE_LEVEL] = level;

  return 1;
} /* _init_segment */

int _check_gate_passthru(int samplerate, dsp_module_parameters_t *parameters) {
    return 1;
} /* _check_gate_passthru */

int _check_gate(int samplerate, dsp_module_parameters_t *parameters) {
  
  if (parameters->float32_type[STATE_PREV_GATE] <= 0.f && parameters->float32_type[PARAM_GATE] > 0.f) {
    printf("math_modules_movement_envelope_segment.c::_check_gate(), first condition\n");
    parameters->int8_type[STATE_STAGE] = -1;
    parameters->int8_type[STATE_RELEASED] = 0;
    parameters->int8_type[STATE_DONE] = 0;
    parameters->int8_type[STATE_COUNTER] = 1 + parameters->int8_type[PARAM_OFFSET];
    return 0;
  } else if (parameters->float32_type[PARAM_GATE] <= -1.f && parameters->float32_type[STATE_PREV_GATE] > -1.f) {
    printf("math_modules_movement_envelope_segment.c::_check_gate(), second condition\n");
    // forced release: jump to last segment overriding its duration
    double dur = -parameters->float32_type[PARAM_GATE] - 1.f;
    parameters->int8_type[STATE_COUNTER] = (int)(dur * samplerate);
    parameters->int8_type[STATE_COUNTER] = modules_math_sc_max(1, parameters->int8_type[STATE_COUNTER]) + parameters->int8_type[PARAM_OFFSET];
    parameters->int8_type[STATE_STAGE] = 1; //parameters->int8_type[PARAM_NUM_STAGES] - 1;
    parameters->int8_type[STATE_RELEASED] = 1;

    printf("math_modules_movement_envelope_segment.c::_check_gate(), second condition, dur: %f\n", dur);
    
    _init_segment(samplerate, parameters, dur);
    return 0;
  } else if (parameters->float32_type[STATE_PREV_GATE] > 0.f && parameters->float32_type[PARAM_GATE] <= 0.f && parameters->int8_type[PARAM_RELEASE_NODE] >= 0 && !parameters->int8_type[STATE_RELEASED]) {
    printf("math_modules_movement_envelope_segment.c::_check_gate(), third condition\n");
    parameters->int8_type[STATE_COUNTER] = parameters->int8_type[PARAM_OFFSET];
    parameters->int8_type[STATE_STAGE] = parameters->int8_type[PARAM_RELEASE_NODE] - 1;
    parameters->int8_type[STATE_RELEASED] = 1;
    return 0;
  }
  return 1;
} /* _check_gate */

int _check_gate_ar(int samplerate, dsp_module_parameters_t *parameters) {
  const int result = _check_gate(samplerate, parameters);
  if (!result) {
    --parameters->float32_type[PARAM_GATE];
  }
  parameters->float32_type[STATE_PREV_GATE] = parameters->float32_type[PARAM_GATE];
  return result;
}

int _next_segment(int samplerate, dsp_module_parameters_t *parameters) {
  // Print("stage %d rel %d\n", parameters->bytes_type->stage, (int)ZIN0(kEnvGen_release_node));
  /* printf("math_modules_movement_envelope_segment.c::_next_segment()\n"); */
  int numstages = (int)parameters->int8_type[PARAM_NUM_STAGES];

  // Print("stage %d   numstages %d\n", envelope_gen->stage, numstages);
  if (parameters->int8_type[STATE_STAGE] + 1 >= numstages) { // num stages
    /* printf("entered done logic\n"); */    
    parameters->int8_type[STATE_COUNTER] = INT_MAX;
    parameters->int8_type[STATE_SHAPE] = 0;
    parameters->float32_type[STATE_LEVEL] = parameters->double_type[STATE_END_LEVEL];
    parameters->int8_type[STATE_DONE] = 1;
    
    /* done logic here */

  } else if (parameters->int8_type[STATE_STAGE] == ENVGEN_NOT_STARTED) {
    parameters->int8_type[STATE_COUNTER] = INT_MAX;
    return 1;
  } else if (parameters->int8_type[STATE_STAGE] + 1 == parameters->int8_type[PARAM_RELEASE_NODE] && !parameters->int8_type[STATE_RELEASED]) { // sustain stage
    int loop_node = (int)parameters->int8_type[PARAM_LOOP_NODE];
    if (loop_node >= 0 && loop_node < numstages) {
      parameters->int8_type[STATE_STAGE] = loop_node;
      return _init_segment(samplerate, parameters, -1);
    } else {
      parameters->int8_type[STATE_COUNTER] = INT_MAX;
      parameters->int8_type[STATE_SHAPE] = shape_Sustain;
      parameters->float32_type[STATE_LEVEL] = parameters->double_type[STATE_END_LEVEL];
    }
    // Print("sustain\n");
  } else {
    parameters->int8_type[STATE_STAGE]++;
    return _init_segment(samplerate, parameters, -1);
  }
  return 1;
} /* _next_segment */

float _perform(int samplerate, dsp_module_parameters_t *parameters, int (*gate_check_func) (int, dsp_module_parameters_t*), int check_gate_on_sustain) {
  float out;
  
  double grow, a2, b1, y0, y1, y2;
  float level = parameters->float32_type[STATE_LEVEL];

  /* printf("math_modules_movement_envelope_segment.c::_perform(), (int)parameters->float32_arr_type[PARAM_SHAPE][0]: %f, %d\n", parameters->float32_arr_type[PARAM_SHAPE][0], (int)parameters->float32_arr_type[PARAM_SHAPE][0]); */
  
  switch ((int)parameters->float32_arr_type[PARAM_SHAPE][0]) {
  case shape_Step:
    break;
  case shape_Hold:
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    break;
  case shape_Linear:
    grow = parameters->double_type[STATE_GROW];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    level += grow;
    break;
  case shape_Exponential:
    grow = parameters->double_type[STATE_GROW];      
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    level *= grow;
    break;
  case shape_Sine:
    a2 = parameters->double_type[STATE_A2];
    b1 = parameters->double_type[STATE_B1];
    y2 = parameters->double_type[STATE_Y2];
    y1 = parameters->double_type[STATE_Y1];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y0 = b1 * y1 - y2;
    level = a2 - y0;
    y2 = y1;
    y1 = y0;
    parameters->double_type[STATE_Y1] = y1;
    parameters->double_type[STATE_Y2] = y2;
    break;
  case shape_Welch:
    a2 = parameters->double_type[STATE_A2];
    b1 = parameters->double_type[STATE_B1];
    y2 = parameters->double_type[STATE_Y2];
    y1 = parameters->double_type[STATE_Y1];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y0 = b1 * y1 - y2;
    level = a2 + y0;
    y2 = y1;
    y1 = y0;
    parameters->double_type[STATE_Y1] = y1;
    parameters->double_type[STATE_Y2] = y2;
    break;
  case shape_Curve:
    a2 = parameters->double_type[STATE_A2];
    b1 = parameters->double_type[STATE_B1];
    grow = parameters->double_type[STATE_GROW];
    /* printf("math_modules_movement_envelope_segment.c::_perform(), parameters->float32_type[STATE_LEVEL]: %f, a2: %f, b1: %f, grow: %f\n", parameters->float32_type[STATE_LEVEL], a2, b1, grow); */
    if (!gate_check_func(samplerate, parameters)) {
      break;
    }
    
    out = level;
    b1 *= grow;
    level = a2 - b1;
    parameters->double_type[STATE_B1] = b1;
    /* printf("math_modules_movement_envelope_segment.c::_perform(), out: %f\n", out); */
    break;
  case shape_Squared:
    grow = parameters->double_type[STATE_GROW];
    y1 = parameters->double_type[STATE_Y1];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y1 += grow;
    level = y1 * y1;
    parameters->double_type[STATE_Y1] = y1;
    break;
  case shape_Cubed:
    grow = parameters->double_type[STATE_GROW];
    y1 = parameters->double_type[STATE_Y1];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y1 += grow;
    y1 = modules_math_sc_max(y1, 0);
    level = y1 * y1 * y1;
    parameters->double_type[STATE_Y1] = y1;
    break;
  case shape_Sustain:
    if (check_gate_on_sustain) {
      if (gate_check_func(samplerate, parameters))
        out = level;
    } else
      out = level;
    break;
  default:
    printf("_perform::WARNING: uknown shape!\n");
  }
  parameters->float32_type[STATE_LEVEL] = level;

  if(parameters->int8_type[STATE_DONE])
    return parameters->float32_arr_type[PARAM_LEVELS][parameters->int8_type[STATE_STAGE]] * parameters->float32_type[PARAM_LEVEL_SCALE] + parameters->float32_type[PARAM_LEVEL_BIAS];
  
  return out;
} /* _perform */

float _next_k(int samplerate, dsp_module_parameters_t *parameters) {
  float out;
  
  float gate = parameters->float32_type[PARAM_GATE];
  // Print("->EnvGen_next_k gate %g\n", gate);
  int counter = parameters->int8_type[STATE_COUNTER];
  double level = parameters->float32_type[STATE_LEVEL];

  _check_gate(samplerate, parameters);
  parameters->float32_type[STATE_PREV_GATE] = gate;

  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, release_node, loop_node,
  // [level, dur, shape, curve]

  if (counter <= 0) {
    int success = _next_segment(samplerate, parameters);
    if (!success)
      return parameters->float32_type[STATE_LEVEL];
  }

  out = _perform(samplerate, parameters, &_check_gate_passthru, 0);

  // Print("x %d %d %d %g\n", envelope_gen->stage, counter, envelope_gen->shape, *out);
  parameters->int8_type[STATE_COUNTER] = counter - 1;

  return out;
} /* _next_k */

float _next_aa(int samplerate, dsp_module_parameters_t *parameters) {
  float out;
  if (parameters->int8_type[STATE_COUNTER] <= 0) {
    
    int success = _next_segment(samplerate, parameters);
    if (!success) {
      /* printf("math_modules_movement_envelope_segment.c::_next_segment(), parameters->float32_type[STATE_LEVEL]: %f\n", parameters->float32_type[STATE_LEVEL]); */
      return parameters->float32_type[STATE_LEVEL];
    }
  }

  /* printf("math_modules_movement_envelope_segment.c::_next_aa(), about to run perform()\n"); */
  out = _perform(samplerate, parameters, &_check_gate_ar, 1);

  /* decrement counter */
  parameters->int8_type[STATE_COUNTER] = parameters->int8_type[STATE_COUNTER] - 1;

  /* printf("math_modules_movement_envelope_segment.c::_next_aa(), returning out: '%f'\n", out);; */
  return out;
} /* _next_aa */

extern
void math_modules_movement_envelope_segment_init(dsp_module_parameters_t *parameters) {  
  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, release_node, loop_node,
  // [level, dur, shape, curve]

  parameters->double_type[STATE_END_LEVEL] = parameters->float32_type[STATE_LEVEL] = \
    parameters->float32_type[PARAM_INIT_LEVEL]* parameters->float32_type[PARAM_LEVEL_SCALE] + \
    parameters->float32_type[PARAM_LEVEL_BIAS];

  parameters->int8_type[STATE_COUNTER] = 0;
  parameters->int8_type[STATE_STAGE] = ENVGEN_NOT_STARTED;
  parameters->int8_type[STATE_SHAPE] = shape_Hold;
  parameters->float32_type[STATE_PREV_GATE] = 0.0f;
  parameters->int8_type[STATE_RELEASED] = 0;
  
  const int initial_shape = (int)parameters->int8_type[STATE_SHAPE];
  
  if (initial_shape == shape_Hold)
    parameters->float32_type[STATE_LEVEL] = parameters->float32_arr_type[PARAM_LEVELS][1]; // we start at the end level;
 
  /* calculate first sample */
  _next_k(jackcli_samplerate, parameters);

} /* math_modules_movement_envelope_segment_init */

extern
void math_modules_movement_envelope_segment_edit(dsp_module_parameters_t *parameters,
                                                 int release_node,
                                                 int loop_node,
                                                 int offset,
                                                 float gate,
                                                 float level_scale,
                                                 float level_bias,
                                                 float time_scale) {
  parameters->int8_type[PARAM_RELEASE_NODE] = release_node;
  parameters->int8_type[PARAM_LOOP_NODE] = loop_node;
  parameters->int8_type[PARAM_OFFSET] = offset;
  parameters->float32_type[PARAM_GATE] = gate;
  parameters->float32_type[PARAM_LEVEL_SCALE] = level_scale;
  parameters->float32_type[PARAM_LEVEL_BIAS] = level_bias;
  parameters->float32_type[PARAM_TIME_SCALE] = time_scale;

} /* math_modules_movement_envelope_segment_edit */

extern
float math_modules_movement_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float out = _next_aa(samplerate, parameters);
  /* printf("out: %f\n", out); */
  /* printf("counter: %d\n", parameters->int8_type[STATE_COUNTER]); */
  /* printf("parameters->int8_type[STATE_DONE]: %d\n", parameters->int8_type[STATE_DONE]); */
  if(!parameters->int8_type[STATE_DONE])
    return out;
  else {
    return parameters->float32_arr_type[PARAM_LEVELS][parameters->int8_type[STATE_STAGE]];
  }
} /* math_modules_movement_envelope_segment */
