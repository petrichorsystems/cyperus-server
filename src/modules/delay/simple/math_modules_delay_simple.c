/* math_modules_delay_simple.c
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

#include "math_modules_delay_simple.h"

extern
float *math_modules_delay_simple(dsp_parameter *delay, int samplerate) {
  /* printf("math_modules_delay_simple.c::math_modules_delay_simple()\n"); */
  
  float *out = malloc(sizeof(float) * dsp_global_period);

  float *amount = delay->parameters->float32_arr_type[1];
  float *feedback = delay->parameters->float32_arr_type[3];
  
  int *time_samples = delay->parameters->int32_arr_type[0];
  
  int delay_pos = delay->parameters->int32_type[0];
  int delay_time_pos = delay->parameters->int32_type[1];
  
  for(int p=0; p<dsp_global_period; p++) {
    if( delay_pos >= time_samples[p] )
      delay_pos = 0;

    delay_time_pos = delay_pos - time_samples[p];
    
    if( delay_time_pos < 0 ) {
      delay_time_pos = delay_time_pos + time_samples[p];
    }    
    out[p] = delay->parameters->float32_arr_type[0][delay_pos] = delay->in[p] + (delay->parameters->float32_arr_type[0][delay_time_pos] * feedback[p]);
  
    delay_pos += 1;
    out[p] *= amount[p];    
  }
  delay->parameters->int32_type[0] = delay_pos;
  delay->parameters->int32_type[1] = delay_time_pos;
  return out;
}
