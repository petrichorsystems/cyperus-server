/* math_modules_audio_delay_simple.c
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

#include "math_modules_audio_delay_simple.h"

extern
float math_modules_audio_delay_simple(dsp_parameter *delay, int samplerate, int pos) {
  /* printf("math_modules_audio_delay_simple.c::math_modules_audio_delay_simple()\n"); */
  
  float out = 0.0f;

  /* user-facing parameters */
  float amount = delay->parameters->float32_type[0];
  float time = delay->parameters->float32_type[1];
  float feedback = delay->parameters->float32_type[2];

  /* internal parameters */
  int delay_pos = delay->parameters->int32_type[0];
  int delay_time_pos = delay->parameters->int32_type[1];

  if( delay_pos >= time )
    delay_pos = 0;

  delay_time_pos = delay_pos - time;

  if( delay_time_pos < 0 )
    delay_time_pos = delay_time_pos + (unsigned int)time;
  
  out = delay->parameters->float32_arr_type[0][delay_pos] = delay->in + (delay->parameters->float32_arr_type[0][delay_time_pos] * feedback);
  delay_pos += 1;
  
  delay->parameters->int32_type[0] = delay_pos;
  delay->parameters->int32_type[1] = delay_time_pos;
  
  return out * amount;
}
