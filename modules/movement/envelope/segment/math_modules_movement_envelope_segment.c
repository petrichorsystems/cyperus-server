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

  printf("math_modules_movement_envelope_segment.c::_init_segment()\n");
  printf("math_modules_movement_envelope_segment.c::_init_segment(), stage: %d\n", parameters->int8_type[5]);  
  // Print("stage %d\n", unit->m_stage);
  // Print("initSegment\n");
  // out = unit->m_level;
  int stage = parameters->int8_type[5];
  float counter;
  float level = parameters->float32_type[6];
  float shape;
  float a1, a2, b1, y1, y2, grow;
  float previous_end_level = parameters->double_type[6];
  if (parameters->int8_type[6] == shape_Hold)
    level = previous_end_level;
  double end_level = parameters->float32_arr_type[0][stage] * parameters->float32_type[1] + parameters->float32_type[2]; // scale levels
  
  if (dur < 0) {
    printf("about to assign duration\n");
    dur = parameters->float32_arr_type[1][stage - 1] * parameters->float32_type[3];
    printf("assigned dur: %f\n", dur);
  }

  shape = (int)parameters->float32_arr_type[2][0];
    
  double curve = (double)parameters->float32_arr_type[3][0];
  parameters->double_type[6] = end_level;

  counter = (int)(dur * samplerate);
  counter = modules_math_sc_max(1, counter);

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
    printf(" !!!!!! in shape_Curve\n");
    printf(" |||||| curve: %f\n", curve);
    printf(" |||||| level: %f\n", level);
    printf(" |||||| end_level: %f\n", end_level);
    printf(" |||||| counter: %f\n", counter);

    printf("before!\n");
      printf(" !!!!!! a1: %f\n", a1);
      printf(" !!!!!! a2: %f\n", a2);
      printf(" !!!!!! b1: %f\n", b1);
      printf(" !!!!! grow: %f\n", grow);
    
    if (fabs(curve) < 0.001) {
      shape = 1; // shape_Linear
      grow = (end_level - level) / counter;
    } else {
      double a1 = (end_level - level) / (1.0 - exp(curve));
      a2 = level + a1;
      b1 = a1;
      grow = exp(curve / counter);

      printf("after!\n");
      printf(" !!!!!! a1: %f\n", a1);
      printf(" !!!!!! a2: %f\n", a2);
      printf(" !!!!!! b1: %f\n", b1);
      printf(" !!!!! grow: %f\n", grow);
      
    }
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

  parameters->double_type[0] = a1;
  parameters->double_type[1] = a2;
  parameters->double_type[2] = b1;
  parameters->double_type[3] = y1;
  parameters->double_type[4] = y2;
  parameters->double_type[5] = grow;
  parameters->int8_type[4] = counter;  
  parameters->int8_type[6] = shape;
  parameters->float32_type[6] = level;
  
  return 1;
}

int _check_gate_passthru(int samplerate, dsp_module_parameters_t *parameters) {
    return 1;
}

int _check_gate(int samplerate, dsp_module_parameters_t *parameters) {
  printf("_check_gate::after variable instantiation\n");
  if (parameters->float32_type[7] <= 0.f && parameters->float32_type[0] > 0.f) {
    parameters->int8_type[5] = -1;
    parameters->int8_type[8] = 0;
    parameters->int8_type[9] = 0;
    parameters->int8_type[4] = parameters->int8_type[2];
    return 0;
  } else if (parameters->float32_type[0] <= -1.f && parameters->float32_type[7] > -1.f) {
    printf("forced release\n");
    // forced release: jump to last segment overriding its duration
    double dur = -parameters->float32_type[0] - 1.f;
    parameters->int8_type[4] = (int)(dur * samplerate);
    parameters->int8_type[4] = modules_math_sc_max(1, parameters->int8_type[4]) + parameters->int8_type[2];
    parameters->int8_type[5] = parameters->int8_type[3] - 1;
    parameters->int8_type[8] = 1;
    _init_segment(samplerate, parameters, dur);
    return 0;
  } else if (parameters->float32_type[7] > 0.f && parameters->float32_type[0] <= 0.f && parameters->int8_type[7] >= 0 && !parameters->int8_type[8]) {
    parameters->int8_type[4] = parameters->int8_type[2];
    parameters->int8_type[5] = parameters->int8_type[7] - 1;
    parameters->int8_type[8] = 1;
    return 0;
  }

  printf("math_modules_movement_enveleope_segment.c::_check_gate(), parameters->int8_type[5]: %d, parameters->int8_type[3] - 1: %d\n", parameters->int8_type[5], parameters->int8_type[3] - 1);
  
  return 1;
}

