/* libsilica_dsp_types.h
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

Copyright 2019 murray foster */

#ifndef LIBSILICA_DSP_TYPES_H
#define LIBSILICA_DSP_TYPES_H

typedef enum{
             NONE = 0,
             ADD_FLOAT2 = 1,
	     ADD_FLOAT3 = 2
} dsp_opcode;

typedef struct {
  dsp_opcode opcode;
  float *x;
  float schedule_flag;
  float num_x;
} libsilica_dsp_add_float2_instruction_t;

typedef struct {
  float *y;
  short unsigned int num_y;
} libsilica_dsp_add_float2_result_t;


#endif
