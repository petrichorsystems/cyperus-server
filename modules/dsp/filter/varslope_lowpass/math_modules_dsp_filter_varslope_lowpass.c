/*
 * cyperus_lowpass_module.c
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

#include "../../math_utils.h"
#include "math_modules_dsp_filter_varslope_lowpass.h"


typedef struct {
  double re, im;
} complex_float32_t;


/* Forward declaration for local functions */
static void cyperus_lowpass_module_hpeq(float N, float BW, float B_data[],
  int32_t B_size[2], float A_data[], int32_t A_size[2]);
static void cyperus_lowpass_module_all(const uint8_t x_data[], const int32_t
  x_size[2], uint8_t y[2]);
static uint8_t cyperus_lowpass_mo_anyNonFinite(const complex_float32_t x_data[], const
  int32_t x_size[2]);
static complex_float32_t cyperus_lowpass_module_sqrt(const complex_float32_t x);
static void cyperus_lowpass_modul_xzlartg_c(const complex_float32_t f, const complex_float32_t g,
  float *cs, complex_float32_t *sn);
static void cyperus_lowpass_module_xzlartg(const complex_float32_t f, const complex_float32_t g,
  float *cs, complex_float32_t *sn, complex_float32_t *r);
static void cyperus_lowpass_module_xzhgeqz(const complex_float32_t A_data[], const int32_t
  A_size[2], int32_t ilo, int32_t ihi, int32_t *info, complex_float32_t alpha1_data[],
  int32_t *alpha1_size, complex_float32_t beta1_data[], int32_t *beta1_size);
static void cyperus_lowpass_module_xzgeev(const complex_float32_t A_data[], const int32_t
  A_size[2], int32_t *info, complex_float32_t alpha1_data[], int32_t *alpha1_size, complex_float32_t
  beta1_data[], int32_t *beta1_size);
static float cyperus_lowpass_module_xnrm2(int32_t n, const complex_float32_t x_data[],
  int32_t ix0);
static float cyperus_lowpass_module_xdlapy3(float x1, float x2, float x3);
static complex_float32_t cyperus_lowpass_module_recip(const complex_float32_t y);
static void cyperus_lowpass_module_xgehrd(const complex_float32_t a_data[], const int32_t
  a_size[2], complex_float32_t b_a_data[], int32_t b_a_size[2]);
static void cyperus_lowpass_module_xzlarfg(const complex_float32_t alpha1, const complex_float32_t x,
  complex_float32_t *b_alpha1, complex_float32_t *b_x, complex_float32_t *tau);
static void cyperus_lowpass_modu_eml_zlahqr(const complex_float32_t h_data[], const
  int32_t h_size[2], complex_float32_t b_h_data[], int32_t b_h_size[2], int32_t *info);
static void cyperus_lowpass_module_eig(const complex_float32_t A_data[], const int32_t
  A_size[2], complex_float32_t V_data[], int32_t *V_size);
static void cyperus_lowpass_module_roots_c(const float c[5], complex_float32_t r_data[],
  int32_t *r_size);
static uint8_t cyperus_lowpass_module_iseq(float x, float y);
static uint8_t cyperus_lowpass_module_sortLE(const complex_float32_t v_data[], int32_t
  idx1, int32_t idx2);
static void cyperus_lowpass_module_sort_c(complex_float32_t x_data[], int32_t *x_size);
static void cyperus_lowpass_module_roots(const float c[3], complex_float32_t r_data[],
  int32_t *r_size);
static void cyperus_lowpa_designEachParamEQ(float N, float BW, float B_data[],
  int32_t B_size[2], float A_data[], int32_t A_size[2]);
static void cyperus_lo_designVarSlopeFilter(float Slope, float Fc, float B[12],
  float A[8]);


float rt_roundd_snf(float u)
{
  float y;
  if (fabs(u) < 4.503599627370496E+15) {
    if (u >= 0.5) {
      y = floor(u + 0.5);
    } else if (u > -0.5) {
      y = u * 0.0;
    } else {
      y = ceil(u - 0.5);
    }
  } else {
    y = u;
  }

  return y;
}

float rt_remd_snf(float u0, float u1)
{
  float y;
  float u1_0;
  if (rtIsNaN(u0) || rtIsNaN(u1) || rtIsInf(u0)) {
    y = (rtNaN);
  } else if (rtIsInf(u1)) {
    y = u0;
  } else {
    if (u1 < 0.0) {
      u1_0 = ceil(u1);
    } else {
      u1_0 = floor(u1);
    }

    if ((u1 != 0.0) && (u1 != u1_0)) {
      u1_0 = fabs(u0 / u1);
      if (fabs(u1_0 - floor(u1_0 + 0.5)) <= DBL_EPSILON * u1_0) {
        y = 0.0 * u0;
      } else {
        y = fmod(u0, u1);
      }
    } else {
      y = fmod(u0, u1);
    }
  }

  return y;
}

static void cyperus_lowpass_module_hpeq(float N, float BW, float B_data[],
  int32_t B_size[2], float A_data[], int32_t A_size[2])
{
  float r;
  float L;
  float WB;
  int32_t offset;
  float Ba_data[15];
  float Aa_data[15];
  uint32_t i_data[4];
  float si_data[4];
  float b_B_data[25];
  float b_A_data[25];
  uint32_t y_data[4];
  int32_t nx;
  float Bhat_data[15];
  float Ahat_data[15];
  int32_t i1idx_data[5];
  uint8_t x_data[5];
  int32_t ii_data[5];
  int32_t idx;
  float tmp_data[5];
  int32_t Ba_size_idx_0;
  int32_t Aa_size_idx_0;
  int32_t y_size_idx_1;
  float r_tmp;
  int32_t Bhat_data_tmp;
  float L_tmp;
  uint8_t exitg1;
  r = rt_remd_snf(N, 2.0);
  L = (N - r) / 2.0;
  WB = tan(3.1415926535897931 * BW / 2.0);
  offset = !(r == 0.0);
  Ba_size_idx_0 = (int32_t)(L + (float)offset);
  idx = (int32_t)(L + (float)offset) * 3 - 1;
  if (0 <= idx) {
    memset(&Ba_data[0], 0, (idx + 1) * sizeof(float));
  }

  Aa_size_idx_0 = (int32_t)(L + (float)offset);
  idx = (int32_t)(L + (float)offset) * 3 - 1;
  if (0 <= idx) {
    memset(&Aa_data[0], 0, (idx + 1) * sizeof(float));
  }

  if (r != 0.0) {
    Ba_data[0] = 0.0 * WB;
    Ba_data[Ba_size_idx_0] = 1.0;
    Ba_data[Ba_size_idx_0 << 1] = 0.0;
    Aa_data[0] = WB;
    Aa_data[Aa_size_idx_0] = 1.0;
    Aa_data[Aa_size_idx_0 << 1] = 0.0;
  }

  if (L > 0.0) {
    if (L < 1.0) {
      y_size_idx_1 = 0;
    } else {
      idx = (int32_t)floor(L - 1.0);
      y_size_idx_1 = idx + 1;
      for (nx = 0; nx <= idx; nx++) {
        y_data[nx] = 1U + nx;
      }
    }

    for (nx = 0; nx < y_size_idx_1; nx++) {
      si_data[nx] = (2.0 * (float)y_data[nx] - 1.0) / N * 3.1415926535897931 /
        2.0;
      i_data[nx] = y_data[nx];
    }

    nx = y_size_idx_1 - 1;
    for (idx = 0; idx <= nx; idx++) {
      si_data[idx] = sin(si_data[idx]);
    }

    r_tmp = WB * WB;
    r = r_tmp * 0.0;
    idx = (int32_t)L;
    for (nx = 0; nx < idx; nx++) {
      Ba_data[(int32_t)(offset + i_data[nx]) - 1] = r;
    }

    for (nx = 0; nx < y_size_idx_1; nx++) {
      Ba_data[((int32_t)(offset + i_data[nx]) + Ba_size_idx_0) - 1] = 0.0 *
        si_data[nx] * WB;
    }

    for (nx = 0; nx < idx; nx++) {
      Ba_data[((int32_t)(offset + i_data[nx]) + (Ba_size_idx_0 << 1)) - 1] = 1.0;
    }

    for (nx = 0; nx < idx; nx++) {
      Aa_data[(int32_t)(offset + i_data[nx]) - 1] = r_tmp;
    }

    for (nx = 0; nx < y_size_idx_1; nx++) {
      Aa_data[((int32_t)(offset + i_data[nx]) + Aa_size_idx_0) - 1] = 2.0 *
        si_data[nx] * WB;
    }

    for (nx = 0; nx < idx; nx++) {
      Aa_data[((int32_t)(offset + i_data[nx]) + (Aa_size_idx_0 << 1)) - 1] = 1.0;
    }
  }

  offset = Ba_size_idx_0 * 5 - 1;
  if (0 <= offset) {
    memset(&b_B_data[0], 0, (offset + 1) * sizeof(float));
    memset(&b_A_data[0], 0, (offset + 1) * sizeof(float));
  }

  nx = Ba_size_idx_0 * 3 - 1;
  if (0 <= nx) {
    memset(&Bhat_data[0], 0, (nx + 1) * sizeof(float));
    memset(&Ahat_data[0], 0, (nx + 1) * sizeof(float));
  }

  for (nx = 0; nx < Ba_size_idx_0; nx++) {
    x_data[nx] = (((Ba_data[nx + Ba_size_idx_0] != 0.0) || (Aa_data[nx +
      Aa_size_idx_0] != 0.0)) && ((Ba_data[(Ba_size_idx_0 << 1) + nx] == 0.0) &&
      (Aa_data[(Aa_size_idx_0 << 1) + nx] == 0.0)));
  }

  idx = 0;
  y_size_idx_1 = Ba_size_idx_0;
  nx = 0;
  exitg1 = 0;
  while ((!exitg1) && (nx <= Ba_size_idx_0 - 1)) {
    if (x_data[nx]) {
      idx++;
      ii_data[idx - 1] = nx + 1;
      if (idx >= Ba_size_idx_0) {
        exitg1 = 1;
      } else {
        nx++;
      }
    } else {
      nx++;
    }
  }

  if (Ba_size_idx_0 == 1) {
    if (idx == 0) {
      y_size_idx_1 = 0;
    }
  } else {
    if (1 > idx) {
      idx = 0;
    }

    y_size_idx_1 = idx;
  }

  if (0 <= y_size_idx_1 - 1) {
    memcpy(&i1idx_data[0], &ii_data[0], y_size_idx_1 * sizeof(int32_t));
  }

  if (y_size_idx_1 != 0) {
    r = Aa_data[i1idx_data[0] - 1];
    WB = Aa_data[(i1idx_data[0] + Aa_size_idx_0) - 1];
    L = WB + r;
    r_tmp = Ba_data[i1idx_data[0] - 1];
    y_size_idx_1 = (i1idx_data[0] + Ba_size_idx_0) - 1;
    Bhat_data[i1idx_data[0] - 1] = (Ba_data[y_size_idx_1] + r_tmp) / L;
    Bhat_data[y_size_idx_1] = (r_tmp - Ba_data[y_size_idx_1]) / L;
    Ahat_data[i1idx_data[0] - 1] = 1.0;
    Ahat_data[y_size_idx_1] = (r - WB) / L;
  }

  for (nx = 0; nx < Ba_size_idx_0; nx++) {
    x_data[nx] = ((Ba_data[(Ba_size_idx_0 << 1) + nx] != 0.0) || (Aa_data
      [(Aa_size_idx_0 << 1) + nx] != 0.0));
  }

  idx = 0;
  y_size_idx_1 = Ba_size_idx_0;
  nx = 0;
  exitg1 = 0;
  while ((!exitg1) && (nx <= Ba_size_idx_0 - 1)) {
    if (x_data[nx]) {
      idx++;
      ii_data[idx - 1] = nx + 1;
      if (idx >= Ba_size_idx_0) {
        exitg1 = 1;
      } else {
        nx++;
      }
    } else {
      nx++;
    }
  }

  if (Ba_size_idx_0 == 1) {
    if (idx == 0) {
      y_size_idx_1 = 0;
    }
  } else {
    if (1 > idx) {
      idx = 0;
    }

    y_size_idx_1 = idx;
  }

  if (0 <= y_size_idx_1 - 1) {
    memcpy(&i1idx_data[0], &ii_data[0], y_size_idx_1 * sizeof(int32_t));
  }

  nx = y_size_idx_1 - 1;
  for (idx = 0; idx <= nx; idx++) {
    r = Aa_data[i1idx_data[idx] - 1];
    WB = Aa_data[((Aa_size_idx_0 << 1) + i1idx_data[idx]) - 1];
    L_tmp = Aa_data[(i1idx_data[idx] + Aa_size_idx_0) - 1];
    L = (L_tmp + r) + WB;
    r_tmp = Ba_data[i1idx_data[idx] - 1];
    y_size_idx_1 = ((Ba_size_idx_0 << 1) + i1idx_data[idx]) - 1;
    Bhat_data_tmp = (i1idx_data[idx] + Ba_size_idx_0) - 1;
    Bhat_data[i1idx_data[idx] - 1] = ((Ba_data[Bhat_data_tmp] + r_tmp) +
      Ba_data[y_size_idx_1]) / L;
    Bhat_data[Bhat_data_tmp] = (r_tmp - Ba_data[y_size_idx_1]) * 2.0 / L;
    Bhat_data[y_size_idx_1] = ((r_tmp - Ba_data[(i1idx_data[idx] + Ba_size_idx_0)
      - 1]) + Ba_data[y_size_idx_1]) / L;
    Ahat_data[i1idx_data[idx] - 1] = 1.0;
    Ahat_data[Bhat_data_tmp] = (r - WB) * 2.0 / L;
    Ahat_data[y_size_idx_1] = ((r - L_tmp) + WB) / L;
  }

  for (nx = 0; nx < 3; nx++) {
    for (Aa_size_idx_0 = 0; Aa_size_idx_0 < Ba_size_idx_0; Aa_size_idx_0++) {
      idx = Ba_size_idx_0 * nx + Aa_size_idx_0;
      b_B_data[Aa_size_idx_0 + Ba_size_idx_0 * nx] = Bhat_data[idx];
      b_A_data[idx] = Ahat_data[idx];
    }
  }

  idx = Ba_size_idx_0 - 1;
  Aa_size_idx_0 = idx + 1;
  for (nx = 0; nx <= idx; nx++) {
    tmp_data[nx] = -b_B_data[nx + Ba_size_idx_0];
  }

  for (nx = 0; nx < Aa_size_idx_0; nx++) {
    b_B_data[nx + Ba_size_idx_0] = tmp_data[nx];
  }

  idx = Ba_size_idx_0 - 1;
  Aa_size_idx_0 = idx + 1;
  for (nx = 0; nx <= idx; nx++) {
    tmp_data[nx] = -b_A_data[nx + Ba_size_idx_0];
  }

  for (nx = 0; nx < Aa_size_idx_0; nx++) {
    b_A_data[nx + Ba_size_idx_0] = tmp_data[nx];
  }

  B_size[0] = Ba_size_idx_0;
  B_size[1] = 5;
  if (0 <= offset) {
    memcpy(&B_data[0], &b_B_data[0], (offset + 1) * sizeof(float));
    memcpy(&A_data[0], &b_A_data[0], (offset + 1) * sizeof(float));
  }

  A_size[0] = Ba_size_idx_0;
  A_size[1] = 5;
}

static void cyperus_lowpass_module_all(const uint8_t x_data[], const int32_t
  x_size[2], uint8_t y[2])
{
  int32_t i2;
  int32_t i1;
  uint8_t exitg1;
  y[0] = 1;
  y[1] = 1;
  i1 = 1;
  i2 = -1 + x_size[0];
  exitg1 = 0;
  while ((!exitg1) && (i1 <= i2 + 1)) {
    if (!x_data[i1 - 1]) {
      y[0] = 0;
      exitg1 = 1;
    } else {
      i1++;
    }
  }

  i1 = i2 + 2;
  i2 += x_size[0];
  exitg1 = 0;
  while ((!exitg1) && (i1 <= i2 + 1)) {
    if (!x_data[i1 - 1]) {
      y[1] = 0;
      exitg1 = 1;
    } else {
      i1++;
    }
  }
}

static uint8_t cyperus_lowpass_mo_anyNonFinite(const complex_float32_t x_data[], const
  int32_t x_size[2])
{
  int32_t nx;
  uint8_t b_p;
  int32_t b_k;
  nx = x_size[0] * x_size[1] - 1;
  b_p = 1;
  for (b_k = 0; b_k <= nx; b_k++) {
    if (b_p && ((!rtIsInf(x_data[b_k].re)) && (!rtIsInf(x_data[b_k].im)) &&
                ((!rtIsNaN(x_data[b_k].re)) && (!rtIsNaN(x_data[b_k].im))))) {
    } else {
      b_p = 0;
    }
  }

  return !b_p;
}

float rt_hypotd_snf(float u0, float u1)
{
  float y;
  float a;
  a = fabs(u0);
  y = fabs(u1);
  if (a < y) {
    a /= y;
    y *= sqrt(a * a + 1.0);
  } else if (a > y) {
    y /= a;
    y = sqrt(y * y + 1.0) * a;
  } else {
    if (!rtIsNaN(y)) {
      y = a * 1.4142135623730951;
    }
  }

  return y;
}

static complex_float32_t cyperus_lowpass_module_sqrt(const complex_float32_t x)
{
  complex_float32_t b_x;
  float xr;
  float absxr;
  xr = x.re;
  if (x.im == 0.0) {
    if (x.re < 0.0) {
      absxr = 0.0;
      xr = sqrt(-x.re);
    } else {
      absxr = sqrt(x.re);
      xr = 0.0;
    }
  } else if (x.re == 0.0) {
    if (x.im < 0.0) {
      absxr = sqrt(-x.im / 2.0);
      xr = -absxr;
    } else {
      absxr = sqrt(x.im / 2.0);
      xr = absxr;
    }
  } else if (rtIsNaN(x.re)) {
    absxr = x.re;
  } else if (rtIsNaN(x.im)) {
    absxr = x.im;
    xr = x.im;
  } else if (rtIsInf(x.im)) {
    absxr = fabs(x.im);
    xr = x.im;
  } else if (rtIsInf(x.re)) {
    if (x.re < 0.0) {
      absxr = 0.0;
      xr = x.im * -x.re;
    } else {
      absxr = x.re;
      xr = 0.0;
    }
  } else {
    absxr = fabs(x.re);
    xr = fabs(x.im);
    if ((absxr > 4.4942328371557893E+307) || (xr > 4.4942328371557893E+307)) {
      absxr *= 0.5;
      xr = rt_hypotd_snf(absxr, xr * 0.5);
      if (xr > absxr) {
        absxr = sqrt(absxr / xr + 1.0) * sqrt(xr);
      } else {
        absxr = sqrt(xr) * 1.4142135623730951;
      }
    } else {
      absxr = sqrt((rt_hypotd_snf(absxr, xr) + absxr) * 0.5);
    }

    if (x.re > 0.0) {
      xr = x.im / absxr * 0.5;
    } else {
      if (x.im < 0.0) {
        xr = -absxr;
      } else {
        xr = absxr;
      }

      absxr = x.im / xr * 0.5;
    }
  }

  b_x.re = absxr;
  b_x.im = xr;
  return b_x;
}

