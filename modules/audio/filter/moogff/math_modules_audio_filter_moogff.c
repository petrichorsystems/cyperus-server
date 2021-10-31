/* math_modules_audio_filter_moogff.c
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

#include "math_modules_audio_filter_moogff.h"

#define twopi 6.28318530717952646f
#define M_SLOPE_FACTOR 2048

#define CALCSLOPE(next, prev) ((next - prev) * (float)M_SLOPE_FACTOR)

float sc_clip(float x, float lo, float hi) {
  return fmaxf(fminf(x, hi), lo);
}
  
float sc_reciprocal(float x) {
#ifdef __SSE__
    // adapted from AP-803 Newton-Raphson Method with Streaming SIMD Extensions                                    
    // 23 bit accuracy (out of 24bit)                 
    const __m128 arg = _mm_set_ss(x);
    const __m128 approx = _mm_rcp_ss(arg);
    const __m128 muls = _mm_mul_ss(_mm_mul_ss(arg, approx), approx);
    const __m128 doubleApprox = _mm_add_ss(approx, approx);
    const __m128 result = _mm_sub_ss(doubleApprox, muls);
    return _mm_cvtss_f32(result);
#else
    return 1.f / x;
#endif
}

/*
 * Zap dangerous values (subnormals, infinities, nans) in feedback loops to zero.
 * Prevents pathological math operations in ugens and can be used at the end of a
 * block to fix any recirculating filter values.
 */
float zapgremlins(float x) {
  float absx = fabs(x);
  // very small numbers fail the first test, eliminating denormalized numbers
  //    (zero also fails the first test, but that is OK since it returns zero.)
  // very large numbers fail the second test, eliminating infinities
  // Not-a-Numbers fail both tests and are eliminated.
  return (absx > (float)1e-15 && absx < (float)1e15) ? x : (float)0.;
}

/**
"MoogFF" - Moog VCF digital implementation.
As described in the paper entitled
"Preserving the Digital Structure of the Moog VCF"
by Federico Fontana
appeared in the Proc. ICMC07, Copenhagen, 25-31 August 2007

Original Java code Copyright F. Fontana - August 2007
federico.fontana@univr.it

Ported to C++ for SuperCollider by Dan Stowell - August 2007
http://www.mcld.co.uk/

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/
extern
float math_modules_audio_filter_moogff(dsp_parameter *filter, int samplerate, int pos) {  
  float k = filter->parameters->float32_type[1];
  k = sc_clip(k, 0.f, 4.f);

  // Load state from the struct
  double s1 = filter->parameters->double_type[2]; /* unit->m_s1; */
  double s2 = filter->parameters->double_type[3]; /* unit->m_s2; */
  double s3 = filter->parameters->double_type[4]; /* unit->m_s3; */
  double s4 = filter->parameters->double_type[5]; /* unit->m_s4; */

  // Reset filter state if requested
  if (filter->parameters->float32_type[2] > 0.0f)
      s1 = s2 = s3 = s4 = 0.f;

  double a1 = filter->parameters->double_type[1], b0 = filter->parameters->double_type[0]; // Filter coefficient parameters

  // Update filter coefficients, but only if freq changes since it involves some expensive operations

  float freqIn = filter->parameters->float32_type[0];
  if (filter->parameters->float32_type[4] != freqIn) {
      // Print("Updated freq to %g\n", freq);
      double T = 1.0f / samplerate;
      double wcD = 2.0 * tan(T * M_PI * freqIn) * samplerate;
      if (wcD < 0)
          wcD = 0; // Protect against negative cutoff freq
      double TwcD = T * wcD;
      b0 = TwcD / (TwcD + 2.);
      a1 = (TwcD - 2.) / (TwcD + 2.);
      filter->parameters->float32_type[4] = freqIn;
      filter->parameters->double_type[0] = b0;
      filter->parameters->double_type[1] = a1;
  }

  float out = 0.0f;
  double in = filter->in;

  if(filter->parameters->float32_type[5] == k) {
    // compute loop values
    double o = s4 + b0 * (s3 + b0 * (s2 + b0 * s1));
    double outs = (b0 * b0 * b0 * b0 * in + o) * sc_reciprocal(1.0 + b0 * b0 * b0 * b0 * k);

    out = outs;

    double u = in - k * outs;

    // update 1st order filter states
    double past = u;
    double future = b0 * past + s1; s1 = b0 * past - a1 * future;

    past = future; future = b0 * past + s2; s2 = b0 * past - a1 * future;
    
    past = future; future = b0 * past + s3; s3 = b0 * past - a1 * future;

    s4 = b0 * future - a1 * outs;
  } else {
      float new_k = k;
      float old_k = filter->parameters->float32_type[5];
      float slope_k = CALCSLOPE(new_k, old_k);
      k = old_k;

      // compute loop values
      double o = s4 + b0 * (s3 + b0 * (s2 + b0 * s1));
      double outs = (b0 * b0 * b0 * b0 * in + o) * sc_reciprocal(1.0 + b0 * b0 * b0 * b0 * k);

      out = outs;

      double u = in - k * outs;

      // update 1st order filter states
      double past = u;
      double future = b0 * past + s1; s1 = b0 * past - a1 * future;

      past = future; future = b0 * past + s2; s2 = b0 * past - a1 * future;
      
      past = future; future = b0 * past + s3; s3 = b0 * past - a1 * future;
      
      s4 = b0 * future - a1 * outs; k += slope_k;
      
      filter->parameters->float32_type[5] = new_k;
  }

  // Store state
  filter->parameters->double_type[2] = zapgremlins(s1);
  filter->parameters->double_type[3] = zapgremlins(s2);
  filter->parameters->double_type[4] = zapgremlins(s3);
  filter->parameters->double_type[5] = zapgremlins(s4);

  return out;
}
