
#ifndef MATH_MODULES_DSP_FILTER_BANDPASS_H
#define MATH_MODULES_DSP_FILTER_BANDPASS_H

#include <float.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "../../../jackcli.h"
#include "../../../dsp_math.h"
#include "../../../dsp_types.h"
#include "../../math_utils.h"

typedef union
{
    float f;
    unsigned int ui;
} t_bigorsmall32;

static inline int PD_BIGORSMALL(float f)  /* exponent outside (-64,64) */
{
    t_bigorsmall32 pun;
    pun.f = f;
    return((pun.ui & 0x20000000) == ((pun.ui >> 1) & 0x20000000));
}

extern float*
math_modules_filter_bandpass(dsp_parameter *filter, int samplerate);

extern float
math_modules_filter_bandpass_init(dsp_parameter *filter);

#endif
