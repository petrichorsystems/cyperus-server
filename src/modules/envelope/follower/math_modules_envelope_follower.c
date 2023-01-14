/* math_modules_motion_envelope_follower.c
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

#include "math_modules_motion_envelope_follower.h"

extern
float math_modules_motion_envelope_follower(dsp_parameter *follower, int samplerate, int pos) {

  float attack_ms = follower->parameters->float32_type[0];
  float decay_ms = follower->parameters->float32_type[1];
  float coeff_attack = exp(log(0.01f) / (attack_ms * samplerate * 0.001f));
  float coeff_decay = exp(log(0.01f) / (decay_ms * samplerate * 0.001f));

  float scale = follower->parameters->float32_type[2];
  
  float insample = follower->in;
  float outsample = 0.0f;
  float absin = 0.0f;
  
  absin = fabs(insample);
  if(absin > follower->parameters->float32_type[3])
    outsample = coeff_attack * ( follower->parameters->float32_type[3] - absin) + absin;
  else
    outsample = coeff_decay * ( follower->parameters->float32_type[3] - absin) + absin;
  
  follower->parameters->float32_type[3] = outsample;

  return fabs(outsample) * scale;
}
