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

#include "dsp_types.h"
#include "dsp_math.h"

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

float _cyperus_level_detector_coeff(int jack_sr, float duration) {
  return exp(log(0.01f) / (duration * jack_sr * 0.001f));
}

float cyperus_level_detector(struct cyperus_parameters *level_detector, int jack_sr, int pos) {
  float attack_ms = level_detector->attack;
  float decay_ms = level_detector->decay;

  float pulse_attack_ms = level_detector->x0;
  float pulse_decay_ms = level_detector->x1;
  
  float coeff_attack = _cyperus_level_detector_coeff(jack_sr, attack_ms);
  float coeff_decay = _cyperus_level_detector_coeff(jack_sr, decay_ms);
  
  float coeff_pulse_attack = _cyperus_level_detector_coeff(jack_sr, attack_ms);
  float coeff_pulse_decay = _cyperus_level_detector_coeff(jack_sr, decay_ms);

  float output_level = level_detector->x2;
  
  float insample = level_detector->in;
  float outsample, absin = 0.0f;
  
  absin = fabs(insample);
  if(absin > level_detector->signal_buffer[0])
    outsample = coeff_attack * (outsample - absin) + outsample;
  else
    outsample = coeff_decay * (outsample - absin) + outsample;
  level_detector->signal_buffer[0] = outsample;
  
  if( level_detector->amt == outsample ) {
    level_detector->signal_buffer[1] = output_level;
  }
  if(absin > level_detector->signal_buffer[1])
    outsample = coeff_pulse_attack * level_detector->signal_buffer[1];
  else
    outsample = coeff_pulse_decay * level_detector->signal_buffer[1];
  level_detector->signal_buffer[1] = outsample;
  
  return outsample;
}

float cyperus_sawtooth(struct cyperus_parameters *sawtoothwav, int jack_sr, int pos) {
  float  *sin_val, *cos_val = NULL;
  sincosf(pos * M_PI / (jack_sr / sawtoothwav->freq), sin_val, cos_val);
  return (2.0f * sawtoothwav->amp) / M_PI * atanf( (*cos_val / *sin_val) );
}

float cyperus_triangle(struct cyperus_parameters *triwav, int jack_sr, int pos) {
  return 2 * triwav->amp / M_PI * asinf( sinf( (2 * M_PI / (jack_sr/triwav->freq)) * pos ) );
}

float cyperus_whitenoise(struct cyperus_parameters *noise, int jack_sr, int pos) {
  if( pos == jack_sr )
    srand(time(NULL));
  return (float)rand()/RAND_MAX;
}

const float A[] = { 0.02109238f, 0.07113478f, 0.68873558f }; // rescaled by (1+P)/(1-P)
const float P[] = { 0.3190f, 0.7756f, 0.9613f };

float cyperus_pinknoise(struct cyperus_parameters *noise, int jack_sr, int pos) {
  /* by Evan Buswell */
  
  static const float RMI2 = 2.0f / ((float) RAND_MAX);
  static const float offset = 0.02109238f + 0.07113478f + 0.68873558f;
  
  float temp = (float) random();
  noise->state0 = P[0] * (noise->state0 - temp) + temp;
  temp = (float) random();
  noise->state1 = P[1] * (noise->state1 - temp) + temp;
  temp = (float) random();
  noise->state2 = P[2] * (noise->state2 - temp) + temp;
  return ((A[0] * noise->state0 + A[1] * noise->state1 + A[2] * noise->state2) * RMI2 - offset);
}

void _filter_freq_reset(struct cyperus_parameters *filter, int jack_sr, int low) {
  float freq;

  if( filter->freq < 0)
    freq = 0;
  else
    freq = filter->freq;

  filter->x0 = freq;
  if( low )
    filter->x1 = freq * (2 * 3.14159f) / jack_sr;
  else
    filter->x1 = 1 - freq * (2 * 3.14159f) / jack_sr;

  if( low ) {
    if( filter->x1 > 1)
      filter->x1 = 1;
    else
      if( filter->x1 < 0 )
        filter->x1 = 0;
  } else {
    if( filter->x1 < 0 )
      filter->x1 = 0;
    else
      if( filter->x1 > 1 )
        filter->x1 = 1;
    filter->state0 = 0.5f * (1+filter->x1);
  }
  filter->x2 = 0.0f;
  
} /* _filter_freq_reset */

void cyperus_highpass_init(struct cyperus_parameters *filter, int jack_sr) {

  _filter_freq_reset(filter, jack_sr, 0);
  
} /* cyperus_highpass_init */

