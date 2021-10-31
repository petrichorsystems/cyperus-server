/* math_modules_movement_envelope_adsr.c
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

/* based off of Francesco D'Este's c-port of Nigel Redmon's c++ adsr envelope.

https://github.com/fdeste/ADSR
Originally Created by Nigel Redmon on 12/18/12.
EarLevel Engineering: earlevel.com
Copyright 2012 Nigel Redmon
C Port 2013 Francesco D'Este

For a complete explanation of the ADSR envelope generator and code,
read the series of articles by the author, starting here:
http://www.earlevel.com/main/2013/06/01/envelope-generators/

License:

This source code is provided as is, without warranty.
You may copy and distribute verbatim copies of this document.
You may modify and use this source code to create binary code for your own purposes, free or commercial.
*/

#include "math_modules_movement_envelope_adsr.h"

float _calc_coeff(float rate, float target_ratio) {
    return exp(-log((1.0 + target_ratio) / target_ratio) / rate);
}

void _set_attack_rate(dsp_module_parameters_t *parameters, float rate) {
  if(rate == parameters->float32_type[0])
    return;
  
  float target_ratio_a = parameters->float32_type[4];
  float attack_coeff, attack_base;

  attack_coeff = _calc_coeff(rate, parameters->float32_type[4]);
  attack_base = (1.0f + target_ratio_a) * (1.0 - attack_coeff);

  parameters->float32_type[0] = rate;
  parameters->float32_type[8] = attack_coeff;
  parameters->float32_type[11] = attack_base;

  printf("math_modules_movement_envelope_adsr.c::_set_attack_rate - assigned attack rate %f\n", rate);
} /* _set_attack_rate */

void _set_decay_rate(dsp_module_parameters_t *parameters, float rate) {
  if(rate == parameters->float32_type[1])
    return;
  
  float target_ratio_dr = parameters->float32_type[5];
  float sustain_level   = parameters->float32_type[3];
  float decay_coeff, decay_base;

  decay_coeff = _calc_coeff(rate, target_ratio_dr);
  decay_base = (sustain_level - target_ratio_dr) * (1.0 - decay_coeff);

  parameters->float32_type[1] = rate;
  parameters->float32_type[9] = decay_coeff;
  parameters->float32_type[12] = decay_base;

  printf("math_modules_movement_envelope_adsr.c::_set_decay_rate - assigned decay rate %f\n", rate);
} /* _set_decay_rate */

void _set_release_rate(dsp_module_parameters_t *parameters, float rate) {
  if(rate == parameters->float32_type[2])
    return;
  
  float target_ratio_dr = parameters->float32_type[5];
  float release_coeff, release_base;

  release_coeff = _calc_coeff(rate, target_ratio_dr);
  release_base = -target_ratio_dr * (1.0 - release_coeff);

  parameters->float32_type[2] = rate;
  parameters->float32_type[10] = release_coeff;
  parameters->float32_type[13] = release_base;

  printf("math_modules_movement_envelope_adsr.c::_set_release_rate - assigned release rate %f\n", rate);
} /* _set_release_rate */

void _set_sustain_level(dsp_module_parameters_t *parameters, float level) {
  if(level == parameters->float32_type[3])
    return;

  float target_ratio_dr = parameters->float32_type[5];
  float decay_coeff = parameters->float32_type[9];
  float decay_base;

  decay_base = (level - target_ratio_dr) * (1.0f - decay_coeff);

  parameters->float32_type[3] = level;
  parameters->float32_type[12] = decay_base;

  printf("math_modules_movement_envelope_adsr.c::_set_sustain_level - assigned sustain level %f\n", level);
} /* _set_sustain_level */

void _set_target_ratio_a(dsp_module_parameters_t *parameters, float target_ratio) {
  if(target_ratio == parameters->float32_type[4])
    return;

  float target_ratio_a = parameters->float32_type[4];
  float attack_coeff = parameters->float32_type[8];  
  float attack_base;
  
  if(target_ratio < 0.000000001f)
    target_ratio = 0.000000001f; // -180dB
  
  attack_base = (1.0f + target_ratio) * (1.0f - attack_coeff);
  
  parameters->float32_type[4] = target_ratio;
  parameters->float32_type[11] = attack_base;

  printf("math_modules_movement_envelope_adsr.c::_set_target_ratio_a - assigned sustain target ratio %f\n", target_ratio);
} /* _set_target_ratio_a */

void _set_target_ratio_dr(dsp_module_parameters_t *parameters, float target_ratio) {
  if(target_ratio == parameters->float32_type[4])
    return;

  float sustain_level = parameters->float32_type[3];
  float decay_coeff = parameters->float32_type[9];
  float release_coeff = parameters->float32_type[10];
  float decay_base, release_base;
  
  if(target_ratio < 0.000000001f)
    target_ratio = 0.000000001f; // -180dB
  
  decay_base = (sustain_level - target_ratio) * (1.0f - decay_coeff);
  release_base = -target_ratio * (1.0f - release_coeff);
  
  parameters->float32_type[5] = target_ratio;
  parameters->float32_type[12] = decay_base;
  parameters->float32_type[13] = release_base;
  
  printf("math_modules_movement_envelope_adsr.c::_set_target_ratio_dr - assigned sustain target ratio %f\n", target_ratio);
} /* _set_target_ratio_dr */

extern
void math_modules_movement_envelope_adsr_init(dsp_module_parameters_t *parameters, int samplerate) {
  _set_attack_rate(parameters, 0.0f);
  _set_decay_rate(parameters, 0.0f);
  _set_release_rate(parameters, 0.0f);
  _set_sustain_level(parameters, 1.0f);
  _set_target_ratio_a(parameters, 0.3f);
  _set_target_ratio_dr(parameters, 0.0001f);
  parameters->float32_type[6] = 1.0f;
  parameters->float32_type[7] = 0.0f;;
} /* math_modules_movement_envelope_adsr_init */

extern
void math_modules_movement_envelope_adsr_edit(dsp_module_parameters_t *parameters,
                                              int gate,
                                              float attack_rate,
                                              float decay_rate,
                                              float release_rate,
                                              float sustain_level,
                                              float target_ratio_a,
                                              float target_ratio_dr,
                                              float mul,
                                              float add) {
  _set_attack_rate(parameters, attack_rate);
  _set_decay_rate(parameters, decay_rate);
  _set_release_rate(parameters, release_rate);
  _set_sustain_level(parameters, sustain_level);
  _set_target_ratio_a(parameters, target_ratio_a);
  _set_target_ratio_dr(parameters, target_ratio_dr);  
  parameters->float32_type[6] = mul;
  parameters->float32_type[7] = add;
}

extern
float math_modules_movement_envelope_adsr(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float outsample = 0.0f;

  
  
  return outsample;
}