static void cyperus_lowpass_modul_xzlartg_c(const complex_float32_t f, const complex_float32_t g,
  float *cs, complex_float32_t *sn)
{
  float scale;
  float g2;
  float f2s;
  float di;
  float fs_re;
  float fs_im;
  float gs_re;
  float gs_im;
  float g2_tmp;
  uint8_t guard1 = 0;
  di = fabs(f.re);
  scale = di;
  g2_tmp = fabs(f.im);
  if (g2_tmp > di) {
    scale = g2_tmp;
  }

  g2 = fabs(g.re);
  f2s = fabs(g.im);
  if (f2s > g2) {
    g2 = f2s;
  }

  if (g2 > scale) {
    scale = g2;
  }

  fs_re = f.re;
  fs_im = f.im;
  gs_re = g.re;
  gs_im = g.im;
  guard1 = 0;
  if (scale >= 7.4428285367870146E+137) {
    do {
      fs_re *= 1.3435752215134178E-138;
      fs_im *= 1.3435752215134178E-138;
      gs_re *= 1.3435752215134178E-138;
      gs_im *= 1.3435752215134178E-138;
      scale *= 1.3435752215134178E-138;
    } while (!(scale < 7.4428285367870146E+137));

    guard1 = 1;
  } else if (scale <= 1.3435752215134178E-138) {
    if ((g.re == 0.0) && (g.im == 0.0)) {
      *cs = 1.0;
      sn->re = 0.0;
      sn->im = 0.0;
    } else {
      do {
        fs_re *= 7.4428285367870146E+137;
        fs_im *= 7.4428285367870146E+137;
        gs_re *= 7.4428285367870146E+137;
        gs_im *= 7.4428285367870146E+137;
        scale *= 7.4428285367870146E+137;
      } while (!(scale > 1.3435752215134178E-138));

      guard1 = 1;
    }
  } else {
    guard1 = 1;
  }

  if (guard1) {
    scale = fs_re * fs_re + fs_im * fs_im;
    g2 = gs_re * gs_re + gs_im * gs_im;
    f2s = g2;
    if (1.0 > g2) {
      f2s = 1.0;
    }

    if (scale <= f2s * 2.0041683600089728E-292) {
      if ((f.re == 0.0) && (f.im == 0.0)) {
        *cs = 0.0;
        g2 = rt_hypotd_snf(gs_re, gs_im);
        sn->re = gs_re / g2;
        sn->im = -gs_im / g2;
      } else {
        scale = sqrt(g2);
        *cs = rt_hypotd_snf(fs_re, fs_im) / scale;
        g2 = di;
        if (g2_tmp > di) {
          g2 = g2_tmp;
        }

        if (g2 > 1.0) {
          g2 = rt_hypotd_snf(f.re, f.im);
          fs_re = f.re / g2;
          fs_im = f.im / g2;
        } else {
          f2s = 7.4428285367870146E+137 * f.re;
          di = 7.4428285367870146E+137 * f.im;
          g2 = rt_hypotd_snf(f2s, di);
          fs_re = f2s / g2;
          fs_im = di / g2;
        }

        gs_re /= scale;
        gs_im = -gs_im / scale;
        sn->re = fs_re * gs_re - fs_im * gs_im;
        sn->im = fs_re * gs_im + fs_im * gs_re;
      }
    } else {
      f2s = sqrt(g2 / scale + 1.0);
      fs_re *= f2s;
      fs_im *= f2s;
      *cs = 1.0 / f2s;
      g2 += scale;
      fs_re /= g2;
      fs_im /= g2;
      sn->re = fs_re * gs_re - fs_im * -gs_im;
      sn->im = fs_re * -gs_im + fs_im * gs_re;
    }
  }
}

static void cyperus_lowpass_module_xzlartg(const complex_float32_t f, const complex_float32_t g,
  float *cs, complex_float32_t *sn, complex_float32_t *r)
{
  float scale;
  int32_t count;
  float rescaledir;
  float f2;
  float g2;
  int32_t b_i;
  float fs_re;
  float fs_im;
  float gs_re;
  float gs_im;
  float scale_tmp;
  float f2_tmp;
  uint8_t guard1 = 0;
  scale_tmp = fabs(f.re);
  scale = scale_tmp;
  f2_tmp = fabs(f.im);
  if (f2_tmp > scale_tmp) {
    scale = f2_tmp;
  }

  f2 = fabs(g.re);
  g2 = fabs(g.im);
  if (g2 > f2) {
    f2 = g2;
  }

  if (f2 > scale) {
    scale = f2;
  }

  fs_re = f.re;
  fs_im = f.im;
  gs_re = g.re;
  gs_im = g.im;
  count = -1;
  rescaledir = 0.0;
  guard1 = 0;
  if (scale >= 7.4428285367870146E+137) {
    do {
      count++;
      fs_re *= 1.3435752215134178E-138;
      fs_im *= 1.3435752215134178E-138;
      gs_re *= 1.3435752215134178E-138;
      gs_im *= 1.3435752215134178E-138;
      scale *= 1.3435752215134178E-138;
    } while (!(scale < 7.4428285367870146E+137));

    rescaledir = 1.0;
    guard1 = 1;
  } else if (scale <= 1.3435752215134178E-138) {
    if ((g.re == 0.0) && (g.im == 0.0)) {
      *cs = 1.0;
      sn->re = 0.0;
      sn->im = 0.0;
      *r = f;
    } else {
      do {
        count++;
        fs_re *= 7.4428285367870146E+137;
        fs_im *= 7.4428285367870146E+137;
        gs_re *= 7.4428285367870146E+137;
        gs_im *= 7.4428285367870146E+137;
        scale *= 7.4428285367870146E+137;
      } while (!(scale > 1.3435752215134178E-138));

      rescaledir = -1.0;
      guard1 = 1;
    }
  } else {
    guard1 = 1;
  }

  if (guard1) {
    f2 = fs_re * fs_re + fs_im * fs_im;
    g2 = gs_re * gs_re + gs_im * gs_im;
    scale = g2;
    if (1.0 > g2) {
      scale = 1.0;
    }

    if (f2 <= scale * 2.0041683600089728E-292) {
      if ((f.re == 0.0) && (f.im == 0.0)) {
        *cs = 0.0;
        r->re = rt_hypotd_snf(g.re, g.im);
        r->im = 0.0;
        f2 = rt_hypotd_snf(gs_re, gs_im);
        sn->re = gs_re / f2;
        sn->im = -gs_im / f2;
      } else {
        rescaledir = sqrt(g2);
        *cs = rt_hypotd_snf(fs_re, fs_im) / rescaledir;
        f2 = scale_tmp;
        if (f2_tmp > scale_tmp) {
          f2 = f2_tmp;
        }

        if (f2 > 1.0) {
          f2 = rt_hypotd_snf(f.re, f.im);
          fs_re = f.re / f2;
          fs_im = f.im / f2;
        } else {
          g2 = 7.4428285367870146E+137 * f.re;
          scale = 7.4428285367870146E+137 * f.im;
          f2 = rt_hypotd_snf(g2, scale);
          fs_re = g2 / f2;
          fs_im = scale / f2;
        }

        gs_re /= rescaledir;
        gs_im = -gs_im / rescaledir;
        sn->re = fs_re * gs_re - fs_im * gs_im;
        sn->im = fs_re * gs_im + fs_im * gs_re;
        r->re = (sn->re * g.re - sn->im * g.im) + *cs * f.re;
        r->im = (sn->re * g.im + sn->im * g.re) + *cs * f.im;
      }
    } else {
      scale = sqrt(g2 / f2 + 1.0);
      r->re = scale * fs_re;
      r->im = scale * fs_im;
      *cs = 1.0 / scale;
      f2 += g2;
      scale_tmp = r->re / f2;
      f2 = r->im / f2;
      sn->re = scale_tmp * gs_re - f2 * -gs_im;
      sn->im = scale_tmp * -gs_im + f2 * gs_re;
      if (rescaledir > 0.0) {
        for (b_i = 0; b_i <= count; b_i++) {
          r->re *= 7.4428285367870146E+137;
          r->im *= 7.4428285367870146E+137;
        }
      } else {
        if (rescaledir < 0.0) {
          for (b_i = 0; b_i <= count; b_i++) {
            r->re *= 1.3435752215134178E-138;
            r->im *= 1.3435752215134178E-138;
          }
        }
      }
    }
  }
}

static void cyperus_lowpass_module_xzhgeqz(const complex_float32_t A_data[], const int32_t
  A_size[2], int32_t ilo, int32_t ihi, int32_t *info, complex_float32_t alpha1_data[],
  int32_t *alpha1_size, complex_float32_t beta1_data[], int32_t *beta1_size)
{
  int32_t n;
  complex_float32_t ctemp;
  float anorm;
  int32_t ifirst;
  int32_t istart;
  int32_t ilastm1;
  int32_t ifrstm;
  int32_t iiter;
  uint8_t goto60;
  uint8_t goto70;
  uint8_t goto90;
  int32_t jp1;
  complex_float32_t ad11;
  complex_float32_t shift;
  complex_float32_t b_A_data[16];
  int32_t jiter;
  float scale;
  float sumsq;
  uint8_t firstNonZero;
  float imAij;
  float temp1;
  int32_t j;
  int32_t b_x;
  int32_t b_A_size_idx_0;
  complex_float32_t sumsq_0;
  complex_float32_t t1;
  float ar;
  float ai;
  float t1_im;
  float sumsq_im;
  float eshift_re;
  float eshift_im;
  int32_t ctemp_tmp;
  int32_t t1_re_tmp;
  float t1_tmp;
  uint8_t guard1 = 0;
  uint8_t guard2 = 0;
  uint8_t guard3 = 0;
  int32_t exitg1;
  uint8_t exitg2;
  uint8_t guard11 = 0;
  b_A_size_idx_0 = A_size[0];
  ifirst = A_size[0] * A_size[1] - 1;
  if (0 <= ifirst) {
    memcpy(&b_A_data[0], &A_data[0], (ifirst + 1) * sizeof(float));
  }

  *info = 0;
  if ((A_size[0] == 1) && (A_size[1] == 1)) {
    ihi = 1;
  }

  n = A_size[0];
  *alpha1_size = A_size[0];
  ifirst = A_size[0];
  if (0 <= ifirst - 1) {
    memset(&alpha1_data[0], 0, ifirst * sizeof(float));
  }

  *beta1_size = A_size[0];
  ifirst = A_size[0];
  for (ctemp_tmp = 0; ctemp_tmp < ifirst; ctemp_tmp++) {
    beta1_data[ctemp_tmp].re = 1.0;
    beta1_data[ctemp_tmp].im = 0.0;
  }

  eshift_re = 0.0;
  eshift_im = 0.0;
  ctemp.re = 0.0;
  ctemp.im = 0.0;
  anorm = 0.0;
  if (ilo <= ihi) {
    scale = 0.0;
    sumsq = 0.0;
    firstNonZero = 1;
    for (j = ilo; j <= ihi; j++) {
      ifirst = j + 1;
      if (ihi < j + 1) {
        ifirst = ihi;
      }

      istart = A_size[0];
      for (jp1 = ilo; jp1 <= ifirst; jp1++) {
        anorm = A_data[((j - 1) * istart + jp1) - 1].re;
        imAij = A_data[((j - 1) * istart + jp1) - 1].im;
        if (anorm != 0.0) {
          temp1 = fabs(anorm);
          if (firstNonZero) {
            sumsq = 1.0;
            scale = temp1;
            firstNonZero = 0;
          } else if (scale < temp1) {
            t1_im = scale / temp1;
            sumsq = sumsq * t1_im * t1_im + 1.0;
            scale = temp1;
          } else {
            t1_im = temp1 / scale;
            sumsq += t1_im * t1_im;
          }
        }

        if (imAij != 0.0) {
          temp1 = fabs(imAij);
          if (firstNonZero) {
            sumsq = 1.0;
            scale = temp1;
            firstNonZero = 0;
          } else if (scale < temp1) {
            imAij = scale / temp1;
            sumsq = sumsq * imAij * imAij + 1.0;
            scale = temp1;
          } else {
            imAij = temp1 / scale;
            sumsq += imAij * imAij;
          }
        }
      }
    }

    anorm = scale * sqrt(sumsq);
  }

  sumsq = 2.2204460492503131E-16 * anorm;
  scale = 2.2250738585072014E-308;
  if (sumsq > 2.2250738585072014E-308) {
    scale = sumsq;
  }

  sumsq = 2.2250738585072014E-308;
  if (anorm > 2.2250738585072014E-308) {
    sumsq = anorm;
  }

  sumsq = 1.0 / sumsq;
  imAij = 1.0 / sqrt(A_size[0]);
  firstNonZero = 1;
  for (ifirst = ihi + 1; ifirst <= n; ifirst++) {
    alpha1_data[ifirst - 1] = A_data[((ifirst - 1) * A_size[0] + ifirst) - 1];
  }

  guard1 = 0;
  guard2 = 0;
  if (ihi >= ilo) {
    ifirst = ilo;
    istart = ilo;
    n = ihi - 1;
    ilastm1 = ihi - 2;
    ifrstm = ilo;
    iiter = 0;
    goto60 = 0;
    goto70 = 0;
    goto90 = 0;
    jiter = 0;
    do {
      exitg1 = 0;
      if (jiter <= ((ihi - ilo) + 1) * 30 - 1) {
        guard11 = 0;
        if (n + 1 == ilo) {
          goto60 = 1;
          guard11 = 1;
        } else {
          ctemp_tmp = b_A_size_idx_0 * ilastm1 + n;
          if (fabs(b_A_data[ctemp_tmp].re) + fabs(b_A_data[b_A_size_idx_0 *
               ilastm1 + n].im) <= scale) {
            b_A_data[ctemp_tmp].re = 0.0;
            b_A_data[ctemp_tmp].im = 0.0;
            goto60 = 1;
            guard11 = 1;
          } else {
            j = ilastm1 + 1;
            guard3 = 0;
            exitg2 = 0;
            while ((!exitg2) && (j >= ilo)) {
              if (j == ilo) {
                guard3 = 1;
                exitg2 = 1;
              } else {
                ctemp_tmp = ((j - 2) * b_A_size_idx_0 + j) - 1;
                if (fabs(b_A_data[ctemp_tmp].re) + fabs(b_A_data[((j - 2) *
                      b_A_size_idx_0 + j) - 1].im) <= scale) {
                  b_A_data[ctemp_tmp].re = 0.0;
                  b_A_data[ctemp_tmp].im = 0.0;
                  guard3 = 1;
                  exitg2 = 1;
                } else {
                  j--;
                  guard3 = 0;
                }
              }
            }

            if (guard3) {
              ifirst = j;
              goto70 = 1;
            }

            if (goto70) {
              guard11 = 1;
            } else {
              j = *alpha1_size;
              for (ctemp_tmp = 0; ctemp_tmp < j; ctemp_tmp++) {
                alpha1_data[ctemp_tmp].re = (rtNaN);
                alpha1_data[ctemp_tmp].im = 0.0;
              }

              j = *beta1_size;
              for (ctemp_tmp = 0; ctemp_tmp < j; ctemp_tmp++) {
                beta1_data[ctemp_tmp].re = (rtNaN);
                beta1_data[ctemp_tmp].im = 0.0;
              }

              *info = 1;
              exitg1 = 1;
            }
          }
        }

        if (guard11) {
          if (goto60) {
            goto60 = 0;
            alpha1_data[n] = b_A_data[b_A_size_idx_0 * n + n];
            n = ilastm1;
            ilastm1--;
            if (n + 1 < ilo) {
              firstNonZero = 0;
              guard2 = 1;
              exitg1 = 1;
            } else {
              iiter = 0;
              eshift_re = 0.0;
              eshift_im = 0.0;
              if (ifrstm > n + 1) {
                ifrstm = ilo;
              }

              jiter++;
            }
          } else {
            if (goto70) {
              goto70 = 0;
              iiter++;
              ifrstm = ifirst;
              if (iiter - iiter / 10 * 10 != 0) {
                ar = b_A_data[b_A_size_idx_0 * ilastm1 + ilastm1].re * sumsq;
                ai = b_A_data[b_A_size_idx_0 * ilastm1 + ilastm1].im * sumsq;
                if (ai == 0.0) {
                  ad11.re = ar / imAij;
                  ad11.im = 0.0;
                } else if (ar == 0.0) {
                  ad11.re = 0.0;
                  ad11.im = ai / imAij;
                } else {
                  ad11.re = ar / imAij;
                  ad11.im = ai / imAij;
                }

                ar = b_A_data[b_A_size_idx_0 * n + n].re * sumsq;
                ai = b_A_data[b_A_size_idx_0 * n + n].im * sumsq;
                if (ai == 0.0) {
                  shift.re = ar / imAij;
                  shift.im = 0.0;
                } else if (ar == 0.0) {
                  shift.re = 0.0;
                  shift.im = ai / imAij;
                } else {
                  shift.re = ar / imAij;
                  shift.im = ai / imAij;
                }

                temp1 = (ad11.re + shift.re) * 0.5;
                t1_im = (ad11.im + shift.im) * 0.5;
                j = b_A_size_idx_0 * n + ilastm1;
                ar = b_A_data[j].re * sumsq;
                ai = b_A_data[j].im * sumsq;
                if (ai == 0.0) {
                  anorm = ar / imAij;
                  sumsq_im = 0.0;
                } else if (ar == 0.0) {
                  anorm = 0.0;
                  sumsq_im = ai / imAij;
                } else {
                  anorm = ar / imAij;
                  sumsq_im = ai / imAij;
                }

                ar = b_A_data[b_A_size_idx_0 * ilastm1 + n].re * sumsq;
                ai = b_A_data[b_A_size_idx_0 * ilastm1 + n].im * sumsq;
                if (ai == 0.0) {
                  ar /= imAij;
                  ai = 0.0;
                } else if (ar == 0.0) {
                  ar = 0.0;
                  ai /= imAij;
                } else {
                  ar /= imAij;
                  ai /= imAij;
                }

                t1.re = ((temp1 * temp1 - t1_im * t1_im) + (anorm * ar -
                          sumsq_im * ai)) - (ad11.re * shift.re - ad11.im *
                  shift.im);
                t1_tmp = temp1 * t1_im;
                t1.im = ((t1_tmp + t1_tmp) + (anorm * ai + sumsq_im * ar)) -
                  (ad11.re * shift.im + ad11.im * shift.re);
                ad11 = cyperus_lowpass_module_sqrt(t1);
                if ((temp1 - shift.re) * ad11.re + (t1_im - shift.im) * ad11.im <=
                    0.0) {
                  shift.re = temp1 + ad11.re;
                  shift.im = t1_im + ad11.im;
                } else {
                  shift.re = temp1 - ad11.re;
                  shift.im = t1_im - ad11.im;
                }
              } else {
                ar = b_A_data[b_A_size_idx_0 * ilastm1 + n].re * sumsq;
                ai = b_A_data[b_A_size_idx_0 * ilastm1 + n].im * sumsq;
                if (ai == 0.0) {
                  anorm = ar / imAij;
                  sumsq_im = 0.0;
                } else if (ar == 0.0) {
                  anorm = 0.0;
                  sumsq_im = ai / imAij;
                } else {
                  anorm = ar / imAij;
                  sumsq_im = ai / imAij;
                }

                eshift_re += anorm;
                eshift_im += sumsq_im;
                shift.re = eshift_re;
                shift.im = eshift_im;
              }

              j = ilastm1;
              jp1 = ilastm1 + 1;
              exitg2 = 0;
              while ((!exitg2) && (j + 1 > ifirst)) {
                istart = j + 1;
                ctemp_tmp = b_A_size_idx_0 * j;
                ctemp.re = b_A_data[ctemp_tmp + j].re * sumsq - shift.re * imAij;
                ctemp.im = b_A_data[b_A_size_idx_0 * j + j].im * sumsq -
                  shift.im * imAij;
                temp1 = fabs(ctemp.re) + fabs(ctemp.im);
                anorm = (fabs(b_A_data[ctemp_tmp + jp1].re) + fabs
                         (b_A_data[b_A_size_idx_0 * j + jp1].im)) * sumsq;
                t1_im = temp1;
                if (anorm > temp1) {
                  t1_im = anorm;
                }

                if ((t1_im < 1.0) && (t1_im != 0.0)) {
                  temp1 /= t1_im;
                  anorm /= t1_im;
                }

                if ((fabs(b_A_data[(j - 1) * b_A_size_idx_0 + j].re) + fabs
                     (b_A_data[(j - 1) * b_A_size_idx_0 + j].im)) * anorm <=
                    temp1 * scale) {
                  goto90 = 1;
                  exitg2 = 1;
                } else {
                  jp1 = j;
                  j--;
                }
              }

              if (!goto90) {
                istart = ifirst;
                ctemp.re = b_A_data[((ifirst - 1) * b_A_size_idx_0 + ifirst) - 1]
                  .re * sumsq - shift.re * imAij;
                ctemp.im = b_A_data[((ifirst - 1) * b_A_size_idx_0 + ifirst) - 1]
                  .im * sumsq - shift.im * imAij;
                goto90 = 1;
              }
            }

            if (goto90) {
              goto90 = 0;
              sumsq_0.re = b_A_data[(istart - 1) * b_A_size_idx_0 + istart].re *
                sumsq;
              sumsq_0.im = b_A_data[(istart - 1) * b_A_size_idx_0 + istart].im *
                sumsq;
              cyperus_lowpass_modul_xzlartg_c(ctemp, sumsq_0, &anorm, &shift);
              j = istart;
              jp1 = istart - 2;
              while (j < n + 1) {
                if (j > istart) {
                  cyperus_lowpass_module_xzlartg(b_A_data[(j + b_A_size_idx_0 *
                    jp1) - 1], b_A_data[j + b_A_size_idx_0 * jp1], &anorm,
                    &shift, &b_A_data[(j + b_A_size_idx_0 * jp1) - 1]);
                  ctemp_tmp = j + b_A_size_idx_0 * jp1;
                  b_A_data[ctemp_tmp].re = 0.0;
                  b_A_data[ctemp_tmp].im = 0.0;
                }

                for (jp1 = j; jp1 <= n + 1; jp1++) {
                  ctemp_tmp = (jp1 - 1) * b_A_size_idx_0 + j;
                  temp1 = b_A_data[ctemp_tmp - 1].re * anorm +
                    (b_A_data[ctemp_tmp].re * shift.re - b_A_data[(jp1 - 1) *
                     b_A_size_idx_0 + j].im * shift.im);
                  t1_im = b_A_data[((jp1 - 1) * b_A_size_idx_0 + j) - 1].im *
                    anorm + (b_A_data[(jp1 - 1) * b_A_size_idx_0 + j].im *
                             shift.re + b_A_data[(jp1 - 1) * b_A_size_idx_0 + j]
                             .re * shift.im);
                  sumsq_im = b_A_data[((jp1 - 1) * b_A_size_idx_0 + j) - 1].re;
                  ctemp_tmp = (jp1 - 1) * b_A_size_idx_0 + j;
                  t1_re_tmp = ctemp_tmp - 1;
                  b_A_data[ctemp_tmp].re = b_A_data[ctemp_tmp].re * anorm -
                    (b_A_data[t1_re_tmp].re * shift.re + b_A_data[((jp1 - 1) *
                      b_A_size_idx_0 + j) - 1].im * shift.im);
                  b_A_data[ctemp_tmp].im = b_A_data[ctemp_tmp].im * anorm -
                    (b_A_data[t1_re_tmp].im * shift.re - shift.im * sumsq_im);
                  b_A_data[t1_re_tmp].re = temp1;
                  b_A_data[t1_re_tmp].im = t1_im;
                }

                shift.re = -shift.re;
                shift.im = -shift.im;
                b_x = j + 2;
                if (n + 1 < j + 2) {
                  b_x = n + 1;
                }

                for (jp1 = ifrstm; jp1 <= b_x; jp1++) {
                  ctemp_tmp = (j - 1) * b_A_size_idx_0;
                  t1_re_tmp = b_A_size_idx_0 * j;
                  temp1 = (b_A_data[(ctemp_tmp + jp1) - 1].re * shift.re -
                           b_A_data[((j - 1) * b_A_size_idx_0 + jp1) - 1].im *
                           shift.im) + b_A_data[(t1_re_tmp + jp1) - 1].re *
                    anorm;
                  t1_im = (b_A_data[((j - 1) * b_A_size_idx_0 + jp1) - 1].im *
                           shift.re + b_A_data[((j - 1) * b_A_size_idx_0 + jp1)
                           - 1].re * shift.im) + b_A_data[(b_A_size_idx_0 * j +
                    jp1) - 1].im * anorm;
                  sumsq_im = b_A_data[(b_A_size_idx_0 * j + jp1) - 1].re;
                  ctemp_tmp = (ctemp_tmp + jp1) - 1;
                  t1_re_tmp = (t1_re_tmp + jp1) - 1;
                  b_A_data[ctemp_tmp].re = b_A_data[ctemp_tmp].re * anorm -
                    (b_A_data[t1_re_tmp].re * shift.re + b_A_data
                     [(b_A_size_idx_0 * j + jp1) - 1].im * shift.im);
                  b_A_data[ctemp_tmp].im = b_A_data[ctemp_tmp].im * anorm -
                    (b_A_data[t1_re_tmp].im * shift.re - shift.im * sumsq_im);
                  b_A_data[t1_re_tmp].re = temp1;
                  b_A_data[t1_re_tmp].im = t1_im;
                }

                jp1 = j - 1;
                j++;
              }
            }

            jiter++;
          }
        }
      } else {
        guard2 = 1;
        exitg1 = 1;
      }
    } while (exitg1 == 0);
  } else {
    guard1 = 1;
  }

  if (guard2) {
    if (firstNonZero) {
      *info = n + 1;
      for (j = 0; j <= n; j++) {
        alpha1_data[j].re = (rtNaN);
        alpha1_data[j].im = 0.0;
        beta1_data[j].re = (rtNaN);
        beta1_data[j].im = 0.0;
      }
    } else {
      guard1 = 1;
    }
  }

  if (guard1) {
    n = ilo - 2;
    for (ifirst = 0; ifirst <= n; ifirst++) {
      alpha1_data[ifirst] = b_A_data[b_A_size_idx_0 * ifirst + ifirst];
    }
  }
}

