/* libcyperus.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.o

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2015 murray foster */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fftw3.h>

#include "dsp_math.h"
#include "rtqueue.h"


#define MAX_FRAME_LENGTH 8192

void smbFft(float *fftBuffer, long fftFrameSize, long sign);
double smbAtan2(double x, double y);


float cyperus_block_processor(struct cyperus_parameters *block_processor, int jack_sr, int pos)
{
  int i;
  float outsample = block_processor->in;
  /*
  block_processor->signal_buffer[pos%1024] = block_processor->in;
  if(!(pos%1024))
    for(i=0; i<1024; i++)
      rtqueue_enq(&block_processor->block_fifo, block_processor->signal_buffer[i]);
  if(rtqueue_isempty(&block_processor->block_fifo) == 0)
    outsample = rtqueue_deq(&block_processor->block_fifo);
  */
  return outsample;
}

float cyperus_envelope_follower(struct cyperus_parameters *envelope_follower, int jack_sr, int pos)
{
  float attack_ms = envelope_follower->attack;
  float decay_ms = envelope_follower->decay;
  float coeff_attack = exp(log(0.01) / (attack_ms * jack_sr * 0.001));
  float coeff_decay = exp(log(0.01) / (decay_ms * jack_sr * 0.001));
  
  float insample = envelope_follower->in;
  float outsample, absin = 0.0;
  
  absin = fabs(insample);
  if(absin > envelope_follower->signal_buffer[0])
    outsample = coeff_attack * (outsample - absin) + outsample;
  else
    outsample = coeff_decay * (outsample - absin) + outsample;
  envelope_follower->signal_buffer[0] = outsample;
  return fabs(outsample);
}

float cyperus_sine(struct cyperus_parameters *sinewav, int jack_sr, int pos)
{
  sinewav->phase_delta += 2.0 * M_PI * sinewav->freq * (1.0/jack_sr) + sinewav->phase;
  while( sinewav->phase_delta > 2.0 * M_PI ) sinewav->phase_delta -= 2 * M_PI;
  return sin(sinewav->phase_delta) * sinewav->amp;
}

float cyperus_square(struct cyperus_parameters *squarewav, int jack_sr, int pos) {
  return sin(squarewav->freq * (2 * M_PI) * pos / jack_sr) >= 0 ? squarewav->amp : -1 * squarewav->amp;
}

float cyperus_triangle(struct cyperus_parameters *triwav, int jack_sr, int pos) {
  float period = jack_sr/triwav->freq;
  return 2 * triwav->amp / M_PI * asinf( sinf( (2 * M_PI / period) * pos ) );
}

float cyperus_whitenoise(struct cyperus_parameters *noiz, int jack_sr, int pos) {
  if( pos == jack_sr )
    srand(time(NULL));
  return (float)rand()/RAND_MAX;
}

const float A[] = { 0.02109238, 0.07113478, 0.68873558 }; // rescaled by (1+P)/(1-P)
const float P[] = { 0.3190, 0.7756, 0.9613 };

float cyperus_pinknoise(struct cyperus_parameters *noiz, int jack_sr, int pos) {
  /* by Evan Buswell */
  
  static const float RMI2 = 2.0 / ((float) RAND_MAX);
  static const float offset = 0.02109238 + 0.07113478 + 0.68873558;
  
  float temp = (float) random();
  noiz->state0 = P[0] * (noiz->state0 - temp) + temp;
  temp = (float) random();
  noiz->state1 = P[1] * (noiz->state1 - temp) + temp;
  temp = (float) random();
  noiz->state2 = P[2] * (noiz->state2 - temp) + temp;
  return ((A[0] * noiz->state0 + A[1] * noiz->state1 + A[2] * noiz->state2) * RMI2 - offset);
}