float cyperus_highpass(struct cyperus_parameters *filter, int jack_sr, int pos) {
  float freq;

  if( filter->freq < 0)
    freq = 0;
  else
    freq = filter->freq;
  
  filter->x0 = freq;
  filter->x1 = 1 - freq * (2 * 3.14159f) / jack_sr;

  float tempsample = 0.0f;
  float outsample = 0.0f;
  float last = filter->x2;
  float coef = filter->x1;

  if( coef < 1) {
    tempsample = filter->in + coef * last;
    outsample = filter->state0 * (tempsample - last);
    last = tempsample;
    filter->x2 = last;
  } else {
    outsample = filter->in;
    filter->x2 = 0;
  }

  return outsample * filter->amt;
} /* cyperus_highpass */

static float _bandpass_qcos(float f)
{
    if (f >= -(0.5f*3.14159f) && f <= 0.5f*3.14159f)
    {
        float g = f*f;
        return (((g*g*g * (-1.0f/720.0f) + g*g*(1.0f/24.0f)) - g*0.5f) + 1);
    }
    else return (0);
}

void cyperus_bandpass_init(struct cyperus_parameters *filter, int jack_sr) {

  float freq = filter->freq;
  float q = filter->q;

  float last = 0.0f;
  float prev = 0.0f;
  float coef1 = 0.0f;
  float coef2 = 0.0f;
  
  float r, oneminusr, omega;
  if (freq < 0.001f) freq = 10;
  if (q < 0) q = 0;
  filter->x0 = freq;
  filter->x1 = q;
  omega = freq * (2.0f * 3.14159f) / jack_sr;
  if (q < 0.001) oneminusr = 1.0f;
  else oneminusr = omega/q;
  if (oneminusr > 1.0f) oneminusr = 1.0f;
  r = 1.0f - oneminusr;
  filter->state0 = 2.0f * _bandpass_qcos(omega) * r;
  filter->state1 = - r * r;
  filter->state2  = 2 * oneminusr * (oneminusr + r * omega);
  /* post("r %f, omega %f, coef1 %f, coef2 %f",
     r, omega, x->x_ctl->c_coef1, x->x_ctl->c_coef2); */

  filter->x3 = 0.0f;
  filter->x4 = 0.0f;
  
} /* cyperus_bandpass_init */

float cyperus_bandpass(struct cyperus_parameters *filter, int jack_sr, int pos) {
  float freq;
  float q;
  float output, outsample;


  if( filter->freq != filter->x0 ||
      filter->q != filter->x1 ) { 
    float r, oneminusr, omega;
    
    freq = filter->freq;
    q = filter->q;
   
    if (freq < 0.001f) freq = 10;
    if (q < 0) q = 0;
    filter->x0 = freq;
    filter->x1 = q;
    omega = freq * (2.0f * 3.14159f) / jack_sr;
    if (q < 0.001) oneminusr = 1.0f;
    else oneminusr = omega/q;
    if (oneminusr > 1.0f) oneminusr = 1.0f;
    r = 1.0f - oneminusr;
    filter->state0 = 2.0f * _bandpass_qcos(omega) * r;
    filter->state1 = - r * r;
    filter->state2  = 2 * oneminusr * (oneminusr + r * omega);

    filter->freq = freq;
    filter->q = q;
  }
  float last = filter->x3;
  float prev = filter->x4;
  float coef1 = filter->state0;;
  float coef2 = filter->state1;
  float gain = filter->state2;
  
  output =  filter->in + coef1 * last + coef2 * prev;
  outsample = output * filter->amt * gain;
  prev = last;
  last = output;
  
  filter->x3 = last;
  filter->x4 = prev;
  
  return outsample;
} /* cyperus_bandpass */


float cyperus_karlsen_lowpass(struct cyperus_parameters *filter, int jack_sr, int pos) {
  /* by Ove Karlsen, 24dB 4-pole lowpass */

  /* need to scale the input cutoff to stable parameters frequencies */
  /* sweet spot for parameters->freq is up to 0-0.7/8, in radians of a circle */
  
  float sampleout = 0.0f;
  filter->tempval = filter->lastoutval3; if (filter->tempval > 1) {filter->tempval = 1;}
  filter->in = (-filter->tempval * filter->res) + filter->in;
  filter->lastoutval = ((-filter->lastoutval + filter->in) * filter->freq) + filter->lastoutval;
  filter->lastoutval1 = ((-filter->lastoutval1 + filter->lastoutval) * filter->freq) + filter->lastoutval1;
  filter->lastoutval2 = ((-filter->lastoutval2 + filter->lastoutval1) * filter->freq) + filter->lastoutval2;
  filter->lastoutval3 = ((-filter->lastoutval3 + filter->lastoutval2) * filter->freq) + filter->lastoutval3;
  sampleout = filter->lastoutval3;
  
  return sampleout;
}