static void cyperus_lowpass_module_xzgeev(const complex_float32_t A_data[], const int32_t
  A_size[2], int32_t *info, complex_float32_t alpha1_data[], int32_t *alpha1_size, complex_float32_t
  beta1_data[], int32_t *beta1_size)
{
  complex_float32_t At_data[16];
  float anrm;
  uint8_t ilascl;
  float anrmto;
  complex_float32_t c_A_data[16];
  complex_float32_t d_A_data[16];
  float absxk;
  float ctoc;
  uint8_t notdone;
  int32_t i;
  int32_t j;
  complex_float32_t c_A_data_0[16];
  complex_float32_t d_A_data_0[16];
  int32_t ii;
  int32_t jrow;
  complex_float32_t s;
  int32_t jcol;
  float cto1;
  float mul;
  int32_t d_A_size[2];
  int32_t At_size_idx_0;
  float atmp_im;
  complex_float32_t alpha1_data_0;
  int32_t loop_ub_tmp;
  int32_t loop_ub_tmp_0;
  int32_t c_A_data_tmp;
  int32_t d_A_data_tmp;
  uint8_t exitg1;
  int32_t exitg2;
  int32_t exitg3;
  uint8_t exitg4;
  At_size_idx_0 = A_size[0];
  jrow = A_size[1];
  loop_ub_tmp = A_size[0] * A_size[1];
  loop_ub_tmp_0 = loop_ub_tmp - 1;
  if (0 <= loop_ub_tmp_0) {
    memcpy(&At_data[0], &A_data[0], (loop_ub_tmp_0 + 1) * sizeof(float));
  }

  *info = 0;
  anrm = 0.0;
  jcol = 0;
  exitg1 = 0;
  while ((!exitg1) && (jcol <= loop_ub_tmp - 1)) {
    absxk = rt_hypotd_snf(A_data[jcol].re, A_data[jcol].im);
    if (rtIsNaN(absxk)) {
      anrm = (rtNaN);
      exitg1 = 1;
    } else {
      if (absxk > anrm) {
        anrm = absxk;
      }

      jcol++;
    }
  }

  if (rtIsInf(anrm) || rtIsNaN(anrm)) {
    *alpha1_size = A_size[0];
    jcol = A_size[0];
    for (i = 0; i < jcol; i++) {
      alpha1_data[i].re = (rtNaN);
      alpha1_data[i].im = 0.0;
    }

    *beta1_size = A_size[0];
    jcol = A_size[0];
    for (i = 0; i < jcol; i++) {
      beta1_data[i].re = (rtNaN);
      beta1_data[i].im = 0.0;
    }
  } else {
    ilascl = 0;
    anrmto = anrm;
    if ((anrm > 0.0) && (anrm < 6.7178761075670888E-139)) {
      anrmto = 6.7178761075670888E-139;
      ilascl = 1;
    } else {
      if (anrm > 1.4885657073574029E+138) {
        anrmto = 1.4885657073574029E+138;
        ilascl = 1;
      }
    }

    if (ilascl) {
      At_size_idx_0 = A_size[0];
      jrow = A_size[1];
      if (0 <= loop_ub_tmp_0) {
        memcpy(&At_data[0], &A_data[0], (loop_ub_tmp_0 + 1) * sizeof(float));
      }

      absxk = anrm;
      ctoc = anrmto;
      notdone = 1;
      while (notdone) {
        atmp_im = absxk * 2.0041683600089728E-292;
        cto1 = ctoc / 4.9896007738368E+291;
        if ((atmp_im > ctoc) && (ctoc != 0.0)) {
          mul = 2.0041683600089728E-292;
          absxk = atmp_im;
        } else if (cto1 > absxk) {
          mul = 4.9896007738368E+291;
          ctoc = cto1;
        } else {
          mul = ctoc / absxk;
          notdone = 0;
        }

        jcol = At_size_idx_0 * jrow - 1;
        for (i = 0; i <= jcol; i++) {
          s.re = mul * At_data[i].re;
          s.im = mul * At_data[i].im;
          At_data[i] = s;
        }
      }
    }

    loop_ub_tmp = At_size_idx_0 * jrow - 1;
    if (0 <= loop_ub_tmp) {
      memcpy(&c_A_data[0], &At_data[0], (loop_ub_tmp + 1) * sizeof(float));
    }

    loop_ub_tmp = 1;
    loop_ub_tmp_0 = At_size_idx_0;
    if (At_size_idx_0 <= 1) {
      loop_ub_tmp_0 = 1;
    } else {
      do {
        exitg3 = 0;
        i = -1;
        j = 0;
        notdone = 0;
        ii = loop_ub_tmp_0;
        exitg1 = 0;
        while ((!exitg1) && (ii > 0)) {
          absxk = 0.0;
          i = ii - 1;
          j = loop_ub_tmp_0;
          jcol = 0;
          exitg4 = 0;
          while ((!exitg4) && (jcol <= loop_ub_tmp_0 - 1)) {
            if ((c_A_data[(At_size_idx_0 * jcol + ii) - 1].re != 0.0) ||
                (c_A_data[(At_size_idx_0 * jcol + ii) - 1].im != 0.0) || (jcol +
                 1 == ii)) {
              if (absxk == 0.0) {
                j = jcol + 1;
                absxk = 1.0;
                jcol++;
              } else {
                absxk = 2.0;
                exitg4 = 1;
              }
            } else {
              jcol++;
            }
          }

          if (absxk < 2.0) {
            notdone = 1;
            exitg1 = 1;
          } else {
            ii--;
          }
        }

        if (!notdone) {
          exitg3 = 2;
        } else {
          jcol = At_size_idx_0 * jrow - 1;
          if (0 <= jcol) {
            memcpy(&c_A_data_0[0], &c_A_data[0], (jcol + 1) * sizeof(float));
          }

          if (i + 1 != loop_ub_tmp_0) {
            for (jcol = 1; jcol <= At_size_idx_0; jcol++) {
              ctoc = c_A_data_0[(jcol - 1) * At_size_idx_0 + i].re;
              atmp_im = c_A_data_0[(jcol - 1) * At_size_idx_0 + i].im;
              c_A_data_tmp = (jcol - 1) * At_size_idx_0;
              ii = (c_A_data_tmp + loop_ub_tmp_0) - 1;
              c_A_data_0[i + c_A_data_tmp] = c_A_data_0[ii];
              c_A_data_0[ii].re = ctoc;
              c_A_data_0[ii].im = atmp_im;
            }
          }

          if (j != loop_ub_tmp_0) {
            for (i = 0; i < loop_ub_tmp_0; i++) {
              ii = (j - 1) * At_size_idx_0 + i;
              ctoc = c_A_data_0[ii].re;
              atmp_im = c_A_data_0[ii].im;
              c_A_data_tmp = (loop_ub_tmp_0 - 1) * At_size_idx_0 + i;
              c_A_data_0[ii] = c_A_data_0[c_A_data_tmp];
              c_A_data_0[c_A_data_tmp].re = ctoc;
              c_A_data_0[c_A_data_tmp].im = atmp_im;
            }
          }

          jcol = At_size_idx_0 * jrow - 1;
          if (0 <= jcol) {
            memcpy(&c_A_data[0], &c_A_data_0[0], (jcol + 1) * sizeof(float));
          }

          loop_ub_tmp_0--;
          if (loop_ub_tmp_0 == 1) {
            exitg3 = 1;
          }
        }
      } while (exitg3 == 0);

      if (exitg3 == 1) {
      } else {
        do {
          exitg2 = 0;
          i = 0;
          j = -1;
          notdone = 0;
          jcol = loop_ub_tmp;
          exitg1 = 0;
          while ((!exitg1) && (jcol <= loop_ub_tmp_0)) {
            absxk = 0.0;
            i = loop_ub_tmp_0;
            j = jcol - 1;
            ii = loop_ub_tmp;
            exitg4 = 0;
            while ((!exitg4) && (ii <= loop_ub_tmp_0)) {
              if ((c_A_data[((jcol - 1) * At_size_idx_0 + ii) - 1].re != 0.0) ||
                  (c_A_data[((jcol - 1) * At_size_idx_0 + ii) - 1].im != 0.0) ||
                  (ii == jcol)) {
                if (absxk == 0.0) {
                  i = ii;
                  absxk = 1.0;
                  ii++;
                } else {
                  absxk = 2.0;
                  exitg4 = 1;
                }
              } else {
                ii++;
              }
            }

            if (absxk < 2.0) {
              notdone = 1;
              exitg1 = 1;
            } else {
              jcol++;
            }
          }

          if (!notdone) {
            exitg2 = 1;
          } else {
            jcol = At_size_idx_0 * jrow - 1;
            if (0 <= jcol) {
              memcpy(&d_A_data_0[0], &c_A_data[0], (jcol + 1) * sizeof(float));
            }

            if (i != loop_ub_tmp) {
              for (jcol = loop_ub_tmp; jcol <= At_size_idx_0; jcol++) {
                ii = (jcol - 1) * At_size_idx_0;
                c_A_data_tmp = (ii + i) - 1;
                ctoc = d_A_data_0[c_A_data_tmp].re;
                atmp_im = d_A_data_0[((jcol - 1) * At_size_idx_0 + i) - 1].im;
                d_A_data_tmp = (ii + loop_ub_tmp) - 1;
                d_A_data_0[c_A_data_tmp] = d_A_data_0[d_A_data_tmp];
                d_A_data_0[d_A_data_tmp].re = ctoc;
                d_A_data_0[d_A_data_tmp].im = atmp_im;
              }
            }

            if (j + 1 != loop_ub_tmp) {
              for (i = 0; i < loop_ub_tmp_0; i++) {
                ii = At_size_idx_0 * j + i;
                ctoc = d_A_data_0[ii].re;
                atmp_im = d_A_data_0[ii].im;
                d_A_data_tmp = (loop_ub_tmp - 1) * At_size_idx_0 + i;
                d_A_data_0[ii] = d_A_data_0[d_A_data_tmp];
                d_A_data_0[d_A_data_tmp].re = ctoc;
                d_A_data_0[d_A_data_tmp].im = atmp_im;
              }
            }

            jcol = At_size_idx_0 * jrow - 1;
            if (0 <= jcol) {
              memcpy(&c_A_data[0], &d_A_data_0[0], (jcol + 1) * sizeof(float));
            }

            loop_ub_tmp++;
            if (loop_ub_tmp == loop_ub_tmp_0) {
              exitg2 = 1;
            }
          }
        } while (exitg2 == 0);
      }
    }

    d_A_size[0] = At_size_idx_0;
    d_A_size[1] = jrow;
    jcol = At_size_idx_0 * jrow - 1;
    if (0 <= jcol) {
      memcpy(&d_A_data[0], &c_A_data[0], (jcol + 1) * sizeof(float));
    }

    if ((At_size_idx_0 > 1) && (loop_ub_tmp_0 >= loop_ub_tmp + 2)) {
      for (jcol = loop_ub_tmp - 1; jcol + 1 < loop_ub_tmp_0 - 1; jcol++) {
        for (jrow = loop_ub_tmp_0 - 2; jrow + 2 > jcol + 2; jrow--) {
          cyperus_lowpass_module_xzlartg(d_A_data[jrow + At_size_idx_0 * jcol],
            d_A_data[(jrow + At_size_idx_0 * jcol) + 1], &absxk, &s,
            &d_A_data[jrow + At_size_idx_0 * jcol]);
          i = (jrow + At_size_idx_0 * jcol) + 1;
          d_A_data[i].re = 0.0;
          d_A_data[i].im = 0.0;
          for (j = jcol + 2; j <= At_size_idx_0; j++) {
            ii = (j - 1) * At_size_idx_0 + jrow;
            c_A_data_tmp = ii + 1;
            ctoc = (d_A_data[c_A_data_tmp].re * s.re - d_A_data[((j - 1) *
                     At_size_idx_0 + jrow) + 1].im * s.im) + d_A_data[ii].re *
              absxk;
            atmp_im = (d_A_data[((j - 1) * At_size_idx_0 + jrow) + 1].im * s.re
                       + d_A_data[((j - 1) * At_size_idx_0 + jrow) + 1].re *
                       s.im) + d_A_data[(j - 1) * At_size_idx_0 + jrow].im *
              absxk;
            cto1 = d_A_data[(j - 1) * At_size_idx_0 + jrow].re;
            d_A_data[c_A_data_tmp].re = d_A_data[((j - 1) * At_size_idx_0 + jrow)
              + 1].re * absxk - (d_A_data[(j - 1) * At_size_idx_0 + jrow].re *
                                 s.re + d_A_data[(j - 1) * At_size_idx_0 + jrow]
                                 .im * s.im);
            d_A_data[c_A_data_tmp].im = d_A_data[c_A_data_tmp].im * absxk -
              (d_A_data[ii].im * s.re - s.im * cto1);
            i = jrow + At_size_idx_0 * (j - 1);
            d_A_data[i].re = ctoc;
            d_A_data[i].im = atmp_im;
          }

          s.re = -s.re;
          s.im = -s.im;
          for (i = 1; i <= loop_ub_tmp_0; i++) {
            ii = (jrow + 1) * At_size_idx_0;
            c_A_data_tmp = At_size_idx_0 * jrow;
            j = (ii + i) - 1;
            d_A_data_tmp = (c_A_data_tmp + i) - 1;
            ctoc = d_A_data[j].re * absxk + (d_A_data[d_A_data_tmp].re * s.re -
              d_A_data[(At_size_idx_0 * jrow + i) - 1].im * s.im);
            atmp_im = d_A_data[(ii + i) - 1].im * absxk + (d_A_data
              [(c_A_data_tmp + i) - 1].im * s.re + d_A_data[(At_size_idx_0 *
              jrow + i) - 1].re * s.im);
            cto1 = d_A_data[j].re;
            d_A_data[d_A_data_tmp].re = d_A_data[d_A_data_tmp].re * absxk -
              (d_A_data[j].re * s.re + d_A_data[((jrow + 1) * At_size_idx_0 + i)
               - 1].im * s.im);
            d_A_data[d_A_data_tmp].im = d_A_data[d_A_data_tmp].im * absxk -
              (d_A_data[j].im * s.re - s.im * cto1);
            d_A_data[j].re = ctoc;
            d_A_data[j].im = atmp_im;
          }
        }
      }
    }

    cyperus_lowpass_module_xzhgeqz(d_A_data, d_A_size, loop_ub_tmp,
      loop_ub_tmp_0, info, alpha1_data, alpha1_size, beta1_data, beta1_size);
    if ((*info == 0) && ilascl) {
      notdone = 1;
      while (notdone) {
        atmp_im = anrmto * 2.0041683600089728E-292;
        cto1 = anrm / 4.9896007738368E+291;
        if ((atmp_im > anrm) && (anrm != 0.0)) {
          mul = 2.0041683600089728E-292;
          anrmto = atmp_im;
        } else if (cto1 > anrmto) {
          mul = 4.9896007738368E+291;
          anrm = cto1;
        } else {
          mul = anrm / anrmto;
          notdone = 0;
        }

        jcol = *alpha1_size;
        for (i = 0; i < jcol; i++) {
          alpha1_data_0.re = mul * alpha1_data[i].re;
          alpha1_data_0.im = mul * alpha1_data[i].im;
          alpha1_data[i] = alpha1_data_0;
        }
      }
    }
  }
}

