/* math_modules_movement_envelope_env.c
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


#include "math_modules_movement_envelope_env.h"

extern
void math_modules_movement_envelope_env_init(dsp_module_parameters_t *parameters,
                                              float attack_rate,
                                              float decay_rate,
                                              float release_rate,
                                              float sustain_level,
                                              float target_ratio_a,
                                              float target_ratio_dr,
                                              float mul,
                                              float add) {

} /* math_modules_movement_envelope_env_init */

extern
void math_modules_movement_envelope_env_edit(dsp_module_parameters_t *parameters,
                                              int gate,
                                              float attack_rate,
                                              float decay_rate,
                                              float release_rate,
                                              float sustain_level,
                                              float target_ratio_a,
                                              float target_ratio_dr,
                                              float mul,
                                              float add) {

}

extern
float math_modules_movement_envelope_env(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float out = 0.0f;

  
  return out;
}