float cyperus_butterworth_biquad_lowpass(struct cyperus_parameters *filter, int jack_sr, int pos) {
  /* by Patrice Tarrabia */

  /* need to scale inputs! */
  /* sweet spot for filter->freq is 100-300(?)Hz, rez can be from
       sqrt(2) to ~0.1, filter->freq 0Hz to samplerate/2  */
  
  float outsample = 0.0f;
  float c = 1.0f / tan(M_PI * filter->freq / jack_sr);
  float a1 = 1.0f / (1.0f + filter->res * c + c * c);
  float a2 = 2 * a1;
  float a3 = a1;
  float b1 = 2.0f * (1.0f - c*c) * a1;
  float b2 = (1.0f - filter->res * c + c * c) * a1;
  outsample = a1 * filter->in + a2 * filter->lastinval + a3 * filter->lastinval1 - b1 * filter->lastoutval - b2 * filter->lastoutval1;
  
  filter->lastoutval1 = filter->lastoutval;
  filter->lastoutval = outsample;
  filter->lastinval1 = filter->lastinval;
  filter->lastinval = filter->in;
  
  return outsample;
}

float cyperus_butterworth_biquad_hipass(struct cyperus_parameters *filter, int jack_sr, int pos) {
  /* by Patrice Tarrabia */
  float outsample = 0.0f;
  float c = tan(M_PI * filter->freq / jack_sr);
  float a1 = 1.0f / (1.0f + filter->res * c + c * c);
  float a2 = -2*a1;
  float a3 = a1;
  float b1 = 2.0f * ( c * c - 1.0f) * a1;
  float b2 = ( 1.0f - filter->res * c + c * c) * a1;
  
  outsample = a1 * filter->in + a2 * filter->lastinval + a3 * filter->lastinval1 - b1 * filter->lastoutval - b2 * filter->lastoutval1;
  
  filter->lastoutval1 = filter->lastoutval;
  filter->lastoutval = outsample;
  filter->lastinval1 = filter->lastinval;
  filter->lastinval = filter->in;

  return outsample;
}

float cyperus_apple_biquad_lowpass(struct cyperus_parameters *filter, int jack_sr, int pos) {
  /* moc.liamg@321tiloen 
     Simple Biquad LP filter from the AU tutorial at apple.com

     cutoff_slider range 20-20000hz
     res_slider range -25/25db
     srate - sample rate
  */
  float outsample = 0.0f;

  float pi = 22.0f/7;

  //coefficients
  float cutoff = filter->freq;
  float res = filter->res;

  cutoff = 2 * filter->freq / jack_sr;
  res = pow(10, 0.05f * -filter->res);
  float k = 0.5f * res * sin(pi * cutoff);
  float c1 = 0.5f * (1 - k) / (1 + k);
  float c2 = (0.5f + c1) * cos(pi * cutoff);
  float c3 = (0.5f + c1 - c2) * 0.25f;

  float mA0 = 2 * c3;
  float mA1 = 2 * 2 * c3;
  float mA2 = 2 * c3;
  float mB1 = 2 * -c2;
  float mB2 = 2 * c1;

  //loop
  outsample = mA0*filter->in + mA1*filter->state0 + mA2*filter->state1 - mB1*filter->state2 - mB2*filter->state3;

  filter->state1 = filter->state0;
  filter->state0 = filter->in;
  filter->state3 = filter->state2;
  filter->state2 = outsample;

  return outsample * filter->amp;
} /* cyperus_apple_biquad_lowpass */

float cyperus_moog_vcf(struct cyperus_parameters *filter, int jack_sr, int pos) {
  /* by Stilson/Smith CCRMA paper, Timo Tossavainen */

  double f = filter->freq * 1.16f;
  double fb = filter->res * (1.0f - 0.15f * f * f);
  
  filter->in -= filter->lastoutval3 * fb;
  filter->in *= 0.35013f * (f*f) * (f*f);
  filter->lastoutval = filter->in + 0.3f * filter->lastinval + (1 - f) * filter->lastoutval;
  filter->lastinval = filter->in;
  filter->lastoutval1 = filter->lastoutval + 0.3f * filter->lastinval1 + (1 - f) * filter->lastoutval1;
  filter->lastinval1 = filter->lastoutval;
  filter->lastoutval2 = filter->lastoutval1 + 0.3f * filter->lastinval2 + (1 - f) * filter->lastoutval2;
  filter->lastinval2 = filter->lastoutval1;
  filter->lastoutval3 = filter->lastoutval2 + 0.3f * filter->lastinval3 + (1 - f) * filter->lastoutval3;
  filter->lastinval3 = filter->lastoutval2;
  return filter->lastoutval3;
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