static float cyperus_lowpass_module_xnrm2(int32_t n, const complex_float32_t x_data[],
  int32_t ix0)
{
  float y;
  float scale;
  float absxk;
  float t;
  int32_t k;
  y = 0.0;
  if (n >= 1) {
    if (n == 1) {
      y = rt_hypotd_snf(x_data[ix0 - 1].re, x_data[ix0 - 1].im);
    } else {
      scale = 3.3121686421112381E-170;
      for (k = ix0; k <= ix0 + 1; k++) {
        absxk = fabs(x_data[k - 1].re);
        if (absxk > scale) {
          t = scale / absxk;
          y = y * t * t + 1.0;
          scale = absxk;
        } else {
          t = absxk / scale;
          y += t * t;
        }

        absxk = fabs(x_data[k - 1].im);
        if (absxk > scale) {
          t = scale / absxk;
          y = y * t * t + 1.0;
          scale = absxk;
        } else {
          t = absxk / scale;
          y += t * t;
        }
      }

      y = scale * sqrt(y);
    }
  }

  return y;
}

static float cyperus_lowpass_module_xdlapy3(float x1, float x2, float x3)
{
  float y;
  float a;
  float b;
  float c;
  a = fabs(x1);
  b = fabs(x2);
  c = fabs(x3);
  if ((a > b) || rtIsNaN(b)) {
    y = a;
  } else {
    y = b;
  }

  if (c > y) {
    y = c;
  }

  if ((y > 0.0) && (!rtIsInf(y))) {
    a /= y;
    b /= y;
    c /= y;
    y *= sqrt((a * a + c * c) + b * b);
  } else {
    y = (a + b) + c;
  }

  return y;
}

static complex_float32_t cyperus_lowpass_module_recip(const complex_float32_t y)
{
  complex_float32_t z;
  float br;
  float brm;
  float bim;
  brm = fabs(y.re);
  bim = fabs(y.im);
  if (y.im == 0.0) {
    z.re = 1.0 / y.re;
    z.im = 0.0;
  } else if (y.re == 0.0) {
    z.re = 0.0;
    z.im = -1.0 / y.im;
  } else if (brm > bim) {
    brm = y.im / y.re;
    bim = brm * y.im + y.re;
    z.re = 1.0 / bim;
    z.im = -brm / bim;
  } else if (brm == bim) {
    bim = 0.5;
    if (y.re < 0.0) {
      bim = -0.5;
    }

    br = 0.5;
    if (y.im < 0.0) {
      br = -0.5;
    }

    z.re = bim / brm;
    z.im = -br / brm;
  } else {
    brm = y.re / y.im;
    bim = brm * y.re + y.im;
    z.re = brm / bim;
    z.im = -1.0 / bim;
  }

  return z;
}

static void cyperus_lowpass_module_xgehrd(const complex_float32_t a_data[], const int32_t
  a_size[2], complex_float32_t b_a_data[], int32_t b_a_size[2])
{
  complex_float32_t c_a_data[16];
  complex_float32_t tau_data[3];
  int32_t n;
  complex_float32_t work_data[4];
  int32_t in;
  int32_t ia0;
  complex_float32_t b_alpha1;
  complex_float32_t c_a_data_0[16];
  complex_float32_t d_a_data[16];
  complex_float32_t e_a_data[16];
  int32_t b_i;
  int32_t c;
  float xnorm;
  int32_t knt;
  int32_t lastc;
  int32_t rowleft;
  int32_t rowright;
  int32_t iy;
  int32_t iac;
  int32_t jA;
  int32_t jy;
  int32_t c_a_size[2];
  int32_t c_a_size_0[2];
  int32_t d_a_size_idx_1;
  int32_t d_idx_0;
  complex_float32_t b_alpha1_0;
  complex_float32_t c_a_data_1;
  float alpha1_re;
  float alpha1_im;
  float c_im;
  float c_a_data_im;
  int32_t im1n_tmp;
  int32_t c_tmp;
  int32_t ia0_tmp;
  int32_t exitg1;
  uint8_t exitg2;
  c_a_size[0] = a_size[0];
  c_a_size[1] = a_size[1];
  rowleft = a_size[0] * a_size[1] - 1;
  if (0 <= rowleft) {
    memcpy(&c_a_data[0], &a_data[0], (rowleft + 1) * sizeof(float));
  }

  n = a_size[0];
  d_idx_0 = a_size[0];
  if (0 <= d_idx_0 - 1) {
    memset(&work_data[0], 0, d_idx_0 * sizeof(float));
  }

  d_idx_0 = a_size[0] - 2;
  for (b_i = 0; b_i <= d_idx_0; b_i++) {
    im1n_tmp = b_i * n;
    in = (b_i + 1) * n;
    ia0 = b_i + 3;
    if (ia0 >= n) {
      ia0 = n;
    }

    ia0 += im1n_tmp;
    c_tmp = n - b_i;
    c = c_tmp - 3;
    c_a_size_0[0] = c_a_size[0];
    c_a_size_0[1] = c_a_size[1];
    rowleft = c_a_size[0] * c_a_size[1] - 1;
    if (0 <= rowleft) {
      memcpy(&c_a_data_0[0], &c_a_data[0], (rowleft + 1) * sizeof(float));
    }

    lastc = (c_a_size[0] * b_i + b_i) + 1;
    b_alpha1 = c_a_data[lastc];
    alpha1_re = 0.0;
    alpha1_im = 0.0;
    if (c + 2 > 0) {
      xnorm = cyperus_lowpass_module_xnrm2(c + 1, c_a_data, ia0);
      if ((xnorm != 0.0) || (c_a_data[(c_a_size[0] * b_i + b_i) + 1].im != 0.0))
      {
        xnorm = cyperus_lowpass_module_xdlapy3(c_a_data[(c_a_size[0] * b_i + b_i)
          + 1].re, c_a_data[(c_a_size[0] * b_i + b_i) + 1].im, xnorm);
        if (c_a_data[(c_a_size[0] * b_i + b_i) + 1].re >= 0.0) {
          xnorm = -xnorm;
        }

        if (fabs(xnorm) < 1.0020841800044864E-292) {
          knt = -1;
          rowleft = ia0 + c;
          do {
            knt++;
            for (lastc = ia0; lastc <= rowleft; lastc++) {
              c_im = c_a_data_0[lastc - 1].re;
              c_a_data_im = c_a_data_0[lastc - 1].im;
              c_a_data_0[lastc - 1].re = 9.9792015476736E+291 * c_im - 0.0 *
                c_a_data_im;
              c_a_data_0[lastc - 1].im = 9.9792015476736E+291 * c_a_data_im +
                0.0 * c_im;
            }

            xnorm *= 9.9792015476736E+291;
            b_alpha1.re *= 9.9792015476736E+291;
            b_alpha1.im *= 9.9792015476736E+291;
          } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

          xnorm = cyperus_lowpass_module_xdlapy3(b_alpha1.re, b_alpha1.im,
            cyperus_lowpass_module_xnrm2(c + 1, c_a_data_0, ia0));
          if (b_alpha1.re >= 0.0) {
            xnorm = -xnorm;
          }

          c_im = xnorm - b_alpha1.re;
          if (0.0 - b_alpha1.im == 0.0) {
            alpha1_re = c_im / xnorm;
          } else if (c_im == 0.0) {
            alpha1_re = 0.0;
            alpha1_im = (0.0 - b_alpha1.im) / xnorm;
          } else {
            alpha1_re = c_im / xnorm;
            alpha1_im = (0.0 - b_alpha1.im) / xnorm;
          }

          b_alpha1_0.re = b_alpha1.re - xnorm;
          b_alpha1_0.im = b_alpha1.im;
          b_alpha1 = cyperus_lowpass_module_recip(b_alpha1_0);
          for (lastc = ia0; lastc <= rowleft; lastc++) {
            c_im = c_a_data_0[lastc - 1].re;
            c_a_data_im = c_a_data_0[lastc - 1].im;
            c_a_data_0[lastc - 1].re = b_alpha1.re * c_im - b_alpha1.im *
              c_a_data_im;
            c_a_data_0[lastc - 1].im = b_alpha1.re * c_a_data_im + b_alpha1.im *
              c_im;
          }

          for (lastc = 0; lastc <= knt; lastc++) {
            xnorm *= 1.0020841800044864E-292;
          }

          b_alpha1.re = xnorm;
          b_alpha1.im = 0.0;
        } else {
          c_im = xnorm - c_a_data[lastc].re;
          if (0.0 - c_a_data[lastc].im == 0.0) {
            alpha1_re = c_im / xnorm;
          } else if (c_im == 0.0) {
            alpha1_im = (0.0 - c_a_data[lastc].im) / xnorm;
          } else {
            alpha1_re = c_im / xnorm;
            alpha1_im = (0.0 - c_a_data[lastc].im) / xnorm;
          }

          c_a_data_1.re = c_a_data[(c_a_size[0] * b_i + b_i) + 1].re - xnorm;
          c_a_data_1.im = c_a_data[(c_a_size[0] * b_i + b_i) + 1].im;
          b_alpha1 = cyperus_lowpass_module_recip(c_a_data_1);
          c_a_size_0[0] = c_a_size[0];
          c_a_size_0[1] = c_a_size[1];
          rowleft = c_a_size[0] * c_a_size[1] - 1;
          if (0 <= rowleft) {
            memcpy(&c_a_data_0[0], &c_a_data[0], (rowleft + 1) * sizeof(float));
          }

          rowleft = ia0 + c;
          for (lastc = ia0; lastc <= rowleft; lastc++) {
            c_im = c_a_data_0[lastc - 1].re;
            c_a_data_im = c_a_data_0[lastc - 1].im;
            c_a_data_0[lastc - 1].re = b_alpha1.re * c_im - b_alpha1.im *
              c_a_data_im;
            c_a_data_0[lastc - 1].im = b_alpha1.re * c_a_data_im + b_alpha1.im *
              c_im;
          }

          b_alpha1.re = xnorm;
          b_alpha1.im = 0.0;
        }
      }
    }

    c = c_a_size_0[0] * c_a_size_0[1] - 1;
    if (0 <= c) {
      memcpy(&c_a_data[0], &c_a_data_0[0], (c + 1) * sizeof(float));
    }

    tau_data[b_i].re = alpha1_re;
    tau_data[b_i].im = alpha1_im;
    ia0 = (b_i + c_a_size_0[0] * b_i) + 1;
    c_a_data[ia0].re = 1.0;
    c_a_data[ia0].im = 0.0;
    ia0_tmp = c_tmp - 2;
    ia0 = ia0_tmp;
    im1n_tmp = (b_i + im1n_tmp) + 2;
    jy = im1n_tmp - 1;
    rowright = c_a_size_0[0];
    d_a_size_idx_1 = c_a_size_0[1];
    if (0 <= c) {
      memcpy(&d_a_data[0], &c_a_data[0], (c + 1) * sizeof(float));
    }

    if ((tau_data[b_i].re != 0.0) || (tau_data[b_i].im != 0.0)) {
      lastc = jy + ia0_tmp;
      while ((ia0 + 1 > 0) && ((c_a_data[lastc].re == 0.0) && (c_a_data[lastc].
               im == 0.0))) {
        ia0--;
        lastc--;
      }

      lastc = n;
      exitg2 = 0;
      while ((!exitg2) && (lastc > 0)) {
        rowleft = in + lastc;
        rowright = ia0 * n + rowleft;
        do {
          exitg1 = 0;
          if (((n > 0) && (rowleft <= rowright)) || ((n < 0) && (rowleft >=
                rowright))) {
            if ((c_a_data[rowleft - 1].re != 0.0) || (c_a_data[rowleft - 1].im
                 != 0.0)) {
              exitg1 = 1;
            } else {
              rowleft += n;
            }
          } else {
            lastc--;
            exitg1 = 2;
          }
        } while (exitg1 == 0);

        if (exitg1 == 1) {
          exitg2 = 1;
        }
      }

      rowright = c_a_size_0[0];
      d_a_size_idx_1 = c_a_size_0[1];
      rowleft = c_a_size_0[0] * c_a_size_0[1] - 1;
      if (0 <= rowleft) {
        memcpy(&d_a_data[0], &c_a_data[0], (rowleft + 1) * sizeof(float));
      }
    } else {
      ia0 = -1;
      lastc = 0;
    }

    if (ia0 + 1 > 0) {
      if (lastc != 0) {
        if (0 <= lastc - 1) {
          memset(&work_data[0], 0, lastc * sizeof(float));
        }

        knt = jy;
        rowleft = n * ia0 + in;
        for (iac = in + 1; n < 0 ? iac >= rowleft + 1 : iac <= rowleft + 1; iac +=
             n) {
          xnorm = d_a_data[knt].re - 0.0 * d_a_data[knt].im;
          c_im = 0.0 * d_a_data[knt].re + d_a_data[knt].im;
          iy = 0;
          c = iac + lastc;
          for (jA = iac; jA < c; jA++) {
            work_data[iy].re += d_a_data[jA - 1].re * xnorm - d_a_data[jA - 1].
              im * c_im;
            work_data[iy].im += d_a_data[jA - 1].re * c_im + d_a_data[jA - 1].im
              * xnorm;
            iy++;
          }

          knt++;
        }
      }

      alpha1_re = -tau_data[b_i].re;
      alpha1_im = -tau_data[b_i].im;
      if ((!(-tau_data[b_i].re == 0.0)) || (!(-tau_data[b_i].im == 0.0))) {
        jA = in;
        for (iac = 0; iac <= ia0; iac++) {
          if ((d_a_data[jy].re != 0.0) || (d_a_data[jy].im != 0.0)) {
            xnorm = d_a_data[jy].re * alpha1_re + d_a_data[jy].im * alpha1_im;
            c_im = d_a_data[jy].re * alpha1_im - d_a_data[jy].im * alpha1_re;
            knt = 0;
            c = lastc + jA;
            for (iy = jA + 1; iy <= c; iy++) {
              d_a_data[iy - 1].re += work_data[knt].re * xnorm - work_data[knt].
                im * c_im;
              d_a_data[iy - 1].im += work_data[knt].re * c_im + work_data[knt].
                im * xnorm;
              knt++;
            }
          }

          jy++;
          jA += n;
        }
      }
    }

    ia0 = c_tmp - 1;
    in = (b_i + in) + 2;
    alpha1_re = tau_data[b_i].re;
    alpha1_im = -tau_data[b_i].im;
    c = rowright * d_a_size_idx_1 - 1;
    if (0 <= c) {
      memcpy(&e_a_data[0], &d_a_data[0], (c + 1) * sizeof(float));
    }

    if ((tau_data[b_i].re != 0.0) || (-tau_data[b_i].im != 0.0)) {
      lastc = (im1n_tmp + ia0) - 2;
      while ((ia0 > 0) && ((d_a_data[lastc].re == 0.0) && (d_a_data[lastc].im ==
               0.0))) {
        ia0--;
        lastc--;
      }

      lastc = ia0_tmp;
      exitg2 = 0;
      while ((!exitg2) && (lastc + 1 > 0)) {
        rowleft = lastc * n + in;
        jA = rowleft;
        do {
          exitg1 = 0;
          if (jA <= (rowleft + ia0) - 1) {
            if ((d_a_data[jA - 1].re != 0.0) || (d_a_data[jA - 1].im != 0.0)) {
              exitg1 = 1;
            } else {
              jA++;
            }
          } else {
            lastc--;
            exitg1 = 2;
          }
        } while (exitg1 == 0);

        if (exitg1 == 1) {
          exitg2 = 1;
        }
      }

      if (0 <= c) {
        memcpy(&e_a_data[0], &d_a_data[0], (c + 1) * sizeof(float));
      }
    } else {
      ia0 = 0;
      lastc = -1;
    }

    if (ia0 > 0) {
      if (lastc + 1 != 0) {
        if (0 <= lastc) {
          memset(&work_data[0], 0, (lastc + 1) * sizeof(float));
        }

        iy = 0;
        rowleft = n * lastc + in;
        for (iac = in; n < 0 ? iac >= rowleft : iac <= rowleft; iac += n) {
          knt = im1n_tmp - 1;
          xnorm = 0.0;
          c_im = 0.0;
          c = iac + ia0;
          for (jA = iac; jA < c; jA++) {
            xnorm += e_a_data[jA - 1].re * e_a_data[knt].re + e_a_data[jA - 1].
              im * e_a_data[knt].im;
            c_im += e_a_data[jA - 1].re * e_a_data[knt].im - e_a_data[jA - 1].im
              * e_a_data[knt].re;
            knt++;
          }

          work_data[iy].re += xnorm - 0.0 * c_im;
          work_data[iy].im += 0.0 * xnorm + c_im;
          iy++;
        }
      }

      if ((!(-tau_data[b_i].re == 0.0)) || (!(tau_data[b_i].im == 0.0))) {
        jA = in - 1;
        jy = 0;
        for (iac = 0; iac <= lastc; iac++) {
          if ((work_data[jy].re != 0.0) || (work_data[jy].im != 0.0)) {
            xnorm = work_data[jy].re * -alpha1_re + work_data[jy].im *
              -alpha1_im;
            c_im = work_data[jy].re * -alpha1_im - work_data[jy].im * -alpha1_re;
            knt = im1n_tmp - 1;
            in = ia0 + jA;
            for (iy = jA + 1; iy <= in; iy++) {
              c_a_data_im = e_a_data[knt].re * c_im + e_a_data[knt].im * xnorm;
              e_a_data[iy - 1].re += e_a_data[knt].re * xnorm - e_a_data[knt].im
                * c_im;
              e_a_data[iy - 1].im += c_a_data_im;
              knt++;
            }
          }

          jy++;
          jA += n;
        }
      }
    }

    c_a_size[0] = rowright;
    c_a_size[1] = d_a_size_idx_1;
    rowleft = rowright * d_a_size_idx_1 - 1;
    if (0 <= rowleft) {
      memcpy(&c_a_data[0], &e_a_data[0], (rowleft + 1) * sizeof(float));
    }

    c_a_data[(b_i + rowright * b_i) + 1] = b_alpha1;
  }

  b_a_size[0] = c_a_size[0];
  b_a_size[1] = c_a_size[1];
  c = c_a_size[0] * c_a_size[1] - 1;
  if (0 <= c) {
    memcpy(&b_a_data[0], &c_a_data[0], (c + 1) * sizeof(float));
  }
}

