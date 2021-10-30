/* math_modules_audio_oscillator_pulse.c
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

#include "math_modules_audio_oscillator_pulse.h"

extern
float math_modules_audio_oscillator_pulse(dsp_module_parameters_t *parameters, int samplerate, int pos) {

  /* float frequency = parameters->float32_type[0]; */
  /* float pulse_width = parameters->float32_type[1]; */
  /* float mul = parameters->float32_type[2]; */
  /* float add = parameters->float32_type[3]; */
  
  return sin(parameters->float32_type[0] * (2 * M_PI) * pos / samplerate) >= (parameters->float32_type[1] - 0.5f) ? (parameters->float32_type[2] + parameters->float32_type[3]) : (-1 * parameters->float32_type[2] + parameters->float32_type[3]);
  
} /* math_modules_audio_oscillator_pulse */
