/* math_modules_envelope_follower.c
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

#include <math.h>

#include "../../../dsp_types.h"
#include "../../../dsp.h"

#include "params_modules_envelope_follower.h"

extern
void math_modules_envelope_follower(dsp_parameter *follower, int samplerate) {
  float *attack_ms = follower->parameters->float32_arr_type[PARAM_USER_ATTACK_MS];
  float *decay_ms = follower->parameters->float32_arr_type[PARAM_USER_DECAY_MS];
  float *scale = follower->parameters->float32_arr_type[PARAM_USER_SCALE];

  float last_sample = follower->parameters->float32_type[PARAM_INTERNAL_LAST_OUTPUT];
  
  float coeff_attack = 0.0f;
  float coeff_decay = 0.0f;
  float absin = 0.0f;
  
  for(int p=0; p<dsp_global_period; p++) {
    coeff_attack = exp(log(0.01f) / (attack_ms[p] * samplerate * 0.001f));
    coeff_decay = exp(log(0.01f) / (decay_ms[p] * samplerate * 0.001f));
  
    absin = fabs(follower->in[p]);
    if(absin > last_sample)
      follower->out[p] = coeff_attack * (last_sample - absin) + absin;
    else
      follower->out[p] = coeff_decay * (last_sample - absin) + absin;
    
    last_sample = follower->out[p];
    follower->out[p] = fabs(follower->out[p]) * scale[p];
  }  
  follower->parameters->float32_type[PARAM_INTERNAL_LAST_OUTPUT] = last_sample;
}