static void cyperus_lowpass_module_xzlarfg(const complex_float32_t alpha1, const complex_float32_t x,
  complex_float32_t *b_alpha1, complex_float32_t *b_x, complex_float32_t *tau)
{
  float xnorm;
  int32_t knt;
  int32_t k;
  complex_float32_t alpha1_0;
  float ar;
  *b_x = x;
  *b_alpha1 = alpha1;
  tau->re = 0.0;
  tau->im = 0.0;
  xnorm = rt_hypotd_snf(x.re, x.im);
  if ((xnorm != 0.0) || (alpha1.im != 0.0)) {
    xnorm = cyperus_lowpass_module_xdlapy3(alpha1.re, alpha1.im, xnorm);
    if (alpha1.re >= 0.0) {
      xnorm = -xnorm;
    }

    if (fabs(xnorm) < 1.0020841800044864E-292) {
      knt = -1;
      do {
        knt++;
        b_x->re *= 9.9792015476736E+291;
        b_x->im *= 9.9792015476736E+291;
        xnorm *= 9.9792015476736E+291;
        b_alpha1->re *= 9.9792015476736E+291;
        b_alpha1->im *= 9.9792015476736E+291;
      } while (!(fabs(xnorm) >= 1.0020841800044864E-292));

      xnorm = cyperus_lowpass_module_xdlapy3(b_alpha1->re, b_alpha1->im,
        rt_hypotd_snf(b_x->re, b_x->im));
      if (b_alpha1->re >= 0.0) {
        xnorm = -xnorm;
      }

      ar = xnorm - b_alpha1->re;
      if (0.0 - b_alpha1->im == 0.0) {
        tau->re = ar / xnorm;
        tau->im = 0.0;
      } else if (ar == 0.0) {
        tau->re = 0.0;
        tau->im = (0.0 - b_alpha1->im) / xnorm;
      } else {
        tau->re = ar / xnorm;
        tau->im = (0.0 - b_alpha1->im) / xnorm;
      }

      alpha1_0.re = b_alpha1->re - xnorm;
      alpha1_0.im = b_alpha1->im;
      *b_alpha1 = cyperus_lowpass_module_recip(alpha1_0);
      ar = b_x->re;
      b_x->re = b_alpha1->re * b_x->re - b_alpha1->im * b_x->im;
      b_x->im = b_alpha1->re * b_x->im + b_alpha1->im * ar;
      for (k = 0; k <= knt; k++) {
        xnorm *= 1.0020841800044864E-292;
      }

      b_alpha1->re = xnorm;
      b_alpha1->im = 0.0;
    } else {
      ar = xnorm - alpha1.re;
      if (0.0 - alpha1.im == 0.0) {
        tau->re = ar / xnorm;
        tau->im = 0.0;
      } else if (ar == 0.0) {
        tau->re = 0.0;
        tau->im = (0.0 - alpha1.im) / xnorm;
      } else {
        tau->re = ar / xnorm;
        tau->im = (0.0 - alpha1.im) / xnorm;
      }

      alpha1_0.re = alpha1.re - xnorm;
      alpha1_0.im = alpha1.im;
      alpha1_0 = cyperus_lowpass_module_recip(alpha1_0);
      b_x->re = alpha1_0.re * x.re - alpha1_0.im * x.im;
      b_x->im = alpha1_0.re * x.im + alpha1_0.im * x.re;
      b_alpha1->re = xnorm;
      b_alpha1->im = 0.0;
    }
  }
}

static void cyperus_lowpass_modu_eml_zlahqr(const complex_float32_t h_data[], const
  int32_t h_size[2], complex_float32_t b_h_data[], int32_t b_h_size[2], int32_t *info)
{
  int32_t n;
  int32_t ldh;
  int32_t i;
  complex_float32_t sc;
  float SMLNUM;
  int32_t L;
  uint8_t goto140;
  float tst;
  float htmp1;
  float ab;
  float ba;
  float aa;
  complex_float32_t t;
  complex_float32_t x2;
  uint8_t goto70;
  int32_t m;
  int32_t its;
  int32_t b_k;
  int32_t c;
  int32_t b;
  int32_t k;
  complex_float32_t v_idx_1;
  complex_float32_t v_idx_0;
  complex_float32_t tmp;
  complex_float32_t tmp_0;
  complex_float32_t x2_0;
  int32_t b_h_size_0;
  int32_t br_tmp;
  int32_t c_tmp;
  int32_t sc_tmp;
  int32_t x2_tmp;
  uint8_t exitg1;
  uint8_t exitg2;
  uint8_t exitg3;
  b_h_size[0] = h_size[0];
  b_h_size[1] = h_size[1];
  n = h_size[0] * h_size[1] - 1;
  if (0 <= n) {
    memcpy(&b_h_data[0], &h_data[0], (n + 1) * sizeof(float));
  }

  n = h_size[0];
  ldh = h_size[0];
  *info = 0;
  if (1 != h_size[0]) {
    if (0 <= h_size[0] - 4) {
      b_h_data[2].re = 0.0;
      b_h_data[2].im = 0.0;
      b_h_data[3].re = 0.0;
      b_h_data[3].im = 0.0;
    }

    if (1 <= h_size[0] - 2) {
      b = (h_size[0] + h_size[0] * (h_size[0] - 3)) - 1;
      b_h_data[b].re = 0.0;
      b_h_data[b].im = 0.0;
    }

    b_h_size_0 = h_size[0];
    for (i = 2; i <= n; i++) {
      if (b_h_data[((i - 2) * b_h_size_0 + i) - 1].im != 0.0) {
        SMLNUM = b_h_data[((i - 2) * b_h_size_0 + i) - 1].re;
        ab = b_h_data[((i - 2) * b_h_size_0 + i) - 1].im;
        br_tmp = ((i - 2) * b_h_size_0 + i) - 1;
        tst = fabs(b_h_data[br_tmp].re) + fabs(b_h_data[((i - 2) * b_h_size_0 +
          i) - 1].im);
        if (ab == 0.0) {
          sc.re = SMLNUM / tst;
          sc.im = 0.0;
        } else if (SMLNUM == 0.0) {
          sc.re = 0.0;
          sc.im = ab / tst;
        } else {
          sc.re = SMLNUM / tst;
          sc.im = ab / tst;
        }

        tst = rt_hypotd_snf(sc.re, sc.im);
        if (-sc.im == 0.0) {
          sc.re /= tst;
          sc.im = 0.0;
        } else if (sc.re == 0.0) {
          sc.re = 0.0;
          sc.im = -sc.im / tst;
        } else {
          sc.re /= tst;
          sc.im = -sc.im / tst;
        }

        b_h_data[br_tmp].re = rt_hypotd_snf(b_h_data[((i - 2) * b_h_size_0 + i)
          - 1].re, b_h_data[((i - 2) * b_h_size_0 + i) - 1].im);
        b_h_data[br_tmp].im = 0.0;
        c_tmp = (i - 1) * ldh;
        c = c_tmp + i;
        b = (n - i) * ldh + c;
        for (br_tmp = c; ldh < 0 ? br_tmp >= b : br_tmp <= b; br_tmp += ldh) {
          tst = b_h_data[br_tmp - 1].re;
          ab = b_h_data[br_tmp - 1].im;
          b_h_data[br_tmp - 1].re = sc.re * tst - sc.im * ab;
          b_h_data[br_tmp - 1].im = sc.re * ab + sc.im * tst;
        }

        sc.im = -sc.im;
        c = i + 1;
        if (n < c) {
          c = n;
        }

        b = c_tmp + c;
        for (br_tmp = c_tmp + 1; br_tmp <= b; br_tmp++) {
          tst = b_h_data[br_tmp - 1].re;
          ab = b_h_data[br_tmp - 1].im;
          b_h_data[br_tmp - 1].re = sc.re * tst - sc.im * ab;
          b_h_data[br_tmp - 1].im = sc.re * ab + sc.im * tst;
        }
      }
    }

    SMLNUM = (float)h_size[0] / 2.2204460492503131E-16 *
      2.2250738585072014E-308;
    i = h_size[0] - 1;
    exitg1 = 0;
    while ((!exitg1) && (i + 1 >= 1)) {
      L = 1;
      goto140 = 0;
      its = 0;
      exitg2 = 0;
      while ((!exitg2) && (its < 301)) {
        br_tmp = i;
        exitg3 = 0;
        while ((!exitg3) && (br_tmp + 1 > L)) {
          b = (br_tmp - 1) * b_h_size[0] + br_tmp;
          if (fabs(b_h_data[b].re) + fabs(b_h_data[(br_tmp - 1) * b_h_size[0] +
               br_tmp].im) <= SMLNUM) {
            exitg3 = 1;
          } else {
            m = b_h_size[0] * br_tmp + br_tmp;
            b_h_size_0 = b - 1;
            tst = (fabs(b_h_data[b_h_size_0].re) + fabs(b_h_data[((br_tmp - 1) *
                     b_h_size[0] + br_tmp) - 1].im)) + (fabs(b_h_data[m].re) +
              fabs(b_h_data[b_h_size[0] * br_tmp + br_tmp].im));
            if (tst == 0.0) {
              if (br_tmp - 1 >= 1) {
                tst = fabs(b_h_data[((br_tmp - 2) * b_h_size[0] + br_tmp) - 1].
                           re);
              }

              if (br_tmp + 2 <= n) {
                tst += fabs(b_h_data[(b_h_size[0] * br_tmp + br_tmp) + 1].re);
              }
            }

            ab = fabs(b_h_data[b].re);
            if (ab <= 2.2204460492503131E-16 * tst) {
              htmp1 = ab + fabs(b_h_data[(br_tmp - 1) * b_h_size[0] + br_tmp].im);
              tst = fabs(b_h_data[m - 1].re) + fabs(b_h_data[(b_h_size[0] *
                br_tmp + br_tmp) - 1].im);
              if (htmp1 > tst) {
                ab = htmp1;
                ba = tst;
              } else {
                ab = tst;
                ba = htmp1;
              }

              htmp1 = fabs(b_h_data[m].re) + fabs(b_h_data[b_h_size[0] * br_tmp
                + br_tmp].im);
              tst = fabs(b_h_data[b_h_size_0].re - b_h_data[b_h_size[0] * br_tmp
                         + br_tmp].re) + fabs(b_h_data[((br_tmp - 1) * b_h_size
                [0] + br_tmp) - 1].im - b_h_data[b_h_size[0] * br_tmp + br_tmp].
                im);
              if (htmp1 > tst) {
                aa = htmp1;
                htmp1 = tst;
              } else {
                aa = tst;
              }

              tst = aa + ab;
              htmp1 = aa / tst * htmp1 * 2.2204460492503131E-16;
              if ((SMLNUM > htmp1) || rtIsNaN(htmp1)) {
                htmp1 = SMLNUM;
              }

              if (ab / tst * ba <= htmp1) {
                exitg3 = 1;
              } else {
                br_tmp--;
              }
            } else {
              br_tmp--;
            }
          }
        }

        L = br_tmp + 1;
        if (br_tmp + 1 > 1) {
          b_h_data[br_tmp + b_h_size[0] * (br_tmp - 1)].re = 0.0;
          b_h_data[br_tmp + b_h_size[0] * (br_tmp - 1)].im = 0.0;
        }

        if (br_tmp + 1 >= i + 1) {
          goto140 = 1;
          exitg2 = 1;
        } else {
          if (its == 10) {
            t.re = fabs(b_h_data[(b_h_size[0] * br_tmp + br_tmp) + 1].re) * 0.75
              + b_h_data[b_h_size[0] * br_tmp + br_tmp].re;
            t.im = b_h_data[b_h_size[0] * br_tmp + br_tmp].im;
          } else if (its == 20) {
            t.re = fabs(b_h_data[(i - 1) * b_h_size[0] + i].re) * 0.75 +
              b_h_data[b_h_size[0] * i + i].re;
            t.im = b_h_data[b_h_size[0] * i + i].im;
          } else {
            m = b_h_size[0] * i + i;
            t = b_h_data[m];
            tmp = cyperus_lowpass_module_sqrt(b_h_data[m - 1]);
            tmp_0 = cyperus_lowpass_module_sqrt(b_h_data[(i - 1) * b_h_size[0] +
              i]);
            sc.re = tmp.re * tmp_0.re - tmp.im * tmp_0.im;
            sc.im = tmp.re * tmp_0.im + tmp.im * tmp_0.re;
            tst = fabs(sc.re) + fabs(sc.im);
            if (tst != 0.0) {
              t.re = (b_h_data[((i - 1) * b_h_size[0] + i) - 1].re - b_h_data[m]
                      .re) * 0.5;
              t.im = (b_h_data[((i - 1) * b_h_size[0] + i) - 1].im -
                      b_h_data[b_h_size[0] * i + i].im) * 0.5;
              ba = fabs(t.re) + fabs(t.im);
              if ((!(tst > ba)) && (!rtIsNaN(ba))) {
                tst = ba;
              }

              if (t.im == 0.0) {
                x2.re = t.re / tst;
                x2.im = 0.0;
              } else if (t.re == 0.0) {
                x2.re = 0.0;
                x2.im = t.im / tst;
              } else {
                x2.re = t.re / tst;
                x2.im = t.im / tst;
              }

              if (sc.im == 0.0) {
                htmp1 = sc.re / tst;
                ab = 0.0;
              } else if (sc.re == 0.0) {
                htmp1 = 0.0;
                ab = sc.im / tst;
              } else {
                htmp1 = sc.re / tst;
                ab = sc.im / tst;
              }

              aa = htmp1;
              htmp1 = htmp1 * htmp1 - ab * ab;
              ab = aa * ab + ab * aa;
              x2_0.re = (x2.re * x2.re - x2.im * x2.im) + htmp1;
              htmp1 = x2.re * x2.im;
              x2_0.im = (htmp1 + htmp1) + ab;
              tmp = cyperus_lowpass_module_sqrt(x2_0);
              htmp1 = tst * tmp.re;
              ab = tst * tmp.im;
              if (ba > 0.0) {
                if (t.im == 0.0) {
                  x2.re = t.re / ba;
                  x2.im = 0.0;
                } else if (t.re == 0.0) {
                  x2.re = 0.0;
                  x2.im = t.im / ba;
                } else {
                  x2.re = t.re / ba;
                  x2.im = t.im / ba;
                }

                if (x2.re * htmp1 + x2.im * ab < 0.0) {
                  htmp1 = -htmp1;
                  ab = -ab;
                }
              }

              tst = t.re + htmp1;
              ba = t.im + ab;
              if (ba == 0.0) {
                if (sc.im == 0.0) {
                  ab = sc.re / tst;
                  tst = 0.0;
                } else if (sc.re == 0.0) {
                  ab = 0.0;
                  tst = sc.im / tst;
                } else {
                  ab = sc.re / tst;
                  tst = sc.im / tst;
                }
              } else if (tst == 0.0) {
                if (sc.re == 0.0) {
                  ab = sc.im / ba;
                  tst = 0.0;
                } else if (sc.im == 0.0) {
                  ab = 0.0;
                  tst = -(sc.re / ba);
                } else {
                  ab = sc.im / ba;
                  tst = -(sc.re / ba);
                }
              } else {
                htmp1 = fabs(tst);
                ab = fabs(ba);
                if (htmp1 > ab) {
                  htmp1 = ba / tst;
                  tst += htmp1 * ba;
                  ab = (htmp1 * sc.im + sc.re) / tst;
                  tst = (sc.im - htmp1 * sc.re) / tst;
                } else if (ab == htmp1) {
                  tst = tst > 0.0 ? 0.5 : -0.5;
                  ba = ba > 0.0 ? 0.5 : -0.5;
                  ab = (sc.re * tst + sc.im * ba) / htmp1;
                  tst = (sc.im * tst - sc.re * ba) / htmp1;
                } else {
                  htmp1 = tst / ba;
                  tst = htmp1 * tst + ba;
                  ab = (htmp1 * sc.re + sc.im) / tst;
                  tst = (htmp1 * sc.im - sc.re) / tst;
                }
              }

              t.re = b_h_data[m].re - (sc.re * ab - sc.im * tst);
              t.im = b_h_data[b_h_size[0] * i + i].im - (sc.re * tst + sc.im *
                ab);
            }
          }

          goto70 = 0;
          m = i;
          exitg3 = 0;
          while ((!exitg3) && (m > br_tmp + 1)) {
            b_h_size_0 = (m - 1) * b_h_size[0] + m;
            sc_tmp = b_h_size_0 - 1;
            sc.re = b_h_data[sc_tmp].re - t.re;
            sc.im = b_h_data[((m - 1) * b_h_size[0] + m) - 1].im - t.im;
            tst = (fabs(sc.re) + fabs(sc.im)) + fabs(b_h_data[b_h_size_0].re);
            if (sc.im == 0.0) {
              sc.re /= tst;
              sc.im = 0.0;
            } else if (sc.re == 0.0) {
              sc.re = 0.0;
              sc.im /= tst;
            } else {
              sc.re /= tst;
              sc.im /= tst;
            }

            htmp1 = b_h_data[b_h_size_0].re / tst;
            v_idx_0 = sc;
            v_idx_1.re = htmp1;
            v_idx_1.im = 0.0;
            if (fabs(b_h_data[((m - 2) * b_h_size[0] + m) - 1].re) * fabs(htmp1)
                <= ((fabs(b_h_data[sc_tmp].re) + fabs(b_h_data[((m - 1) *
                    b_h_size[0] + m) - 1].im)) + (fabs(b_h_data[b_h_size[0] * m
                   + m].re) + fabs(b_h_data[b_h_size[0] * m + m].im))) * (fabs
                 (sc.re) + fabs(sc.im)) * 2.2204460492503131E-16) {
              goto70 = 1;
              exitg3 = 1;
            } else {
              m--;
            }
          }

          if (!goto70) {
            sc.re = b_h_data[b_h_size[0] * br_tmp + br_tmp].re - t.re;
            sc.im = b_h_data[b_h_size[0] * br_tmp + br_tmp].im - t.im;
            b = (b_h_size[0] * br_tmp + br_tmp) + 1;
            tst = (fabs(sc.re) + fabs(sc.im)) + fabs(b_h_data[b].re);
            if (sc.im == 0.0) {
              v_idx_0.re = sc.re / tst;
              v_idx_0.im = 0.0;
            } else if (sc.re == 0.0) {
              v_idx_0.re = 0.0;
              v_idx_0.im = sc.im / tst;
            } else {
              v_idx_0.re = sc.re / tst;
              v_idx_0.im = sc.im / tst;
            }

            v_idx_1.re = b_h_data[b].re / tst;
            v_idx_1.im = 0.0;
          }

          sc_tmp = i + 1;
          b_h_size_0 = b_h_size[0];
          for (b_k = m; b_k < sc_tmp; b_k++) {
            if (b_k > m) {
              c = (b_k - 2) * b_h_size_0 + b_k;
              v_idx_0 = b_h_data[c - 1];
              v_idx_1 = b_h_data[c];
            }

            cyperus_lowpass_module_xzlarfg(v_idx_0, v_idx_1, &x2, &sc, &t);
            v_idx_0 = x2;
            v_idx_1 = sc;
            if (b_k > m) {
              b_h_data[(b_k + b_h_size_0 * (b_k - 2)) - 1] = x2;
              b_h_data[b_k + b_h_size_0 * (b_k - 2)].re = 0.0;
              b_h_data[b_k + b_h_size_0 * (b_k - 2)].im = 0.0;
            }

            tst = t.re * sc.re - t.im * sc.im;
            for (c = b_k; c <= n; c++) {
              c_tmp = (c - 1) * b_h_size_0 + b_k;
              x2_tmp = c_tmp - 1;
              x2.re = (b_h_data[x2_tmp].re * t.re - b_h_data[((c - 1) *
                        b_h_size_0 + b_k) - 1].im * -t.im) + b_h_data[c_tmp].re *
                tst;
              x2.im = (b_h_data[((c - 1) * b_h_size_0 + b_k) - 1].im * t.re +
                       b_h_data[((c - 1) * b_h_size_0 + b_k) - 1].re * -t.im) +
                b_h_data[(c - 1) * b_h_size_0 + b_k].im * tst;
              b_h_data[x2_tmp].re = b_h_data[((c - 1) * b_h_size_0 + b_k) - 1].
                re - x2.re;
              b_h_data[x2_tmp].im -= x2.im;
              b_h_data[c_tmp].re -= x2.re * sc.re - x2.im * sc.im;
              b = (c - 1) * b_h_size_0 + b_k;
              b_h_data[b].im -= x2.re * sc.im + x2.im * sc.re;
            }

            b = b_k + 2;
            c = i + 1;
            if (b < c) {
              c = b;
            }

            c--;
            for (b = 0; b <= c; b++) {
              c_tmp = (b_k - 1) * b_h_size_0 + b;
              x2_tmp = b_h_size_0 * b_k + b;
              x2.re = (b_h_data[c_tmp].re * t.re - b_h_data[(b_k - 1) *
                       b_h_size_0 + b].im * t.im) + b_h_data[x2_tmp].re * tst;
              x2.im = (b_h_data[(b_k - 1) * b_h_size_0 + b].im * t.re +
                       b_h_data[(b_k - 1) * b_h_size_0 + b].re * t.im) +
                b_h_data[b_h_size_0 * b_k + b].im * tst;
              b_h_data[c_tmp].re = b_h_data[(b_k - 1) * b_h_size_0 + b].re -
                x2.re;
              b_h_data[c_tmp].im -= x2.im;
              b_h_data[x2_tmp].re -= x2.re * sc.re - x2.im * -sc.im;
              b_h_data[x2_tmp].im -= x2.re * -sc.im + x2.im * sc.re;
            }

            if ((b_k == m) && (m > br_tmp + 1)) {
              t.re = 1.0 - t.re;
              t.im = 0.0 - t.im;
              tst = rt_hypotd_snf(t.re, t.im);
              if (t.im == 0.0) {
                t.re /= tst;
                t.im = 0.0;
              } else if (t.re == 0.0) {
                t.re = 0.0;
                t.im /= tst;
              } else {
                t.re /= tst;
                t.im /= tst;
              }

              c = (m - 1) * b_h_size_0 + m;
              tst = b_h_data[c].re;
              ab = b_h_data[(m - 1) * b_h_size_0 + m].im;
              b_h_data[c].re = b_h_data[c].re * t.re - ab * -t.im;
              b_h_data[c].im = tst * -t.im + ab * t.re;
              if (m + 2 <= i + 1) {
                c = b_h_size_0 * m + 3;
                tst = b_h_data[c].re;
                ab = b_h_data[b_h_size_0 * m + 3].im;
                b_h_data[c].re = b_h_data[c].re * t.re - ab * t.im;
                b_h_data[c].im = tst * t.im + ab * t.re;
              }

              for (x2_tmp = m; x2_tmp <= i + 1; x2_tmp++) {
                if (m + 1 != x2_tmp) {
                  if (n > x2_tmp) {
                    c = x2_tmp * ldh + x2_tmp;
                    b = ((n - x2_tmp) - 1) * ldh + c;
                    for (k = c; ldh < 0 ? k >= b : k <= b; k += ldh) {
                      tst = b_h_data[k - 1].re;
                      ab = b_h_data[k - 1].im;
                      b_h_data[k - 1].re = t.re * tst - t.im * ab;
                      b_h_data[k - 1].im = t.re * ab + t.im * tst;
                    }
                  }

                  c_tmp = (x2_tmp - 1) * ldh;
                  sc.re = t.re;
                  sc.im = -t.im;
                  b = c_tmp + x2_tmp;
                  for (k = c_tmp + 1; k < b; k++) {
                    tst = b_h_data[k - 1].re;
                    ab = b_h_data[k - 1].im;
                    b_h_data[k - 1].re = sc.re * tst - sc.im * ab;
                    b_h_data[k - 1].im = sc.re * ab + sc.im * tst;
                  }
                }
              }
            }
          }

          t = b_h_data[(i - 1) * b_h_size[0] + i];
          if (b_h_data[(i - 1) * b_h_size[0] + i].im != 0.0) {
            tst = rt_hypotd_snf(b_h_data[(i - 1) * b_h_size[0] + i].re,
                                b_h_data[(i - 1) * b_h_size[0] + i].im);
            b_h_data[i + b_h_size[0] * (i - 1)].re = tst;
            b_h_data[i + b_h_size[0] * (i - 1)].im = 0.0;
            if (t.im == 0.0) {
              t.re /= tst;
              t.im = 0.0;
            } else if (t.re == 0.0) {
              t.re = 0.0;
              t.im /= tst;
            } else {
              t.re /= tst;
              t.im /= tst;
            }

            if (n > i + 1) {
              c = ((i + 1) * ldh + i) + 1;
              sc.re = t.re;
              sc.im = -t.im;
              b = ((n - i) - 2) * ldh + c;
              for (br_tmp = c; ldh < 0 ? br_tmp >= b : br_tmp <= b; br_tmp +=
                   ldh) {
                tst = b_h_data[br_tmp - 1].re;
                ab = b_h_data[br_tmp - 1].im;
                b_h_data[br_tmp - 1].re = sc.re * tst - sc.im * ab;
                b_h_data[br_tmp - 1].im = sc.re * ab + sc.im * tst;
              }
            }

            c_tmp = i * ldh;
            b = c_tmp + i;
            for (br_tmp = c_tmp + 1; br_tmp <= b; br_tmp++) {
              tst = b_h_data[br_tmp - 1].re;
              ab = b_h_data[br_tmp - 1].im;
              b_h_data[br_tmp - 1].re = t.re * tst - t.im * ab;
              b_h_data[br_tmp - 1].im = t.re * ab + t.im * tst;
            }
          }

          its++;
        }
      }

      if (!goto140) {
        *info = i + 1;
        exitg1 = 1;
      } else {
        i = L - 2;
      }
    }
  }
}

