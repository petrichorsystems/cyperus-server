/* dsp_math.h
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


#ifndef DSP_MATH_H
#define DSP_MATH_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct dsp_module_parameters {
	int *int32_type;
	int **int32_arr_type;
	short *int16_type;
	float *float32_type;
	float **float32_arr_type;
	double *double_type;
	uint8_t *uint8_type;
	uint16_t *uint16_type;
	uint32_t *uint32_type;
	char **char_type;
	void *bytes_type;
} dsp_module_parameters_t;

#endif