float cyperus_karlsen_lowpass(struct cyperus_parameters *filtr, int jack_sr, int pos) {
  /* by Ove Karlsen, 24dB 4-pole lowpass */

  /* need to scale the input cutoff to stable parameters frequencies */
  /* sweet spot for parameters->freq is up to 0-0.7/8, in radians of a circle */
  
  float sampleout = 0.0;
  filtr->tempval = filtr->lastoutval3; if (filtr->tempval > 1) {filtr->tempval = 1;}
  filtr->in = (-filtr->tempval * filtr->res) + filtr->in;
  filtr->lastoutval = ((-filtr->lastoutval + filtr->in) * filtr->freq) + filtr->lastoutval;
  filtr->lastoutval1 = ((-filtr->lastoutval1 + filtr->lastoutval) * filtr->freq) + filtr->lastoutval1;
  filtr->lastoutval2 = ((-filtr->lastoutval2 + filtr->lastoutval1) * filtr->freq) + filtr->lastoutval2;
  filtr->lastoutval3 = ((-filtr->lastoutval3 + filtr->lastoutval2) * filtr->freq) + filtr->lastoutval3;
  sampleout = filtr->lastoutval3;
  
  return sampleout;
}

float cyperus_butterworth_biquad_lowpass(struct cyperus_parameters *filtr, int jack_sr, int pos) {
  /* by Patrice Tarrabia */

  /* need to scale inputs! */
  /* sweet spot for filtr->freq is 100-300(?)Hz, rez can be from
       sqrt(2) to ~0.1, filtr->freq 0Hz to samplerate/2  */
  
  float outsample = 0.0;
  float c = 1.0 / tan(M_PI * filtr->freq / jack_sr);
  float a1 = 1.0 / (1.0 + filtr->res * c + c * c);
  float a2 = 2 * a1;
  float a3 = a1;
  float b1 = 2.0 * (1.0 - c*c) * a1;
  float b2 = (1.0 - filtr->res * c + c * c) * a1;

  outsample = a1 * filtr->in + a2 * filtr->lastinval + a3 * filtr->lastinval1 - b1 * filtr->lastoutval - b2 * filtr->lastoutval1;
  
  filtr->lastoutval1 = filtr->lastoutval;
  filtr->lastoutval = outsample;
  filtr->lastinval1 = filtr->lastinval;
  filtr->lastinval = filtr->in;

  return outsample;
}

float cyperus_butterworth_biquad_hipass(struct cyperus_parameters *filtr, int jack_sr, int pos) {
  /* by Patrice Tarrabia */
  float outsample = 0.0;
  float c = tan(M_PI * filtr->freq / jack_sr);
  float a1 = 1.0 / (1.0 + filtr->res * c + c * c);
  float a2 = -2*a1;
  float a3 = a1;
  float b1 = 2.0 * ( c * c - 1.0) * a1;
  float b2 = ( 1.0 - filtr->res * c + c * c) * a1;
  
  outsample = a1 * filtr->in + a2 * filtr->lastinval + a3 * filtr->lastinval1 - b1 * filtr->lastoutval - b2 * filtr->lastoutval1;
  
  filtr->lastoutval1 = filtr->lastoutval;
  filtr->lastoutval = outsample;
  filtr->lastinval1 = filtr->lastinval;
  filtr->lastinval = filtr->in;

  return outsample;
}

float cyperus_moog_vcf(struct cyperus_parameters *filtr, int jack_sr, int pos) {
  /* by Stilson/Smith CCRMA paper, Timo Tossavainen */

  double f = filtr->freq * 1.16;
  double fb = filtr->res * (1.0 - 0.15 * f * f);
  
  filtr->in -= filtr->lastoutval3 * fb;
  filtr->in *= 0.35013 * (f*f) * (f*f);
  filtr->lastoutval = filtr->in + 0.3 * filtr->lastinval + (1 - f) * filtr->lastoutval;
  filtr->lastinval = filtr->in;
  filtr->lastoutval1 = filtr->lastoutval + 0.3 * filtr->lastinval1 + (1 - f) * filtr->lastoutval1;
  filtr->lastinval1 = filtr->lastoutval;
  filtr->lastoutval2 = filtr->lastoutval1 + 0.3 * filtr->lastinval2 + (1 - f) * filtr->lastoutval2;
  filtr->lastinval2 = filtr->lastoutval1;
  filtr->lastoutval3 = filtr->lastoutval2 + 0.3 * filtr->lastinval3 + (1 - f) * filtr->lastoutval3;
  filtr->lastinval3 = filtr->lastoutval2;
  return filtr->lastoutval3;
}