static void cyperus_lowpass_module_eig(const complex_float32_t A_data[], const int32_t
  A_size[2], complex_float32_t V_data[], int32_t *V_size)
{
  complex_float32_t beta1_data[4];
  complex_float32_t T_data[16];
  uint8_t b_p;
  complex_float32_t c_h_data[16];
  int32_t istart;
  int32_t i;
  int32_t jend;
  int32_t n;
  complex_float32_t tmp_data[16];
  int32_t c_h_size[2];
  int32_t tmp_size[2];
  int32_t T_size_idx_0;
  int32_t b_idx_0;
  float brm;
  float bim;
  float sgnbi;
  float d;
  int32_t A_size_0;
  complex_float32_t V_data_0;
  int32_t exitg1;
  uint8_t exitg2;
  if (cyperus_lowpass_mo_anyNonFinite(A_data, A_size)) {
    if ((A_size[0] == 1) && (A_size[1] == 1)) {
      *V_size = 1;
      V_data[0].re = (rtNaN);
      V_data[0].im = 0.0;
    } else {
      *V_size = A_size[0];
      istart = A_size[0];
      for (A_size_0 = 0; A_size_0 < istart; A_size_0++) {
        V_data[A_size_0].re = (rtNaN);
        V_data[A_size_0].im = 0.0;
      }
    }
  } else if ((A_size[0] == 1) && (A_size[1] == 1)) {
    *V_size = 1;
    V_data[0] = A_data[0];
  } else {
    b_p = (A_size[0] == A_size[1]);
    if (b_p) {
      n = 0;
      exitg2 = 0;
      while ((!exitg2) && (n <= A_size[1] - 1)) {
        istart = 0;
        do {
          exitg1 = 0;
          if (istart <= n) {
            if ((!(A_data[A_size[0] * n + istart].re == A_data[A_size[0] *
                   istart + n].re)) || (!(A_data[A_size[0] * n + istart].im ==
                  -A_data[A_size[0] * istart + n].im))) {
              b_p = 0;
              exitg1 = 1;
            } else {
              istart++;
            }
          } else {
            n++;
            exitg1 = 2;
          }
        } while (exitg1 == 0);

        if (exitg1 == 1) {
          exitg2 = 1;
        }
      }
    }

    if (b_p) {
      if (cyperus_lowpass_mo_anyNonFinite(A_data, A_size)) {
        b_idx_0 = A_size[0];
        T_size_idx_0 = A_size[0];
        istart = A_size[0] * A_size[1] - 1;
        for (A_size_0 = 0; A_size_0 <= istart; A_size_0++) {
          T_data[A_size_0].re = (rtNaN);
          T_data[A_size_0].im = 0.0;
        }

        if (1 < A_size[0]) {
          istart = 2;
          if (A_size[0] - 2 < A_size[1] - 1) {
            A_size_0 = A_size[0] - 1;
          } else {
            A_size_0 = A_size[1];
          }

          jend = A_size_0 - 1;
          for (n = 0; n <= jend; n++) {
            for (i = istart; i <= b_idx_0; i++) {
              A_size_0 = (i + b_idx_0 * n) - 1;
              T_data[A_size_0].re = 0.0;
              T_data[A_size_0].im = 0.0;
            }

            istart++;
          }
        }
      } else {
        cyperus_lowpass_module_xgehrd(A_data, A_size, tmp_data, tmp_size);
        cyperus_lowpass_modu_eml_zlahqr(tmp_data, tmp_size, c_h_data, c_h_size,
          &n);
        T_size_idx_0 = c_h_size[0];
        istart = c_h_size[0] * c_h_size[1] - 1;
        if (0 <= istart) {
          memcpy(&T_data[0], &c_h_data[0], (istart + 1) * sizeof(float));
        }

        if (3 < c_h_size[0]) {
          T_data[3].re = 0.0;
          T_data[3].im = 0.0;
        }
      }

      n = T_size_idx_0 - 1;
      *V_size = T_size_idx_0;
      for (istart = 0; istart <= n; istart++) {
        V_data[istart] = T_data[T_size_idx_0 * istart + istart];
      }
    } else {
      cyperus_lowpass_module_xzgeev(A_data, A_size, &n, V_data, V_size,
        beta1_data, &istart);
      istart = *V_size;
      for (A_size_0 = 0; A_size_0 < istart; A_size_0++) {
        if (beta1_data[A_size_0].im == 0.0) {
          if (V_data[A_size_0].im == 0.0) {
            bim = V_data[A_size_0].re / beta1_data[A_size_0].re;
            brm = 0.0;
          } else if (V_data[A_size_0].re == 0.0) {
            bim = 0.0;
            brm = V_data[A_size_0].im / beta1_data[A_size_0].re;
          } else {
            bim = V_data[A_size_0].re / beta1_data[A_size_0].re;
            brm = V_data[A_size_0].im / beta1_data[A_size_0].re;
          }
        } else if (beta1_data[A_size_0].re == 0.0) {
          if (V_data[A_size_0].re == 0.0) {
            bim = V_data[A_size_0].im / beta1_data[A_size_0].im;
            brm = 0.0;
          } else if (V_data[A_size_0].im == 0.0) {
            bim = 0.0;
            brm = -(V_data[A_size_0].re / beta1_data[A_size_0].im);
          } else {
            bim = V_data[A_size_0].im / beta1_data[A_size_0].im;
            brm = -(V_data[A_size_0].re / beta1_data[A_size_0].im);
          }
        } else {
          brm = fabs(beta1_data[A_size_0].re);
          bim = fabs(beta1_data[A_size_0].im);
          if (brm > bim) {
            brm = beta1_data[A_size_0].im / beta1_data[A_size_0].re;
            d = brm * beta1_data[A_size_0].im + beta1_data[A_size_0].re;
            bim = (brm * V_data[A_size_0].im + V_data[A_size_0].re) / d;
            brm = (V_data[A_size_0].im - brm * V_data[A_size_0].re) / d;
          } else if (bim == brm) {
            d = beta1_data[A_size_0].re > 0.0 ? 0.5 : -0.5;
            sgnbi = beta1_data[A_size_0].im > 0.0 ? 0.5 : -0.5;
            bim = (V_data[A_size_0].re * d + V_data[A_size_0].im * sgnbi) / brm;
            brm = (V_data[A_size_0].im * d - V_data[A_size_0].re * sgnbi) / brm;
          } else {
            brm = beta1_data[A_size_0].re / beta1_data[A_size_0].im;
            d = brm * beta1_data[A_size_0].re + beta1_data[A_size_0].im;
            bim = (brm * V_data[A_size_0].re + V_data[A_size_0].im) / d;
            brm = (brm * V_data[A_size_0].im - V_data[A_size_0].re) / d;
          }
        }

        V_data_0.re = bim;
        V_data_0.im = brm;
        V_data[A_size_0] = V_data_0;
      }
    }
  }
}

static void cyperus_lowpass_module_roots_c(const float c[5], complex_float32_t r_data[],
  int32_t *r_size)
{
  int32_t k1;
  int32_t k2;
  int32_t nTrailingZeros;
  int32_t companDim;
  float ctmp[5];
  int32_t j;
  complex_float32_t a_data[16];
  complex_float32_t eiga_data[4];
  int32_t b_k;
  int32_t a_size[2];
  uint8_t exitg1;
  uint8_t exitg2;
  memset(&r_data[0], 0, sizeof(float) << 2U);
  k1 = 1;
  while ((k1 <= 5) && (!(c[k1 - 1] != 0.0))) {
    k1++;
  }

  k2 = 5;
  while ((k2 >= k1) && (!(c[k2 - 1] != 0.0))) {
    k2--;
  }

  nTrailingZeros = 4 - k2;
  if (k1 < k2) {
    companDim = k2 - k1;
    exitg1 = 0;
    while ((!exitg1) && (companDim > 0)) {
      j = 0;
      exitg2 = 0;
      while ((!exitg2) && (j + 1 <= companDim)) {
        ctmp[j] = c[k1 + j] / c[k1 - 1];
        if (rtIsInf(fabs(ctmp[j]))) {
          exitg2 = 1;
        } else {
          j++;
        }
      }

      if (j + 1 > companDim) {
        exitg1 = 1;
      } else {
        k1++;
        companDim--;
      }
    }

    if (companDim < 1) {
      if (1 > 5 - k2) {
        j = 0;
      } else {
        j = 5 - k2;
      }

      *r_size = j;
    } else {
      a_size[0] = companDim;
      a_size[1] = companDim;
      j = companDim * companDim - 1;
      if (0 <= j) {
        memset(&a_data[0], 0, (j + 1) * sizeof(float));
      }

      j = companDim - 2;
      for (b_k = 0; b_k <= j; b_k++) {
        k1 = companDim * b_k;
        a_data[k1].re = -ctmp[b_k];
        a_data[k1].im = 0.0;
        k1 = (b_k + k1) + 1;
        a_data[k1].re = 1.0;
        a_data[k1].im = 0.0;
      }

      k1 = companDim * (companDim - 1);
      a_data[k1].re = -ctmp[companDim - 1];
      a_data[k1].im = 0.0;
      if (0 <= nTrailingZeros) {
        memset(&r_data[0], 0, (nTrailingZeros + 1) * sizeof(float));
      }

      cyperus_lowpass_module_eig(a_data, a_size, eiga_data, &nTrailingZeros);
      for (nTrailingZeros = 0; nTrailingZeros < companDim; nTrailingZeros++) {
        r_data[(nTrailingZeros - k2) + 5] = eiga_data[nTrailingZeros];
      }

      *r_size = (companDim - k2) + 5;
    }
  } else {
    if (1 > 5 - k2) {
      j = 0;
    } else {
      j = 5 - k2;
    }

    *r_size = j;
  }
}