int _check_gate_ar(int samplerate, dsp_module_parameters_t *parameters) {
  const int result = _check_gate(samplerate, parameters);
  if (!result) {
    --parameters->float32_type[0];
  }
  return result;
}

int _next_segment(int samplerate, dsp_module_parameters_t *parameters) {
  printf("math_modules_movement_envelope_segment.c::_next_segment()\n");
  printf("math_modules_movement_envelope_segment.c::next_segment(), parameters->int8_type[5]: %d\n", parameters->int8_type[5]);
  printf("math_modules_movement_envelope_segment.c::next_segment(), parameters->int8_type[3]: %d\n", parameters->int8_type[3]);
  
  // Print("stage %d rel %d\n", parameters->bytes_type->stage, (int)ZIN0(kEnvGen_release_node));
  
  int numstages = (int)parameters->int8_type[3];

  // Print("stage %d   numstages %d\n", envelope_gen->stage, numstages);
  if (parameters->int8_type[5] + 1 >= numstages) { // num stages
    printf("entered done logic\n");
    
    // Print("stage+1 > num stages\n");
    
    parameters->int8_type[4] = INT_MAX;
    parameters->int8_type[6] = 0;
    parameters->float32_type[6] = parameters->double_type[6];
    parameters->int8_type[9] = 1;
    
    /* done logic here */
    
  } else if (parameters->int8_type[5] == ENVGEN_NOT_STARTED) {
    printf("entered not started\n");
    parameters->int8_type[4] = INT_MAX;
    return 1;
  } else if (parameters->int8_type[5] + 1 == parameters->int8_type[7] && !parameters->int8_type[8]) { // sustain stage
    int loop_node = (int)parameters->int8_type[1];
    if (loop_node >= 0 && loop_node < numstages) {
      parameters->int8_type[5] = loop_node;
      return _init_segment(samplerate, parameters, -1);
    } else {
      parameters->int8_type[4] = INT_MAX;
      parameters->int8_type[6] = shape_Sustain;
      parameters->float32_type[6] = parameters->double_type[6];
    }
    // Print("sustain\n");
  } else {
    printf("hello?\n");
    parameters->int8_type[5]++;
    printf("new stage: %d\n", parameters->int8_type[5]);
    return _init_segment(samplerate, parameters, -1);
  }
  return 1;
}

