/* math_modules_filter_bandpass.c
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

#include "params_modules_filter_bandpass.h"
#include "math_modules_filter_bandpass.h"

static float _bandpass_qcos(float f)
{
	if (f >= -(0.5f*3.14159f) && f <= 0.5f*3.14159f)
	{
		float g = f*f;
		return (((g*g*g * (-1.0f/720.0f) + g*g*(1.0f/24.0f)) - g*0.5f) + 1);
	}
	else return (0);
}

static void _bandpass_docoeff(dsp_parameter *filter, int samplerate, int idx)
{
	float *frequency = filter->parameters->float32_arr_type[PARAM_USER_FREQUENCY];
	float *q = filter->parameters->float32_arr_type[PARAM_USER_Q];

	float r, oneminusr, omega;

	if (frequency[idx] < 0.001f)
		frequency[idx] = 10.0f;
	if (q[idx] < 0)
		q[idx] = 0.0f;
  
	filter->parameters->float32_arr_type[PARAM_USER_FREQUENCY][idx] = frequency[idx];
	filter->parameters->float32_arr_type[PARAM_USER_Q][idx] = q[idx];
  
	omega = frequency[idx] * (2.0f * 3.14159f) / samplerate;
  
	if (q[idx] < 0.001f)
		oneminusr = 1.0f;
	else
		oneminusr = omega/q[idx];
  
	if (oneminusr > 1.0f)
		oneminusr = 1.0f;
  
	r = 1.0f - oneminusr;
  
	filter->parameters->float32_arr_type[PARAM_INTERNAL_COEF1][idx] = 2.0f * _bandpass_qcos(omega) * r; /* coef1 */
	filter->parameters->float32_arr_type[PARAM_INTERNAL_COEF2][idx] = - r * r; /* coef2 */
	filter->parameters->float32_arr_type[PARAM_INTERNAL_GAIN][idx] = 2 * oneminusr * (oneminusr + r * omega); /* gain */
}

extern void math_modules_filter_bandpass_init(dsp_parameter *filter)
{
	for(int p=0; p<dsp_global_period; p++)
		_bandpass_docoeff(filter, jackcli_samplerate, p);
}

extern void math_modules_filter_bandpass(dsp_parameter *filter, int samplerate)
{
	float *frequency, *q, *amount;
	float output, outsample;

	float *frequency_old, *q_old, *last, *prev, *coef1, *coef2, *gain;
  
	frequency = filter->parameters->float32_arr_type[PARAM_USER_FREQUENCY];
	q = filter->parameters->float32_arr_type[PARAM_USER_Q];
	amount = filter->parameters->float32_arr_type[PARAM_USER_AMOUNT];

	frequency_old = filter->parameters->float32_arr_type[PARAM_INTERNAL_FREQUENCY_OLD];
	q_old = filter->parameters->float32_arr_type[PARAM_INTERNAL_Q_OLD];

	last = filter->parameters->float32_arr_type[PARAM_INTERNAL_LAST];
	prev = filter->parameters->float32_arr_type[PARAM_INTERNAL_PREV];
	coef1 = filter->parameters->float32_arr_type[PARAM_INTERNAL_COEF1];
	coef2 = filter->parameters->float32_arr_type[PARAM_INTERNAL_COEF2];
	gain = filter->parameters->float32_arr_type[PARAM_INTERNAL_GAIN];  
  
	for (int p=0; p<dsp_global_period; p++) {	  
		if( (frequency[p] != frequency_old[p]) ||
		    (q[p] != q_old[p]) ) {
			_bandpass_docoeff(filter, samplerate, p);
			frequency_old[p] = frequency[p];
			q_old[p] = q[p];		  
		}

		output = filter->in[p] + coef1[p] * last[p] + coef2[p] * prev[p];
		outsample = gain[p] * output;
		
		prev[p] = last[p];
		last[p] = output;
		
		if (PD_BIGORSMALL(last[p]))
			last[p] = 0;
		if (PD_BIGORSMALL(prev[p]))
			prev[p] = 0;
  
		filter->out[p] = outsample * amount[p];
	};
}