static uint8_t cyperus_lowpass_module_iseq(float x, float y)
{
  uint8_t p;
  float absxk;
  int32_t b_exponent;
  absxk = fabs(y / 2.0);
  if ((!rtIsInf(absxk)) && (!rtIsNaN(absxk))) {
    if (absxk <= 2.2250738585072014E-308) {
      absxk = 4.94065645841247E-324;
    } else {
      frexp(absxk, &b_exponent);
      absxk = ldexp(1.0, b_exponent - 53);
    }
  } else {
    absxk = (rtNaN);
  }

  if ((fabs(y - x) < absxk) || (rtIsInf(x) && rtIsInf(y) && ((x > 0.0) == (y >
         0.0)))) {
    p = 1;
  } else {
    p = 0;
  }

  return p;
}

float rt_atan2d_snf(float u0, float u1)
{
  float y;
  int32_t u0_0;
  int32_t u1_0;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = (rtNaN);
  } else if (rtIsInf(u0) && rtIsInf(u1)) {
    if (u0 > 0.0) {
      u0_0 = 1;
    } else {
      u0_0 = -1;
    }

    if (u1 > 0.0) {
      u1_0 = 1;
    } else {
      u1_0 = -1;
    }

    y = atan2(u0_0, u1_0);
  } else if (u1 == 0.0) {
    if (u0 > 0.0) {
      y = RT_PI / 2.0;
    } else if (u0 < 0.0) {
      y = -(RT_PI / 2.0);
    } else {
      y = 0.0;
    }
  } else {
    y = atan2(u0, u1);
  }

  return y;
}

static uint8_t cyperus_lowpass_module_sortLE(const complex_float32_t v_data[], int32_t
  idx1, int32_t idx2)
{
  uint8_t p;
  uint8_t SCALEB;
  uint8_t SCALEA;
  float absar;
  float absai;
  float absbr;
  float absbi;
  float Ma;
  float Mb;
  if (rtIsNaN(v_data[idx2 - 1].re) || rtIsNaN(v_data[idx2 - 1].im)) {
    p = (rtIsNaN(v_data[idx1 - 1].re) || rtIsNaN(v_data[idx1 - 1].im) ||
         ((!rtIsNaN(v_data[idx1 - 1].re)) && (!rtIsNaN(v_data[idx1 - 1].im))));
  } else if (rtIsNaN(v_data[idx1 - 1].re) || rtIsNaN(v_data[idx1 - 1].im)) {
    p = 0;
  } else {
    absar = fabs(v_data[idx1 - 1].re);
    if ((absar > 8.9884656743115785E+307) || (fabs(v_data[idx1 - 1].im) >
         8.9884656743115785E+307)) {
      SCALEA = 1;
    } else {
      SCALEA = 0;
    }

    absbr = fabs(v_data[idx2 - 1].re);
    if ((absbr > 8.9884656743115785E+307) || (fabs(v_data[idx2 - 1].im) >
         8.9884656743115785E+307)) {
      SCALEB = 1;
    } else {
      SCALEB = 0;
    }

    if (SCALEA || SCALEB) {
      absbi = rt_hypotd_snf(v_data[idx1 - 1].re / 2.0, v_data[idx1 - 1].im / 2.0);
      absai = rt_hypotd_snf(v_data[idx2 - 1].re / 2.0, v_data[idx2 - 1].im / 2.0);
    } else {
      absbi = rt_hypotd_snf(v_data[idx1 - 1].re, v_data[idx1 - 1].im);
      absai = rt_hypotd_snf(v_data[idx2 - 1].re, v_data[idx2 - 1].im);
    }

    if (cyperus_lowpass_module_iseq(absbi, absai)) {
      Mb = fabs(v_data[idx1 - 1].im);
      absbi = fabs(v_data[idx2 - 1].im);
      if (absar > Mb) {
        Ma = absar;
        absar = Mb;
      } else {
        Ma = Mb;
      }

      if (absbr > absbi) {
        Mb = absbr;
        absbr = absbi;
      } else {
        Mb = absbi;
      }

      if (Ma > Mb) {
        if (absar < absbr) {
          absbi = Ma - Mb;
          absai = (absar / 2.0 + absbr / 2.0) / (Ma / 2.0 + Mb / 2.0) * (absbr -
            absar);
        } else {
          absbi = Ma;
          absai = Mb;
        }
      } else if (Ma < Mb) {
        if (absar > absbr) {
          absai = Mb - Ma;
          absbi = (absar / 2.0 + absbr / 2.0) / (Ma / 2.0 + Mb / 2.0) * (absar -
            absbr);
        } else {
          absbi = Ma;
          absai = Mb;
        }
      } else {
        absbi = absar;
        absai = absbr;
      }

      if (cyperus_lowpass_module_iseq(absbi, absai)) {
        absbi = rt_atan2d_snf(v_data[idx1 - 1].im, v_data[idx1 - 1].re);
        absai = rt_atan2d_snf(v_data[idx2 - 1].im, v_data[idx2 - 1].re);
        if (cyperus_lowpass_module_iseq(absbi, absai)) {
          absar = v_data[idx1 - 1].re;
          absbr = v_data[idx1 - 1].im;
          absai = v_data[idx2 - 1].re;
          Ma = v_data[idx2 - 1].im;
          if (absbi > 0.78539816339744828) {
            if (absbi > 2.3561944901923448) {
              absar = -absbr;
              absai = -Ma;
            } else {
              absar = -absar;
              absai = -absai;
            }
          } else if (absbi > -0.78539816339744828) {
            absar = absbr;
            absai = Ma;
          } else {
            if (!(absbi > -2.3561944901923448)) {
              absar = -absbr;
              absai = -Ma;
            }
          }

          absbi = absar;
          if (cyperus_lowpass_module_iseq(absar, absai)) {
            absbi = 0.0;
            absai = 0.0;
          }
        }
      }
    }

    p = (absbi <= absai);
  }

  return p;
}

static void cyperus_lowpass_module_sort_c(complex_float32_t x_data[], int32_t *x_size)
{
  int32_t dim;
  int32_t vlen;
  complex_float32_t vwork_data[4];
  int32_t vstride;
  int32_t b_j;
  int32_t n;
  int32_t b_idx_data[4];
  int32_t iwork_data[4];
  int32_t i;
  int32_t i2;
  int32_t j;
  int32_t pEnd;
  int32_t p;
  int32_t q;
  int32_t qEnd;
  int32_t kEnd;
  complex_float32_t xwork_data[4];
  int32_t dim_idx_0;
  dim = 2;
  if (*x_size != 1) {
    dim = 1;
  }

  if (dim <= 1) {
    vlen = *x_size;
    dim_idx_0 = *x_size;
  } else {
    vlen = 1;
    dim_idx_0 = 1;
  }

  vlen--;
  vstride = 1;
  i = dim - 2;
  for (dim = 0; dim <= i; dim++) {
    vstride *= *x_size;
  }

  for (b_j = 0; b_j < vstride; b_j++) {
    for (i2 = 0; i2 <= vlen; i2++) {
      vwork_data[i2] = x_data[i2 * vstride + b_j];
    }

    n = dim_idx_0 + 1;
    if (dim_idx_0 != 0) {
      if (0 <= dim_idx_0 - 1) {
        memset(&b_idx_data[0], 0, dim_idx_0 * sizeof(int32_t));
      }

      for (i2 = 1; i2 <= dim_idx_0 - 1; i2 += 2) {
        if (cyperus_lowpass_module_sortLE(vwork_data, i2, i2 + 1)) {
          b_idx_data[i2 - 1] = i2;
          b_idx_data[i2] = i2 + 1;
        } else {
          b_idx_data[i2 - 1] = i2 + 1;
          b_idx_data[i2] = i2;
        }
      }

      if ((dim_idx_0 & 1U) != 0U) {
        b_idx_data[dim_idx_0 - 1] = dim_idx_0;
      }

      i = 2;
      while (i < n - 1) {
        i2 = i << 1;
        j = 1;
        for (pEnd = 1 + i; pEnd < n; pEnd = qEnd + i) {
          p = j - 1;
          q = pEnd - 1;
          qEnd = j + i2;
          if (qEnd > n) {
            qEnd = n;
          }

          dim = 0;
          kEnd = qEnd - j;
          while (dim + 1 <= kEnd) {
            if (cyperus_lowpass_module_sortLE(vwork_data, b_idx_data[p],
                 b_idx_data[q])) {
              iwork_data[dim] = b_idx_data[p];
              p++;
              if (p + 1 == pEnd) {
                while (q + 1 < qEnd) {
                  dim++;
                  iwork_data[dim] = b_idx_data[q];
                  q++;
                }
              }
            } else {
              iwork_data[dim] = b_idx_data[q];
              q++;
              if (q + 1 == qEnd) {
                while (p + 1 < pEnd) {
                  dim++;
                  iwork_data[dim] = b_idx_data[p];
                  p++;
                }
              }
            }

            dim++;
          }

          for (pEnd = 0; pEnd < kEnd; pEnd++) {
            b_idx_data[(j + pEnd) - 1] = iwork_data[pEnd];
          }

          j = qEnd;
        }

        i = i2;
      }

      if (0 <= n - 2) {
        memcpy(&xwork_data[0], &vwork_data[0], (n + -1) * sizeof(float));
      }

      for (i = 0; i <= n - 2; i++) {
        vwork_data[i] = xwork_data[b_idx_data[i] - 1];
      }
    }

    for (pEnd = 0; pEnd <= vlen; pEnd++) {
      x_data[b_j + pEnd * vstride] = vwork_data[pEnd];
    }
  }
}

static void cyperus_lowpass_module_roots(const float c[3], complex_float32_t r_data[],
  int32_t *r_size)
{
  int32_t k1;
  int32_t k2;
  int32_t nTrailingZeros;
  int32_t companDim;
  float ctmp[3];
  int32_t j;
  complex_float32_t a_data[4];
  complex_float32_t eiga_data[2];
  complex_float32_t tmp_data[4];
  int32_t a_size[2];
  uint8_t exitg1;
  uint8_t exitg2;
  r_data[0].re = 0.0;
  r_data[0].im = 0.0;
  r_data[1].re = 0.0;
  r_data[1].im = 0.0;
  k1 = 1;
  while ((k1 <= 3) && (!(c[k1 - 1] != 0.0))) {
    k1++;
  }

  k2 = 3;
  while ((k2 >= k1) && (!(c[k2 - 1] != 0.0))) {
    k2--;
  }

  nTrailingZeros = 2 - k2;
  if (k1 < k2) {
    companDim = k2 - k1;
    exitg1 = 0;
    while ((!exitg1) && (companDim > 0)) {
      j = 0;
      exitg2 = 0;
      while ((!exitg2) && (j + 1 <= companDim)) {
        ctmp[j] = c[k1 + j] / c[k1 - 1];
        if (rtIsInf(fabs(ctmp[j]))) {
          exitg2 = 1;
        } else {
          j++;
        }
      }

      if (j + 1 > companDim) {
        exitg1 = 1;
      } else {
        k1++;
        companDim--;
      }
    }

    if (companDim < 1) {
      if (1 > 3 - k2) {
        k1 = 0;
      } else {
        k1 = 3 - k2;
      }

      if (0 <= k1 - 1) {
        eiga_data[0] = r_data[0];
      }

      *r_size = k1;
      if (0 <= k1 - 1) {
        r_data[0] = eiga_data[0];
      }
    } else {
      a_size[0] = companDim;
      a_size[1] = companDim;
      k1 = companDim * companDim - 1;
      if (0 <= k1) {
        memset(&a_data[0], 0, (k1 + 1) * sizeof(float));
      }

      if (0 <= companDim - 2) {
        a_data[0].re = -ctmp[0];
        a_data[0].im = 0.0;
        a_data[1].re = 1.0;
        a_data[1].im = 0.0;
      }

      j = companDim * (companDim - 1);
      a_data[j].re = -ctmp[companDim - 1];
      a_data[j].im = 0.0;
      if (0 <= nTrailingZeros) {
        memset(&r_data[0], 0, (nTrailingZeros + 1) * sizeof(float));
      }

      cyperus_lowpass_module_eig(a_data, a_size, tmp_data, &nTrailingZeros);
      if (0 <= nTrailingZeros - 1) {
        memcpy(&eiga_data[0], &tmp_data[0], nTrailingZeros * sizeof(float));
      }

      for (nTrailingZeros = 0; nTrailingZeros < companDim; nTrailingZeros++) {
        r_data[(nTrailingZeros - k2) + 3] = eiga_data[nTrailingZeros];
      }

      *r_size = (companDim - k2) + 3;
    }
  } else {
    if (1 > 3 - k2) {
      k1 = 0;
    } else {
      k1 = 3 - k2;
    }

    *r_size = k1;
  }
}

