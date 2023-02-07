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

#include "math_modules_filter_bandpass.h"



static
float _bandpass_qcos(float f)
{
    if (f >= -(0.5f*3.14159f) && f <= 0.5f*3.14159f)
    {
        float g = f*f;
        return (((g*g*g * (-1.0f/720.0f) + g*g*(1.0f/24.0f)) - g*0.5f) + 1);
    }
    else return (0);
}

static
void _bandpass_docoeff(dsp_parameter *filter, int samplerate) {
  float frequency = filter->parameters->float32_type[0];
  float q = filter->parameters->float32_type[1];

  float r, oneminusr, omega;
  
  if (frequency < 0.001f)
    frequency = 10;
  if (q < 0)
    q = 0;
  
  filter->parameters->float32_type[0] = frequency;
  filter->parameters->float32_type[1] = q;
  
  omega = frequency * (2.0f * 3.14159f) / samplerate;

  if (q < 0.001f)
    oneminusr = 1.0f;
  else
    oneminusr = omega/q;

  if (oneminusr > 1.0f)
    oneminusr = 1.0f;

  r = 1.0f - oneminusr;
  
  filter->parameters->float32_type[7] = 2.0f * _bandpass_qcos(omega) * r; /* coef1 */
  filter->parameters->float32_type[8] = - r * r; /* coef2 */
  filter->parameters->float32_type[9] = 2 * oneminusr * (oneminusr + r * omega); /* gain */
}

extern
float math_modules_filter_bandpass_init(dsp_parameter *filter) {
  _bandpass_docoeff(filter, jackcli_samplerate);
  filter->parameters->float32_type[5] = 0.0f; /* last */
  filter->parameters->float32_type[6] = 0.0f; /* prev */
}

extern
float math_modules_filter_bandpass(dsp_parameter *filter, int samplerate, int pos) {
  float frequency, q, amount;
  float output, outsample;

  float frequency_old, q_old, last, prev, coef1, coef2, gain;
  
  frequency = filter->parameters->float32_type[0];
  q = filter->parameters->float32_type[1];
  amount = filter->parameters->float32_type[2];

  frequency_old = filter->parameters->float32_type[3];
  q_old = filter->parameters->float32_type[4];

  if( (frequency != frequency_old) ||
      (q != q_old) )
    _bandpass_docoeff(filter, samplerate);
  
  last = filter->parameters->float32_type[5];
  prev = filter->parameters->float32_type[6];
  coef1 = filter->parameters->float32_type[7];
  coef2 = filter->parameters->float32_type[8];
  gain = filter->parameters->float32_type[9];  

  output = filter->in + coef1 * last + coef2 * prev;
  outsample = gain * output;
  
  prev = last;
  last = output;

  if (PD_BIGORSMALL(last))
    last = 0;
  if (PD_BIGORSMALL(prev))
    prev = 0;
    

  filter->parameters->float32_type[5] = last;
  filter->parameters->float32_type[6] = prev;
  
  return outsample * amount;
}
