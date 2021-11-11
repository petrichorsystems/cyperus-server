/*
 * rtGetNaN.c
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

/*
 * Abstract:
 *      Function to initialize non-finite, NaN
 */
#include "math_utils.h"
#define NumBitsPerChar                 8U

float rtInf;
float rtMinusInf;
float rtNaN;
float rtInfF;
float rtMinusInfF;
float rtNaNF;

/*
 * Initialize rtNaN needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
float rtGetNaN(void)
{
  size_t bitsPerReal = sizeof(float) * (NumBitsPerChar);
  float nan = 0.0;
  if (bitsPerReal == 32U) {
    nan = rtGetNaNF();
  } else {
    uint16_t one = 1U;
    enum {
      LittleEndian,
      BigEndian
    } machByteOrder = (*((uint8_t *) &one) == 1U) ? LittleEndian : BigEndian;
    switch (machByteOrder) {
     case LittleEndian:
      {
        union {
          LittleEndianIEEEDouble bitVal;
          float fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0xFFF80000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        nan = tmpVal.fltVal;
        break;
      }

     case BigEndian:
      {
        union {
          BigEndianIEEEDouble bitVal;
          float fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0x7FFFFFFFU;
        tmpVal.bitVal.words.wordL = 0xFFFFFFFFU;
        nan = tmpVal.fltVal;
        break;
      }
    }
  }

  return nan;
}

/*
 * Initialize rtNaNF needed by the generated code.
 * NaN is initialized as non-signaling. Assumes IEEE.
 */
float rtGetNaNF(void)
{
  IEEESingle nanF = { { 0 } };

  uint16_t one = 1U;
  enum {
    LittleEndian,
    BigEndian
  } machByteOrder = (*((uint8_t *) &one) == 1U) ? LittleEndian : BigEndian;
  switch (machByteOrder) {
   case LittleEndian:
    {
      nanF.wordL.wordLuint = 0xFFC00000U;
      break;
    }

   case BigEndian:
    {
      nanF.wordL.wordLuint = 0x7FFFFFFFU;
      break;
    }
  }

  return nanF.wordL.wordLreal;
}

/*
 * Initialize rtInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
float rtGetInf(void)
{
  size_t bitsPerReal = sizeof(float) * (NumBitsPerChar);
  float inf = 0.0;
  if (bitsPerReal == 32U) {
    inf = rtGetInfF();
  } else {
    uint16_t one = 1U;
    enum {
      LittleEndian,
      BigEndian
    } machByteOrder = (*((uint8_t *) &one) == 1U) ? LittleEndian : BigEndian;
    switch (machByteOrder) {
     case LittleEndian:
      {
        union {
          LittleEndianIEEEDouble bitVal;
          float fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0x7FF00000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        inf = tmpVal.fltVal;
        break;
      }

     case BigEndian:
      {
        union {
          BigEndianIEEEDouble bitVal;
          float fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0x7FF00000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        inf = tmpVal.fltVal;
        break;
      }
    }
  }

  return inf;
}

/*
 * Initialize rtInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
float rtGetInfF(void)
{
  IEEESingle infF;
  infF.wordL.wordLuint = 0x7F800000U;
  return infF.wordL.wordLreal;
}

/*
 * Initialize rtMinusInf needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
float rtGetMinusInf(void)
{
  size_t bitsPerReal = sizeof(float) * (NumBitsPerChar);
  float minf = 0.0;
  if (bitsPerReal == 32U) {
    minf = rtGetMinusInfF();
  } else {
    uint16_t one = 1U;
    enum {
      LittleEndian,
      BigEndian
    } machByteOrder = (*((uint8_t *) &one) == 1U) ? LittleEndian : BigEndian;
    switch (machByteOrder) {
     case LittleEndian:
      {
        union {
          LittleEndianIEEEDouble bitVal;
          float fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0xFFF00000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        minf = tmpVal.fltVal;
        break;
      }

     case BigEndian:
      {
        union {
          BigEndianIEEEDouble bitVal;
          float fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0xFFF00000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        minf = tmpVal.fltVal;
        break;
      }
    }
  }

  return minf;
}

/*
 * Initialize rtMinusInfF needed by the generated code.
 * Inf is initialized as non-signaling. Assumes IEEE.
 */
float rtGetMinusInfF(void)
{
  IEEESingle minfF;
  minfF.wordL.wordLuint = 0xFF800000U;
  return minfF.wordL.wordLreal;
}

/*
 * Initialize the rtInf, rtMinusInf, and rtNaN needed by the
 * generated code. NaN is initialized as non-signaling. Assumes IEEE.
 */
void rt_InitInfAndNaN(size_t realSize)
{
  (void) (realSize);
  rtNaN = rtGetNaN();
  rtNaNF = rtGetNaNF();
  rtInf = rtGetInf();
  rtInfF = rtGetInfF();
  rtMinusInf = rtGetMinusInf();
  rtMinusInfF = rtGetMinusInfF();
}

/* Test if value is infinite */
uint8_t rtIsInf(float value)
{
  return (uint8_t)((value==rtInf || value==rtMinusInf) ? 1U : 0U);
}

/* Test if single-precision value is infinite */
uint8_t rtIsInfF(float value)
{
  return (uint8_t)(((value)==rtInfF || (value)==rtMinusInfF) ? 1U : 0U);
}

/* Test if value is not a number */
uint8_t rtIsNaN(float value)
{
  uint8_t result = (uint8_t) 0;
  size_t bitsPerReal = sizeof(float) * (NumBitsPerChar);
  if (bitsPerReal == 32U) {
    result = rtIsNaNF((float)value);
  } else {
    union {
      LittleEndianIEEEDouble bitVal;
      float fltVal;
    } tmpVal;

    tmpVal.fltVal = value;
    result = (uint8_t)((tmpVal.bitVal.words.wordH & 0x7FF00000) == 0x7FF00000 &&
                         ( (tmpVal.bitVal.words.wordH & 0x000FFFFF) != 0 ||
                          (tmpVal.bitVal.words.wordL != 0) ));
  }

  return result;
}

/* Test if single-precision value is not a number */
uint8_t rtIsNaNF(float value)
{
  IEEESingle tmp;
  tmp.wordL.wordLreal = value;
  return (uint8_t)( (tmp.wordL.wordLuint & 0x7F800000) == 0x7F800000 &&
                     (tmp.wordL.wordLuint & 0x007FFFFF) != 0 );
}

