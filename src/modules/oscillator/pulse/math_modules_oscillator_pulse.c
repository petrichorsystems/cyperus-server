/*
 * math_modules_oscillator_pulse.c
 *
 * Code generation for model "cyperus_lowpass_module".
 *
 * Model version              : 1.2
 * Simulink Coder version : 9.1 (R2019a) 23-Nov-2018
 * C source code generated on : Mon Apr 13 12:16:16 2020
 *
 * Target selection: grt.tlc
 * Note: GRT includes extra infrastructure and instrumentation for prototyping
 * Embedded hardware selection: Intel->x86-64 (Windows64)
 * Code generation objective: Execution efficiency
 * Validation result: Not run
 */

#include <math.h>

#include "math_modules_oscillator_pulse.h"

#define twopi 6.28318530717952646f

#define M_SLOPE_FACTOR 2048

#define K_SINE_SIZE 8192

#define xlomask13 0x00007FFC
#define xlobits 14

const float kBadValue = 1e20f;

float PhaseFrac(uint32_t inPhase) {
    union {
        uint32_t itemp;
        float ftemp;
    } u;
    u.itemp = 0x3F800000 | (0x007FFF80 & ((inPhase) << 7));
    return u.ftemp - 1.f;
}

float lininterp(float x, float a, float b) { return a + x * (b - a); }

extern
void math_modules_oscillator_pulse_init(dsp_module_parameters_t *parameters) {
  double sineIndexToPhase = twopi / K_SINE_SIZE;
  double pmf = (1L << 29) / twopi;
  for (int i = 0; i <= K_SINE_SIZE; ++i) {
    double phase = i * sineIndexToPhase;
    float d = sin(phase);
    parameters->float32_arr_type[0][i] = d;
    parameters->float32_arr_type[1][i] = 1.0 / d;
  }  
  parameters->float32_arr_type[1][0] = parameters->float32_arr_type[1][K_SINE_SIZE / 2] = parameters->float32_arr_type[1][K_SINE_SIZE] = kBadValue;
  int sz = K_SINE_SIZE;
  int sz2 = sz >> 1;
  for (int i = 1; i <= 8; ++i) {
    parameters->float32_arr_type[1][i] = parameters->float32_arr_type[1][sz - i] = kBadValue;
    parameters->float32_arr_type[1][sz2 - i] = parameters->float32_arr_type[1][sz2 + i] = kBadValue;
  }
} /* math_modules_oscillator_pulse_init */