float cyperus_delay(struct cyperus_parameters *delay, int jack_sr, int pos) {
  float outsample = 0.0;

  
  if( delay->pos >= delay->delay_time )
    delay->pos = 0;

  delay->delay_pos = delay->pos - delay->delay_time;

  if( delay->delay_pos < 0 )
    delay->delay_pos += delay->delay_time;

  outsample = delay->signal_buffer[delay->pos] = delay->in + (delay->signal_buffer[delay->delay_pos] * delay->fb);
  delay->pos += 1;

  return outsample * delay->delay_amt;
}

void smbPitchShift(float pitchShift, long numSampsToProcess, long fftFrameSize, long osamp, float sampleRate, float *indata, float *outdata)
/*
	Routine smbPitchShift(). See top of file for explanation
	Purpose: doing pitch shifting while maintaining duration using the Short
	Time Fourier Transform.
	Author: (c)1999-2015 Stephan M. Bernsee <s.bernsee [AT] zynaptiq [DOT] com>
*/
{

	static float gInFIFO[MAX_FRAME_LENGTH];
	static float gOutFIFO[MAX_FRAME_LENGTH];
	static float gFFTworksp[2*MAX_FRAME_LENGTH];
	static float gLastPhase[MAX_FRAME_LENGTH/2+1];
	static float gSumPhase[MAX_FRAME_LENGTH/2+1];
	static float gOutputAccum[2*MAX_FRAME_LENGTH];
	static float gAnaFreq[MAX_FRAME_LENGTH];
	static float gAnaMagn[MAX_FRAME_LENGTH];
	static float gSynFreq[MAX_FRAME_LENGTH];
	static float gSynMagn[MAX_FRAME_LENGTH];
	static long gRover = 0, gInit = 0;
	double magn, phase, tmp, window, real, imag;
	double freqPerBin, expct;
	long i,k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

	/* set up some handy variables */
	fftFrameSize2 = fftFrameSize/2;
	stepSize = fftFrameSize/osamp;
	freqPerBin = sampleRate/(double)fftFrameSize;
	expct = 2.*M_PI*(double)stepSize/(double)fftFrameSize;
	inFifoLatency = fftFrameSize-stepSize;
	if (gRover == 0) gRover = inFifoLatency;

	/* initialize our static arrays */
	if (gInit == 0) {
		memset(gInFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gOutFIFO, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gFFTworksp, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(gLastPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
		memset(gSumPhase, 0, (MAX_FRAME_LENGTH/2+1)*sizeof(float));
		memset(gOutputAccum, 0, 2*MAX_FRAME_LENGTH*sizeof(float));
		memset(gAnaFreq, 0, MAX_FRAME_LENGTH*sizeof(float));
		memset(gAnaMagn, 0, MAX_FRAME_LENGTH*sizeof(float));
		gInit = 1;
	}

	/* main processing loop */
	for (i = 0; i < numSampsToProcess; i++){

		/* As long as we have not yet collected enough data just read in */
		gInFIFO[gRover] = indata[i];
		outdata[i] = gOutFIFO[gRover-inFifoLatency];
		gRover++;

		/* now we have enough data for processing */
		if (gRover >= fftFrameSize) {
			gRover = inFifoLatency;

			/* do windowing and re,im interleave */
			for (k = 0; k < fftFrameSize;k++) {
				window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
				gFFTworksp[2*k] = gInFIFO[k] * window;
				gFFTworksp[2*k+1] = 0.;
			}


			/* ***************** ANALYSIS ******************* */
			/* do transform */
			smbFft(gFFTworksp, fftFrameSize, -1);

			/* this is the analysis step */
			for (k = 0; k <= fftFrameSize2; k++) {

				/* de-interlace FFT buffer */
				real = gFFTworksp[2*k];
				imag = gFFTworksp[2*k+1];

				/* compute magnitude and phase */
				magn = 2.*sqrt(real*real + imag*imag);
				phase = smbAtan2(imag,real);

				/* compute phase difference */
				tmp = phase - gLastPhase[k];
				gLastPhase[k] = phase;

				/* subtract expected phase difference */
				tmp -= (double)k*expct;

				/* map delta phase into +/- Pi interval */
				qpd = tmp/M_PI;
				if (qpd >= 0) qpd += qpd&1;
				else qpd -= qpd&1;
				tmp -= M_PI*(double)qpd;

				/* get deviation from bin frequency from the +/- Pi interval */
				tmp = osamp*tmp/(2.*M_PI);

				/* compute the k-th partials' true frequency */
				tmp = (double)k*freqPerBin + tmp*freqPerBin;

				/* store magnitude and true frequency in analysis arrays */
				gAnaMagn[k] = magn;
				gAnaFreq[k] = tmp;

			}

			/* ***************** PROCESSING ******************* */
			/* this does the actual pitch shifting */
			memset(gSynMagn, 0, fftFrameSize*sizeof(float));
			memset(gSynFreq, 0, fftFrameSize*sizeof(float));
			for (k = 0; k <= fftFrameSize2; k++) { 
				index = k*pitchShift;
				if (index <= fftFrameSize2) { 
					gSynMagn[index] += gAnaMagn[k]; 
					gSynFreq[index] = gAnaFreq[k] * pitchShift; 
				} 
			}
			
			/* ***************** SYNTHESIS ******************* */
			/* this is the synthesis step */
			for (k = 0; k <= fftFrameSize2; k++) {

				/* get magnitude and true frequency from synthesis arrays */
				magn = gSynMagn[k];
				tmp = gSynFreq[k];

				/* subtract bin mid frequency */
				tmp -= (double)k*freqPerBin;

				/* get bin deviation from freq deviation */
				tmp /= freqPerBin;

				/* take osamp into account */
				tmp = 2.*M_PI*tmp/osamp;

				/* add the overlap phase advance back in */
				tmp += (double)k*expct;

				/* accumulate delta phase to get bin phase */
				gSumPhase[k] += tmp;
				phase = gSumPhase[k];

				/* get real and imag part and re-interleave */
				gFFTworksp[2*k] = magn*cos(phase);
				gFFTworksp[2*k+1] = magn*sin(phase);
			} 

			/* zero negative frequencies */
			for (k = fftFrameSize+2; k < 2*fftFrameSize; k++) gFFTworksp[k] = 0.;

			/* do inverse transform */
			smbFft(gFFTworksp, fftFrameSize, 1);

			/* do windowing and add to output accumulator */ 
			for(k=0; k < fftFrameSize; k++) {
				window = -.5*cos(2.*M_PI*(double)k/(double)fftFrameSize)+.5;
				gOutputAccum[k] += 2.*window*gFFTworksp[2*k]/(fftFrameSize2*osamp);
			}
			for (k = 0; k < stepSize; k++) gOutFIFO[k] = gOutputAccum[k];

			/* shift accumulator */
			memmove(gOutputAccum, gOutputAccum+stepSize, fftFrameSize*sizeof(float));

			/* move input FIFO */
			for (k = 0; k < inFifoLatency; k++) gInFIFO[k] = gInFIFO[k+stepSize];
		}
	}
}

// -----------------------------------------------------------------------------------------------------------------


void smbFft(float *fftBuffer, long fftFrameSize, long sign)
/* 
	FFT routine, (C)1996 S.M.Bernsee. Sign = -1 is FFT, 1 is iFFT (inverse)
	Fills fftBuffer[0...2*fftFrameSize-1] with the Fourier transform of the
	time domain data in fftBuffer[0...2*fftFrameSize-1]. The FFT array takes
	and returns the cosine and sine parts in an interleaved manner, ie.
	fftBuffer[0] = cosPart[0], fftBuffer[1] = sinPart[0], asf. fftFrameSize
	must be a power of 2. It expects a complex input signal (see footnote 2),
	ie. when working with 'common' audio signals our input signal has to be
	passed as {in[0],0.,in[1],0.,in[2],0.,...} asf. In that case, the transform
	of the frequencies of interest is in fftBuffer[0...fftFrameSize].
*/
{
	float wr, wi, arg, *p1, *p2, temp;
	float tr, ti, ur, ui, *p1r, *p1i, *p2r, *p2i;
	long i, bitm, j, le, le2, k;

	for (i = 2; i < 2*fftFrameSize-2; i += 2) {
		for (bitm = 2, j = 0; bitm < 2*fftFrameSize; bitm <<= 1) {
			if (i & bitm) j++;
			j <<= 1;
		}
		if (i < j) {
			p1 = fftBuffer+i; p2 = fftBuffer+j;
			temp = *p1; *(p1++) = *p2;
			*(p2++) = temp; temp = *p1;
			*p1 = *p2; *p2 = temp;
		}
	}
	for (k = 0, le = 2; k < (long)(log(fftFrameSize)/log(2.)+.5); k++) {
		le <<= 1;
		le2 = le>>1;
		ur = 1.0;
		ui = 0.0;
		arg = M_PI / (le2>>1);
		wr = cos(arg);
		wi = sign*sin(arg);
		for (j = 0; j < le2; j += 2) {
			p1r = fftBuffer+j; p1i = p1r+1;
			p2r = p1r+le2; p2i = p2r+1;
			for (i = j; i < 2*fftFrameSize; i += le) {
				tr = *p2r * ur - *p2i * ui;
				ti = *p2r * ui + *p2i * ur;
				*p2r = *p1r - tr; *p2i = *p1i - ti;
				*p1r += tr; *p1i += ti;
				p1r += le; p1i += le;
				p2r += le; p2i += le;
			}
			tr = ur*wr - ui*wi;
			ui = ur*wi + ui*wr;
			ur = tr;
		}
	}
}


// -----------------------------------------------------------------------------------------------------------------

/*

    12/12/02, smb
    
    PLEASE NOTE:
    
    There have been some reports on domain errors when the atan2() function was used
    as in the above code. Usually, a domain error should not interrupt the program flow
    (maybe except in Debug mode) but rather be handled "silently" and a global variable
    should be set according to this error. However, on some occasions people ran into
    this kind of scenario, so a replacement atan2() function is provided here.
    
    If you are experiencing domain errors and your program stops, simply replace all
    instances of atan2() with calls to the smbAtan2() function below.
    
*/

double smbAtan2(double x, double y)
{
  double signx;
  if (x > 0.) signx = 1.;  
  else signx = -1.;
  
  if (x == 0.) return 0.;
  if (y == 0.) return signx * M_PI / 2.;
  
  return atan2(x, y);
}


float cyperus_pitch_shift(struct cyperus_parameters *pitch_shift, int jack_sr, int pos)
{
  int i, count;
  float sample = 0.0;

  long total_samples = 1024;
  long frame_size = 1024;
  long osamp = 32;

  float samples_in[2048] = {0.};
  float samples_out[2048]  = {0.};

  float outsample = pitch_shift->in;
  if((pos%total_samples) == 0)
    {
      for(i=0; i<total_samples; i++)
	samples_in[i] = pitch_shift->signal_buffer[i];
      smbPitchShift(pitch_shift->shift, total_samples, frame_size, osamp, jack_sr, samples_in, samples_out);
      for(i=0; i<total_samples; i++)
	rtqueue_enq(&pitch_shift->block_fifo, samples_out[i]);
    }
  else
    pitch_shift->signal_buffer[pos%total_samples] = pitch_shift->in;
  if(rtqueue_isempty(&pitch_shift->block_fifo) == 0)
    outsample = rtqueue_deq(&pitch_shift->block_fifo);
  
  return outsample;
}