static void cyperus_lowpa_designEachParamEQ(float N, float BW, float B_data[],
  int32_t B_size[2], float A_data[], int32_t A_size[2])
{
  float No2;
  int32_t nextidx;
  float Br_data[25];
  float Ar_data[25];
  complex_float32_t rs_data[4];
  complex_float32_t rs2_data[4];
  float Bf_data[25];
  float Af_data[25];
  float G_data[24];
  int32_t e;
  float w_data[15];
  uint8_t y;
  uint8_t x[2];
  int32_t ncols;
  int32_t ibmat;
  complex_float32_t b_x_data[2];
  float Br[5];
  float Br_0[3];
  uint8_t Bf_data_0[10];
  int32_t loop_ub;
  int32_t Bf_size[2];
  int32_t Af_size[2];
  int32_t Ar_size[2];
  int32_t Br_size[2];
  int32_t Af_size_0[2];
  int32_t Bf_size_0[2];
  int32_t Br_size_idx_0;
  int32_t Ar_size_idx_0;
  float p1_idx_1_re;
  float p1_idx_2_re;
  float p2_idx_1_re;
  float p2_idx_2_re;
  float p3_idx_1_re;
  float p3_idx_2_re;
  float p4_idx_1_re;
  float p4_idx_2_re;
  int32_t B_size_tmp;
  int32_t loop_ub_tmp;
  int32_t A_data_tmp;
  float p1_idx_1_re_tmp;
  float p2_idx_1_re_tmp;
  float p3_idx_1_re_tmp;
  float p4_idx_1_re_tmp;
  uint8_t guard1 = 0;
  uint8_t guard2 = 0;
  uint8_t exitg1;
  No2 = N / 2.0;
  B_size[0] = 3;
  B_size_tmp = (int32_t)No2;
  B_size[1] = B_size_tmp;
  loop_ub_tmp = 3 * B_size_tmp - 1;
  if (0 <= loop_ub_tmp) {
    memset(&B_data[0], 0, (loop_ub_tmp + 1) * sizeof(float));
  }

  for (ncols = 0; ncols < B_size_tmp; ncols++) {
    B_data[3 * ncols] = 1.0;
  }

  A_size[0] = 2;
  A_size[1] = B_size_tmp;
  loop_ub = (B_size_tmp << 1) - 1;
  if (0 <= loop_ub) {
    memset(&A_data[0], 0, (loop_ub + 1) * sizeof(float));
  }

  cyperus_lowpass_module_hpeq(No2, BW, Bf_data, Bf_size, Af_data, Af_size);
  loop_ub = Bf_size[0];
  Bf_size_0[0] = Bf_size[0];
  Bf_size_0[1] = 2;
  nextidx = Bf_size[0];
  for (ncols = 0; ncols < loop_ub; ncols++) {
    Bf_data_0[ncols] = (Bf_data[nextidx * 3 + ncols] == 0.0);
    Bf_data_0[ncols + loop_ub] = (Bf_data[(nextidx << 2) + ncols] == 0.0);
  }

  cyperus_lowpass_module_all(Bf_data_0, Bf_size_0, x);
  y = 1;
  ibmat = 0;
  exitg1 = 0;
  while ((!exitg1) && (ibmat < 2)) {
    if (!x[ibmat]) {
      y = 0;
      exitg1 = 1;
    } else {
      ibmat++;
    }
  }

  guard1 = 0;
  if (y) {
    loop_ub = Af_size[0];
    Af_size_0[0] = Af_size[0];
    Af_size_0[1] = 2;
    nextidx = Af_size[0];
    for (ncols = 0; ncols < loop_ub; ncols++) {
      Bf_data_0[ncols] = (Af_data[nextidx * 3 + ncols] == 0.0);
      Bf_data_0[ncols + loop_ub] = (Af_data[(nextidx << 2) + ncols] == 0.0);
    }

    cyperus_lowpass_module_all(Bf_data_0, Af_size_0, x);
    ibmat = 0;
    exitg1 = 0;
    while ((!exitg1) && (ibmat < 2)) {
      if (!x[ibmat]) {
        y = 0;
        exitg1 = 1;
      } else {
        ibmat++;
      }
    }

    if (y) {
      loop_ub = Bf_size[0];
      nextidx = Bf_size[0];
      for (ncols = 0; ncols < loop_ub; ncols++) {
        B_data[3 * ncols] = Bf_data[ncols];
        B_data[1 + 3 * ncols] = Bf_data[ncols + nextidx];
        B_data[2 + 3 * ncols] = Bf_data[(nextidx << 1) + ncols];
      }

      loop_ub = Af_size[0];
      nextidx = Af_size[0];
      for (ncols = 0; ncols < loop_ub; ncols++) {
        A_data_tmp = ncols << 1;
        A_data[A_data_tmp] = Af_data[ncols + nextidx];
        A_data[1 + A_data_tmp] = Af_data[(nextidx << 1) + ncols];
      }
    } else {
      guard1 = 1;
    }
  } else {
    guard1 = 1;
  }

  if (guard1) {
    if (rt_remd_snf(No2, 2.0) != 0.0) {
      B_data[0] = Bf_data[0];
      B_data[1] = Bf_data[Bf_size[0]];
      B_data[2] = Bf_data[Bf_size[0] << 1];
      A_data[0] = Af_data[Af_size[0]];
      A_data[1] = Af_data[Af_size[0] << 1];
      nextidx = 2;
      if (2 > Bf_size[0]) {
        e = 0;
        loop_ub = -1;
      } else {
        e = 1;
        loop_ub = Bf_size[0] - 1;
      }

      Ar_size_idx_0 = loop_ub - e;
      Br_size_idx_0 = Ar_size_idx_0 + 1;
      for (ncols = 0; ncols < 5; ncols++) {
        for (loop_ub = 0; loop_ub <= Ar_size_idx_0; loop_ub++) {
          Br_data[loop_ub + Br_size_idx_0 * ncols] = Bf_data[(e + loop_ub) +
            Bf_size[0] * ncols];
        }
      }

      if (2 > Af_size[0]) {
        e = 0;
        loop_ub = -1;
      } else {
        e = 1;
        loop_ub = Af_size[0] - 1;
      }

      ibmat = loop_ub - e;
      Ar_size_idx_0 = ibmat + 1;
      for (ncols = 0; ncols < 5; ncols++) {
        for (loop_ub = 0; loop_ub <= ibmat; loop_ub++) {
          Ar_data[loop_ub + Ar_size_idx_0 * ncols] = Af_data[(e + loop_ub) +
            Af_size[0] * ncols];
        }
      }
    } else {
      nextidx = 1;
      Br_size_idx_0 = Bf_size[0];
      loop_ub = Bf_size[0] * Bf_size[1] - 1;
      if (0 <= loop_ub) {
        memcpy(&Br_data[0], &Bf_data[0], (loop_ub + 1) * sizeof(float));
      }

      Ar_size_idx_0 = Af_size[0];
      loop_ub = Af_size[0] * Af_size[1] - 1;
      if (0 <= loop_ub) {
        memcpy(&Ar_data[0], &Af_data[0], (loop_ub + 1) * sizeof(float));
      }
    }

    for (ncols = 0; ncols <= loop_ub_tmp; ncols++) {
      G_data[ncols] = 1.0;
    }

    if (nextidx > No2) {
      e = 0;
      loop_ub = 1;
    } else {
      e = nextidx - 1;
      loop_ub = 2;
    }

    ncols = Br_size_idx_0 - 1;
    for (loop_ub_tmp = 0; loop_ub_tmp <= ncols; loop_ub_tmp++) {
      ibmat = loop_ub_tmp * 3;
      w_data[ibmat] = Br_data[loop_ub_tmp];
      w_data[ibmat + 1] = Br_data[loop_ub_tmp];
      w_data[ibmat + 2] = Br_data[loop_ub_tmp];
    }

    Br_size[0] = Br_size_idx_0;
    Br_size[1] = 2;
    for (ncols = 0; ncols < Br_size_idx_0; ncols++) {
      ibmat = 3 * (e + loop_ub * ncols);
      G_data[ibmat] = w_data[3 * ncols];
      G_data[1 + ibmat] = w_data[3 * ncols + 1];
      G_data[2 + ibmat] = w_data[3 * ncols + 2];
      ibmat = ncols + Br_size_idx_0;
      Bf_data_0[ncols] = (Br_data[ibmat] == 0.0);
      Bf_data_0[ibmat] = (Br_data[Br_size_idx_0 * 3 + ncols] == 0.0);
    }

    cyperus_lowpass_module_all(Bf_data_0, Br_size, x);
    y = 1;
    ibmat = 0;
    exitg1 = 0;
    while ((!exitg1) && (ibmat < 2)) {
      if (!x[ibmat]) {
        y = 0;
        exitg1 = 1;
      } else {
        ibmat++;
      }
    }

    guard2 = 0;
    if (y) {
      Ar_size[0] = Ar_size_idx_0;
      Ar_size[1] = 2;
      for (ncols = 0; ncols < Ar_size_idx_0; ncols++) {
        e = ncols + Ar_size_idx_0;
        Bf_data_0[ncols] = (Ar_data[e] == 0.0);
        Bf_data_0[e] = (Ar_data[Ar_size_idx_0 * 3 + ncols] == 0.0);
      }

      cyperus_lowpass_module_all(Bf_data_0, Ar_size, x);
      ibmat = 0;
      exitg1 = 0;
      while ((!exitg1) && (ibmat < 2)) {
        if (!x[ibmat]) {
          y = 0;
          exitg1 = 1;
        } else {
          ibmat++;
        }
      }

      if (y) {
        e = (int32_t)(((No2 - 1.0) + (2.0 - (float)nextidx)) / 2.0) - 1;
        for (ibmat = 0; ibmat <= e; ibmat++) {
          No2 = (float)ibmat * 2.0 + (float)nextidx;
          ncols = (int32_t)ceil(No2 / 2.0);
          Br_0[0] = Br_data[ncols - 1];
          Br_0[1] = Br_data[((Br_size_idx_0 << 1) + ncols) - 1];
          Br_0[2] = Br_data[((Br_size_idx_0 << 2) + ncols) - 1];
          cyperus_lowpass_module_roots(Br_0, b_x_data, &A_data_tmp);
          loop_ub = A_data_tmp;
          if (0 <= A_data_tmp - 1) {
            memcpy(&rs_data[0], &b_x_data[0], A_data_tmp * sizeof(float));
          }

          Br_0[0] = Ar_data[ncols - 1];
          Br_0[1] = Ar_data[((Ar_size_idx_0 << 1) + ncols) - 1];
          Br_0[2] = Ar_data[((Ar_size_idx_0 << 2) + ncols) - 1];
          cyperus_lowpass_module_roots(Br_0, b_x_data, &A_data_tmp);
          if (0 <= A_data_tmp - 1) {
            memcpy(&rs2_data[0], &b_x_data[0], A_data_tmp * sizeof(float));
          }

          if (0 <= loop_ub - 1) {
            memcpy(&b_x_data[0], &rs_data[0], loop_ub * sizeof(float));
          }

          ncols = loop_ub - 1;
          for (loop_ub_tmp = 0; loop_ub_tmp <= ncols; loop_ub_tmp++) {
            b_x_data[loop_ub_tmp] = cyperus_lowpass_module_sqrt
              (b_x_data[loop_ub_tmp]);
          }

          if (0 <= loop_ub - 1) {
            memcpy(&rs_data[0], &b_x_data[0], loop_ub * sizeof(float));
          }

          cyperus_lowpass_module_sort_c(rs_data, &loop_ub);
          if (0 <= A_data_tmp - 1) {
            memcpy(&b_x_data[0], &rs2_data[0], A_data_tmp * sizeof(float));
          }

          ncols = A_data_tmp - 1;
          for (loop_ub_tmp = 0; loop_ub_tmp <= ncols; loop_ub_tmp++) {
            b_x_data[loop_ub_tmp] = cyperus_lowpass_module_sqrt
              (b_x_data[loop_ub_tmp]);
          }

          if (0 <= A_data_tmp - 1) {
            memcpy(&rs2_data[0], &b_x_data[0], A_data_tmp * sizeof(float));
          }

          cyperus_lowpass_module_sort_c(rs2_data, &A_data_tmp);
          p1_idx_1_re_tmp = -rs_data[0].im * 0.0;
          p1_idx_1_re = -rs_data[0].re - p1_idx_1_re_tmp;
          p1_idx_2_re = -rs_data[0].re * p1_idx_1_re - (-rs_data[0].re * 0.0 +
            -rs_data[0].im) * rs_data[0].im;
          p2_idx_1_re_tmp = rs_data[1].im * 0.0;
          p2_idx_1_re = rs_data[1].re - p2_idx_1_re_tmp;
          p2_idx_2_re = rs_data[1].re * p2_idx_1_re - (rs_data[1].re * 0.0 +
            rs_data[1].im) * -rs_data[1].im;
          p3_idx_1_re_tmp = -rs2_data[0].im * 0.0;
          p3_idx_1_re = -rs2_data[0].re - p3_idx_1_re_tmp;
          p3_idx_2_re = -rs2_data[0].re * p3_idx_1_re - (-rs2_data[0].re * 0.0 +
            -rs2_data[0].im) * rs2_data[0].im;
          p4_idx_1_re_tmp = rs2_data[1].im * 0.0;
          p4_idx_1_re = rs2_data[1].re - p4_idx_1_re_tmp;
          p4_idx_2_re = rs2_data[1].re * p4_idx_1_re - (rs2_data[1].re * 0.0 +
            rs2_data[1].im) * -rs2_data[1].im;
          for (loop_ub = 2; loop_ub >= 2; loop_ub--) {
            p1_idx_1_re -= rs_data[0].re - p1_idx_1_re_tmp;
            p2_idx_1_re -= -rs_data[1].re - p2_idx_1_re_tmp;
            p3_idx_1_re -= rs2_data[0].re - p3_idx_1_re_tmp;
            p4_idx_1_re -= -rs2_data[1].re - p4_idx_1_re_tmp;
          }

          ncols = 3 * ((int32_t)No2 - 1);
          B_data[ncols] = 1.0;
          B_data[ncols + 1] = p1_idx_1_re;
          B_data[ncols + 2] = p1_idx_2_re;
          A_data_tmp = ((int32_t)No2 - 1) << 1;
          A_data[A_data_tmp] = p3_idx_1_re;
          A_data[A_data_tmp + 1] = p3_idx_2_re;
          ncols = 3 * ((int32_t)(No2 + 1.0) - 1);
          B_data[ncols] = 1.0;
          B_data[ncols + 1] = p2_idx_1_re;
          B_data[ncols + 2] = p2_idx_2_re;
          A_data_tmp = ((int32_t)(No2 + 1.0) - 1) << 1;
          A_data[A_data_tmp] = p4_idx_1_re;
          A_data[A_data_tmp + 1] = p4_idx_2_re;
        }
      } else {
        guard2 = 1;
      }
    } else {
      guard2 = 1;
    }

    if (guard2) {
      ibmat = (int32_t)(((No2 - 1.0) + (2.0 - (float)nextidx)) / 2.0) - 1;
      for (loop_ub_tmp = 0; loop_ub_tmp <= ibmat; loop_ub_tmp++) {
        No2 = (float)loop_ub_tmp * 2.0 + (float)nextidx;
        ncols = (int32_t)ceil(No2 / 2.0);
        for (loop_ub = 0; loop_ub < 5; loop_ub++) {
          Br[loop_ub] = Br_data[(Br_size_idx_0 * loop_ub + ncols) - 1];
        }

        cyperus_lowpass_module_roots_c(Br, rs_data, &loop_ub);
        cyperus_lowpass_module_sort_c(rs_data, &loop_ub);
        for (loop_ub = 0; loop_ub < 5; loop_ub++) {
          Br[loop_ub] = Ar_data[(Ar_size_idx_0 * loop_ub + ncols) - 1];
        }

        cyperus_lowpass_module_roots_c(Br, rs2_data, &A_data_tmp);
        cyperus_lowpass_module_sort_c(rs2_data, &A_data_tmp);
        p1_idx_1_re = -rs_data[0].re - -rs_data[0].im * 0.0;
        p1_idx_2_re = -rs_data[1].re * p1_idx_1_re - (-rs_data[0].re * 0.0 +
          -rs_data[0].im) * -rs_data[1].im;
        p2_idx_1_re = -rs_data[2].re - -rs_data[2].im * 0.0;
        p2_idx_2_re = -rs_data[3].re * p2_idx_1_re - (-rs_data[2].re * 0.0 +
          -rs_data[2].im) * -rs_data[3].im;
        p3_idx_1_re = -rs2_data[0].re - -rs2_data[0].im * 0.0;
        p3_idx_2_re = -rs2_data[1].re * p3_idx_1_re - (-rs2_data[0].re * 0.0 +
          -rs2_data[0].im) * -rs2_data[1].im;
        p4_idx_1_re = -rs2_data[2].re - -rs2_data[2].im * 0.0;
        p4_idx_2_re = -rs2_data[3].re * p4_idx_1_re - (-rs2_data[2].re * 0.0 +
          -rs2_data[2].im) * -rs2_data[3].im;
        for (loop_ub = 2; loop_ub >= 2; loop_ub--) {
          p1_idx_1_re -= rs_data[1].re - rs_data[1].im * 0.0;
          p2_idx_1_re -= rs_data[3].re - rs_data[3].im * 0.0;
          p3_idx_1_re -= rs2_data[1].re - rs2_data[1].im * 0.0;
          p4_idx_1_re -= rs2_data[3].re - rs2_data[3].im * 0.0;
        }

        e = (int32_t)No2;
        ncols = 3 * (e - 1);
        B_data[ncols] = 1.0;
        B_data[ncols + 1] = p1_idx_1_re;
        B_data[ncols + 2] = p1_idx_2_re;
        A_data_tmp = (e - 1) << 1;
        A_data[A_data_tmp] = p3_idx_1_re;
        A_data[A_data_tmp + 1] = p3_idx_2_re;
        e = (int32_t)(No2 + 1.0);
        ncols = 3 * (e - 1);
        B_data[ncols] = 1.0;
        B_data[ncols + 1] = p2_idx_1_re;
        B_data[ncols + 2] = p2_idx_2_re;
        A_data_tmp = (e - 1) << 1;
        A_data[A_data_tmp] = p4_idx_1_re;
        A_data[A_data_tmp + 1] = p4_idx_2_re;
      }
    }

    B_size[0] = 3;
    loop_ub = 3 * B_size_tmp - 1;
    for (ncols = 0; ncols <= loop_ub; ncols++) {
      B_data[ncols] *= G_data[ncols];
    }
  }
}

static void cyperus_lo_designVarSlopeFilter(dsp_module_parameters_t *parameters)
{
  float Fc = parameters.float32_type[0];
  float Slope = parameters.float32_type[1];

  float B[12];
  float A[8];
  
  int32_t N;
  float b_B_data[24];
  float b_A_data[16];
  float b;
  int32_t i;
  int32_t b_B_size[2];
  int32_t b_A_size[2];
  int32_t A_tmp;

  if (!(Fc < 1.0)) {
    Fc = 1.0;
  }

  memcpy(&B,
	 parameters->float32_type + (3 * sizeof(float)), 
	   sizeof(float) * 12);
  memcpy(&A,
	 parameters->float32_type + (15 * sizeof(float)), 
	 sizeof(float) * 8);
  
  Slope = rt_roundd_snf(Slope / 6.0) * 6.0;
  memset(&B[0], 0, 12U * sizeof(float));
  B[0] = 1.0;
  B[3] = 1.0;
  B[6] = 1.0;
  B[9] = 1.0;
  memset(&A[0], 0, sizeof(float) << 3U);
  if (Slope != 0.0) {
    N = 2;
    if (Slope == 12.0) {
      N = 4;
    } else if (Slope == 18.0) {
      N = 6;
    } else if (Slope == 24.0) {
      N = 8;
    } else if (Slope == 30.0) {
      N = 10;
    } else if (Slope == 36.0) {
      N = 12;
    } else if (Slope == 42.0) {
      N = 14;
    } else {
      if (Slope == 48.0) {
        N = 16;
      }
    }

    b = 1.0 - Fc;
    if (1.0 - Fc > 1.0) {
      b = 1.0;
    }

    cyperus_lowpa_designEachParamEQ((float)N, b, b_B_data, b_B_size, b_A_data,
      b_A_size);
    N = (int32_t)ceil((float)N / 4.0);
    for (i = 0; i < N; i++) {
      B[3 * i] = b_B_data[3 * i];
      B[1 + 3 * i] = b_B_data[3 * i + 1];
      B[2 + 3 * i] = b_B_data[3 * i + 2];
    }

    for (i = 0; i < N; i++) {
      A_tmp = i << 1;
      A[A_tmp] = b_A_data[A_tmp];
      A[1 + (i << 1)] = b_A_data[A_tmp + 1];
    }

    memcpy(parameters->float32_type + (3 * sizeof(float)),
	   &B, 
	   sizeof(float) * 12);
    memcpy(parameters->float32_type + (15 * sizeof(float)),
	   &A, 
	   sizeof(float) * 8);
  }
}

void math_modules_dsp_filter_varslope_lowpass_init(dsp_module_parameters_t *parameters, int jack_sr) {
  /* initialize non-finites */
  int i;
  rt_InitInfAndNaN(sizeof(float));
  for (i = 0; i < 8; i++) {
    parameters->float32_type[i + 32]] = 0.0f;
  } 
  cyperus_lo_designVarSlopeFilter(parameters);  
} /* cyperus_filter_varslope_lowpass_init */

void math_modules_dsp_filter_varslope_lowpass_edit(dsp_module_parameters_t *parameters) {
  cyperus_lo_designVarSlopeFilter(parameters);
}

float math_modules_dsp_filter_varslope_lowpass(struct cyperus_parameters *filter, int samplerate, int pos) {
  float insample = filter->in;
  
  void *audio;
  uint8_t flag;
  float Fs;
  float Slope;
  int i;
  float *u0;
  
  float numAccum;
  float stageIn;
  float stageOut;

  int ioIdx;

  float B[12];
  float A[8];
  float W0_FILT_STATES[16];

  memcpy(&B,
	 filter->parameters->float32_type + (3 * sizeof(float)), 
	 sizeof(float) * 12);
  memcpy(&A,
	 filter->parameters->float32_type + (15 * sizeof(float)), 
	 sizeof(float) * 8);
  memcpy(&W0_FILT_STATES,
	 filter->parameters->float32_type + (23 * sizeof(float)), 
	 sizeof(float) * 16);
  
  numAccum = W0_FILT_STATES[0];
  stageOut = B[0] * insample + numAccum;
  numAccum = W0_FILT_STATES[1];
  numAccum += B[1] * insample;
  W0_FILT_STATES[0] = numAccum - A[0] * stageOut;
  W0_FILT_STATES[1] = B[2] * insample -
    A[1] * stageOut;
  stageIn = stageOut;
  numAccum = W0_FILT_STATES[2];
  stageOut = B[3] * stageOut + numAccum;
  numAccum = W0_FILT_STATES[3];
  numAccum += B[4] * stageIn;
  W0_FILT_STATES[2] = numAccum - A[2] * stageOut;
  W0_FILT_STATES[3] = B[5] * stageIn -
    A[3] * stageOut;
  stageIn = stageOut;
  numAccum = W0_FILT_STATES[4];
  stageOut = B[6] * stageOut + numAccum;
  numAccum = W0_FILT_STATES[5];
  numAccum += B[7] * stageIn;
  W0_FILT_STATES[4] = numAccum - A[4] * stageOut;
  W0_FILT_STATES[5] = B[8] * stageIn -
    A[5] * stageOut;
  stageIn = stageOut;
  numAccum = W0_FILT_STATES[6];
  stageOut = B[9] * stageOut + numAccum;
  numAccum = W0_FILT_STATES[7];
  numAccum += B[10] * stageIn;
  W0_FILT_STATES[6] = numAccum - A[6] * stageOut;
  W0_FILT_STATES[7] = B[11] * stageIn -
    A[7] * stageOut;

  memcpy(filter->parameters->float32_type + (23 * sizeof(float)),
	 &W0_FILT_STATES, 
	 sizeof(float) * 16);

  return stageOut * filter->parameters->float32_type[1];
}  /* cyperus_filter_varslope_lowpass */