float _perform(int samplerate, dsp_module_parameters_t *parameters, int (*gate_check_func) (int, dsp_module_parameters_t*), int check_gate_on_sustain) {
  float out;
  
  double grow, a2, b1, y0, y1, y2;

  printf("shape_Curve: %d\n", (int)shape_Curve);
  printf("parameters->float32_arr_type[2][0]: %f\n", parameters->float32_arr_type[2][0]);
  float level = parameters->float32_type[6];
  
  printf("_perform::about to run the switch\n");
  
  switch ((int)parameters->float32_arr_type[2][0]) {
  case shape_Step:
    break;
  case shape_Hold:
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    break;
  case shape_Linear:
    grow = parameters->double_type[5];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    level += grow;
    break;
  case shape_Exponential:
    grow = parameters->double_type[5];      
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    level *= grow;
    break;
  case shape_Sine:
    a2 = parameters->double_type[1];
    b1 = parameters->double_type[2];
    y2 = parameters->double_type[4];
    y1 = parameters->double_type[3];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y0 = b1 * y1 - y2;
    level = a2 - y0;
    y2 = y1;
    y1 = y0;
    parameters->double_type[3] = y1;
    parameters->double_type[4] = y2;
    break;
  case shape_Welch:
    a2 = parameters->double_type[1];
    b1 = parameters->double_type[2];
    y2 = parameters->double_type[4];
    y1 = parameters->double_type[3];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y0 = b1 * y1 - y2;
    level = a2 + y0;
    y2 = y1;
    y1 = y0;
    parameters->double_type[3] = y1;
    parameters->double_type[4] = y2;
    break;
  case shape_Curve:
    a2 = parameters->double_type[1];
    b1 = parameters->double_type[2];
    grow = parameters->double_type[5];
    
    if (!gate_check_func(samplerate, parameters)) {
      break;
    }
    printf(" ?????? _perform::level: %f\n", level);
    printf(" ?????? _perform::grow: %f\n", grow);
    printf(" ?????? _perform:: b1: %f\n", b1);
    printf(" ?????? _perform:: a2: %f\n", a2);
    
    out = level;
    b1 *= grow;
    level = a2 - b1;
    parameters->double_type[2] = b1;
    
    break;
  case shape_Squared:
    grow = parameters->double_type[5];
    y1 = parameters->double_type[3];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y1 += grow;
    level = y1 * y1;
    parameters->double_type[3] = y1;
    break;
  case shape_Cubed:
    grow = parameters->double_type[5];
    y1 = parameters->double_type[3];
    if (!gate_check_func(samplerate, parameters))
      break;
    out = level;
    y1 += grow;
    y1 = modules_math_sc_max(y1, 0);
    level = y1 * y1 * y1;
    parameters->double_type[3] = y1;
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
  
  parameters->float32_type[6] = level;
  printf("_perform::returning\n");
  
  return out;
}

float _next_k(int samplerate, dsp_module_parameters_t *parameters) {
  float out;
  
  float gate = parameters->float32_type[0];
  // Print("->EnvGen_next_k gate %g\n", gate);
  int counter = parameters->int8_type[4];
  double level = parameters->float32_type[6];

  printf("about to run _check_gate()\n");
  _check_gate(samplerate, parameters);
  parameters->float32_type[7] = gate;

  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, release_node, loop_node,
  // [level, dur, shape, curve]

  printf("about to run _next_segment()\n");
  if (counter <= 0) {
    int success = _next_segment(samplerate, parameters);
    if (!success)
      return parameters->float32_type[6];
  }

  printf("about to run _perform\n");
  out = _perform(samplerate, parameters, &_check_gate_passthru, 0);

  // Print("x %d %d %d %g\n", envelope_gen->stage, counter, envelope_gen->shape, *out);
  parameters->int8_type[4] = counter - 1;

  return out;
}

float _next_aa(int samplerate, dsp_module_parameters_t *parameters) {
  float out;
  if (parameters->int8_type[4] <= 0) {
    int success = _next_segment(samplerate, parameters);
    if (!success)
      return parameters->float32_type[6];
  }

  out = _perform(samplerate, parameters, &_check_gate_ar, 1);

  /* decrement counter */
  parameters->int8_type[4] = parameters->int8_type[4] - 1;

  printf(" ^^^^^^ counter/parameters->int8_type[4]: %d\n", parameters->int8_type[4]);

  return out;
}

extern
void math_modules_movement_envelope_segment_init(dsp_module_parameters_t *parameters) {
  printf("math_modules_movement_envelope_segment::math_modules_movement_envelope_segment_init()\n");

  
  // gate = 1.0, levelScale = 1.0, levelBias = 0.0, timeScale
  // level0, numstages, release_node, loop_node,
  // [level, dur, shape, curve]

  parameters->double_type[6] = parameters->float32_type[6] = \
    parameters->float32_type[5] * parameters->float32_type[1] + \
    parameters->float32_type[2];

  parameters->int8_type[4] = 0;
  parameters->int8_type[5] = ENVGEN_NOT_STARTED;
  parameters->int8_type[6] = shape_Hold;
  parameters->float32_type[7] = 0.0f;
  parameters->int8_type[8] = 0;

  printf("math_modules_movement_envelope_segment_init::envelope_gen->shape: %d, shape_Hold: %d\n",
         parameters->int8_type[6],
         shape_Hold);
  
  const int initial_shape = (int)parameters->int8_type[6];
  
  if (initial_shape == shape_Hold)
    parameters->float32_type[6] = parameters->float32_arr_type[0][1]; // we start at the end level;

  printf("about to run _next_k()\n");
  
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
  parameters->int8_type[7] = release_node;
  parameters->int8_type[1] = loop_node;
  parameters->int8_type[2] = offset;
  parameters->float32_type[0] = gate;
  parameters->float32_type[1] = level_scale;
  parameters->float32_type[2] = level_bias;
  parameters->float32_type[3] = time_scale;
  

} /* math_modules_movement_envelope_segment_edit */

extern
float math_modules_movement_envelope_segment(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float out = _next_aa(samplerate, parameters);
  if( !parameters->int8_type[9] && !parameters->int8_type[1])
    return out;
  else
    return parameters->float32_arr_type[0][parameters->int8_type[5]];
} /* math_modules_movement_envelope_segment */