extern
float math_modules_oscillator_pulse(dsp_module_parameters_t *parameters, int samplerate, int pos) {
  float outsample = 0.0f;

  float freqin = parameters->float32_type[0];
  float duty = parameters->float32_type[1];

  float freqin_last = parameters->float32_type[6];
  int phase = parameters->float32_type[8];
  float y1 = parameters->float32_type[10];

    float* numtbl = parameters->float32_arr_type[0];
    float* dentbl = parameters->float32_arr_type[1];

    int freq, N, prevN;
    float scale, prevscale;
    int crossfade;

    if (freqin != freqin_last) {
        N = (int)((samplerate * 0.5) / freqin);
        if (N != parameters->float32_type[5]) {
            float maxfreqin;
            maxfreqin = modules_math_sc_max(parameters->float32_type[6], freqin);
            freq = (int)(parameters->float32_type[4] * maxfreqin);
            crossfade = 1;
        } else {
            freq = (int)(parameters->float32_type[4] * freqin);
            crossfade = 0;
        }
        prevN = parameters->float32_type[5];
        prevscale = parameters->float32_type[7];
        parameters->float32_type[5] = N;
        parameters->float32_type[7] = scale = 0.5 / N;
    } else {
        N = parameters->float32_type[5];
        freq = (int)(parameters->float32_type[4] * freqin);
        scale = parameters->float32_type[7];
        crossfade = 0;
    }
    int N2 = 2 * N + 1;

    int phaseoff = parameters->float32_type[9];
    int next_phaseoff = (int)(duty * (1L << 28));
    int phaseoff_slope = (int)((next_phaseoff - phaseoff) * M_SLOPE_FACTOR);
    parameters->float32_type[9] = next_phaseoff;
    float rscale = 1.f / scale + 1.f;
    float pul1, pul2;

    if (crossfade) {
        int prevN2 = 2 * prevN + 1;
        float xfade_slope = M_SLOPE_FACTOR;
        float xfade = 0.f;
            float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13)); float t0 = tbl[0];
            float t1 = tbl[1]; if (t0 == kBadValue || t1 == kBadValue) {
                tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
                t0 = tbl[0];
                t1 = tbl[1];
                float pfrac = PhaseFrac(phase);
                float denom = t0 + (t1 - t0) * pfrac;
                if (fabs(denom) < 0.0005f) {
                    pul1 = 1.f;
                } else {
                    int rphase = phase * prevN2;
                    pfrac = PhaseFrac(rphase);
                    tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                    float numer = lininterp(pfrac, tbl[0], tbl[1]);
                    float n1 = (numer / denom - 1.f) * prevscale;

                    rphase = phase * N2;
                    pfrac = PhaseFrac(rphase);
                    tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                    numer = lininterp(pfrac, tbl[0], tbl[1]);
                    float n2 = (numer / denom - 1.f) * scale;

                    pul1 = lininterp(xfade, n1, n2);
                }
            } else {
                float pfrac = PhaseFrac(phase);
                float denom = lininterp(pfrac, t0, t1);

                int rphase = phase * prevN2;
                pfrac = PhaseFrac(rphase);
                tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                float numer = lininterp(pfrac, tbl[0], tbl[1]);
                float n1 = (numer * denom - 1.f) * prevscale;

                rphase = phase * N2;
                pfrac = PhaseFrac(rphase);
                tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                numer = lininterp(pfrac, tbl[0], tbl[1]);
                float n2 = (numer * denom - 1.f) * scale;

                pul1 = lininterp(xfade, n1, n2);
            }

            int phase2 = phase + phaseoff;
            tbl = (float*)((char*)dentbl + ((phase2 >> xlobits) & xlomask13)); t0 = tbl[0]; t1 = tbl[1];
            if (t0 == kBadValue || t1 == kBadValue) {
                tbl = (float*)((char*)numtbl + ((phase2 >> xlobits) & xlomask13));
                t0 = tbl[0];
                t1 = tbl[1];
                float pfrac = PhaseFrac(phase2);
                float denom = t0 + (t1 - t0) * pfrac;
                if (fabs(denom) < 0.0005f) {
                    pul2 = 1.f;
                } else {
                    int rphase = phase2 * prevN2;
                    pfrac = PhaseFrac(rphase);
                    tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                    float numer = lininterp(pfrac, tbl[0], tbl[1]);
                    float n1 = (numer / denom - 1.f) * prevscale;

                    rphase = phase2 * N2;
                    pfrac = PhaseFrac(rphase);
                    tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                    numer = lininterp(pfrac, tbl[0], tbl[1]);
                    float n2 = (numer / denom - 1.f) * scale;

                    pul2 = lininterp(xfade, n1, n2);
                }
            } else {
                float pfrac = PhaseFrac(phase2);
                float denom = t0 + (t1 - t0) * pfrac;

                int rphase = phase2 * prevN2;
                pfrac = PhaseFrac(rphase);
                tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                float numer = lininterp(pfrac, tbl[0], tbl[1]);
                float n1 = (numer * denom - 1.f) * prevscale;

                rphase = phase2 * N2;
                pfrac = PhaseFrac(rphase);
                tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                numer = lininterp(pfrac, tbl[0], tbl[1]);
                float n2 = (numer * denom - 1.f) * scale;

                pul2 = lininterp(xfade, n1, n2);
            }

            outsample = y1 = pul1 - pul2 + 0.999f * y1;
            phase += freq;
            phaseoff += phaseoff_slope;
            xfade += xfade_slope;
    } else {
            float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13)); float t0 = tbl[0];
            float t1 = tbl[1]; if (t0 == kBadValue || t1 == kBadValue) {
                tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
                t0 = tbl[0];
                t1 = tbl[1];
                float pfrac = PhaseFrac(phase);
                float denom = t0 + (t1 - t0) * pfrac;
                if (fabs(denom) < 0.0005f) {
                    pul1 = rscale;
                } else {
                    int rphase = phase * N2;
                    pfrac = PhaseFrac(rphase);
                    tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                    float numer = lininterp(pfrac, tbl[0], tbl[1]);
                    pul1 = numer / denom;
                }
            } else {
                float pfrac = PhaseFrac(phase);
                float denom = t0 + (t1 - t0) * pfrac;
                int rphase = phase * N2;
                pfrac = PhaseFrac(rphase);
                float* tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                float numer = lininterp(pfrac, tbl[0], tbl[1]);
                pul1 = (numer * denom);
            }
            
            int phase2 = phase + phaseoff;
            tbl = (float*)((char*)dentbl + ((phase2 >> xlobits) & xlomask13)); t0 = tbl[0]; t1 = tbl[1];
            if (t0 == kBadValue || t1 == kBadValue) {
                tbl = (float*)((char*)numtbl + ((phase2 >> xlobits) & xlomask13));
                t0 = tbl[0];
                t1 = tbl[1];
                float pfrac = PhaseFrac(phase2);
                float denom = t0 + (t1 - t0) * pfrac;
                if (fabs(denom) < 0.0005f) {
                    pul2 = rscale;
                } else {
                    int rphase = phase2 * N2;
                    pfrac = PhaseFrac(rphase);
                    tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                    float numer = lininterp(pfrac, tbl[0], tbl[1]);
                    pul2 = numer / denom;
                }
            } else {
                float pfrac = PhaseFrac(phase2);
                float denom = t0 + (t1 - t0) * pfrac;
                int rphase = phase2 * N2;
                pfrac = PhaseFrac(rphase);
                float* tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
                float numer = lininterp(pfrac, tbl[0], tbl[1]);

                pul2 = (numer * denom);
            }
            
            outsample = y1 = (pul1 - pul2) * scale + 0.999f * y1;
            phase += freq;
            phaseoff += phaseoff_slope;
    }
    
    parameters->float32_type[10] = y1;
    parameters->float32_type[8] = phase;
    parameters->float32_type[6] = freqin;

    return outsample * parameters->float32_type[2] + parameters->float32_type[3];
}
