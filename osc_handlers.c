/* osc_handlers.c
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

Copyright 2015 murray foster */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);

#include "cyperus.h"
#include "rtqueue.h"
#include "libcyperus.h"
#include "dsp.h"
#include "dsp_types.h"
#include "dsp_ops.h"
#include "jackcli.h"
#include "osc.h"
#include "osc_handlers.h"

char* concat(char *str_prefix, char *str_suffix) {
  char *full_str;  
  full_str = (char*)malloc(sizeof(char)*(strlen(str_suffix)+strlen(str_prefix)+1));
  snprintf(full_str, strlen(str_suffix)+strlen(str_prefix)+1, "%s%s", str_prefix, str_suffix);
  return full_str;
}

void osc_error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}

/* for debugging,
 * catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
  int i;
  
  printf("path: <%s>\n", path);
  for (i = 0; i < argc; i++) {
    printf("arg %d '%c' ", i, types[i]);
    lo_arg_pp((lo_type)types[i], argv[i]);
    printf("\n");
  }
  return 0;
}

int osc_list_mains_handler(const char *path, const char *types, lo_arg ** argv,
			   int argc, void *data, void *user_data)
{
  struct dsp_port_out *temp_port_out;
  struct dsp_port_in *temp_port_in;
  char *mains_str = malloc(sizeof(char) * ((44 * (jackcli_channels_in +
						  jackcli_channels_out)) +
					   4 + /* strlen("in:\n") */
					   5 + /* strlen("out:\n") */
					   1));
  strcpy(mains_str, "in:\n");
  /* process main inputs */
  temp_port_out = dsp_main_ins;
  while(temp_port_out != NULL) {
    strcat(mains_str, "/mains{");
    strcat(mains_str, temp_port_out->id);
    strcat(mains_str, "\n");
    temp_port_out = temp_port_out->next;
  }

  strcat(mains_str, "out:\n");
  /* process main outputs */
  temp_port_in = dsp_main_outs;
  while(temp_port_in != NULL) {
    strcat(mains_str, "/mains}");
    strcat(mains_str, temp_port_in->id);
    strcat(mains_str, "\n");
    temp_port_in = temp_port_in->next;
  }

  lo_send(lo_addr_send,"/cyperus/list/mains", "s", mains_str);
  
  return 0;
} /* osc_remove_module_handler */

int osc_remove_module_handler(const char *path, const char *types, lo_arg ** argv,
			      int argc, void *data, void *user_data)
{
  int voice;
  int module_no;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;

  printf("removing module #%d..\n",module_no);
  
  dsp_remove_module(0,module_no);
  
  return 0;
} /* osc_remove_module_handler */

int osc_list_modules_handler(const char *path, const char *types, lo_arg ** argv,
			     int argc, void *data, void *user_data)
{
  int voice_no;
  char *module_list;
  char return_string[100];
  
  printf("path: <%s>\n", path);
  voice_no=argv[0]->i;

  module_list = dsp_list_modules(voice_no);
  
  printf("listing modules for voice #%d..\n",voice_no);

  lo_send(lo_addr_send,"/cyperus/list","s",module_list);

  free(module_list);
  return 0;
  
} /* osc_list_modules_handler */

int osc_add_sine_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  float freq;
  float amp;
  float phase;
  
  printf("path: <%s>\n", path);
  freq=argv[0]->f;
  amp=argv[1]->f;
  phase=argv[2]->f;

  printf("creating sine wave at freq %f and amp %f..\n",freq,amp);
  
  /* add sine() function from libcyperus onto correct voice/signal chain */
  dsp_create_sine(freq,amp,phase);
  
  return 0;
} /* osc_create_sine_handler */

int osc_edit_sine_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  int module_no;
  float freq;
  float amp;
  float phase;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;
  freq=argv[1]->f;
  amp=argv[2]->f;
  phase=argv[3]->f;
  
  printf("module_no %d, editing sine wave to freq %f and amp %f..\n",module_no,freq,amp);
  
  /* add sine() function from libcyperus onto correct voice/signal chain */
  dsp_edit_sine(module_no,freq,amp,phase);
  
  return 0;
} /* osc_edit_sine_handler */

int osc_add_square_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  float freq;
  float amp;
  
  printf("path: <%s>\n", path);
  freq=argv[0]->f;
  amp=argv[1]->f;

  printf("creating square wave at freq %f and amp %f..\n",freq,amp);
  
  dsp_create_square(freq,amp);
  
  return 0;
} /* osc_create_square_handler */

int osc_edit_square_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  int module_no;
  float freq;
  float amp;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;
  freq=argv[1]->f;
  amp=argv[2]->f;

  printf("module_no %d, editing square wave to freq %f and amp %f..\n",module_no,freq,amp);
  
  dsp_edit_square(module_no,freq,amp);
  
  return 0;
} /* osc_edit_square_handler */

int osc_add_pinknoise_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  
  printf("path: <%s>\n", path);

  printf("creating pink noise..\n");
  
  /* add pinknoise() function from libcyperus onto correct voice/signal chain */
  dsp_create_pinknoise();
  
  return 0;
} /* osc_add_pinknoise_handler */


int osc_add_butterworth_biquad_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  float cutoff;
  float res;
  
  printf("path: <%s>\n", path);
  cutoff=argv[0]->f;
  res=argv[1]->f;

  printf("creating butterworth biquad lowpass filter at freq cutoff %f and resonance %f..\n",cutoff,res);
  
  dsp_create_butterworth_biquad_lowpass(cutoff,res);
  
  return 0;
} /* osc_create_butterworth_biquad_lowpass_handler */

int osc_edit_butterworth_biquad_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  int module_no;
  float cutoff;
  float res;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;
  cutoff=argv[1]->f;
  res=argv[2]->f;

  printf("module_no %d, editing butterworth biquad lowpass filter at cutoff freq %f and resonance %f..\n",module_no,cutoff,res);
  
  dsp_edit_butterworth_biquad_lowpass(module_no,cutoff,res);
  
  return 0;
} /* osc_edit_butterworth_biquad_lowpass_handler */

int osc_add_delay_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  float amt;
  float time;
  float feedback;
  
  printf("path: <%s>\n", path);
  amt=argv[0]->f;
  time=argv[1]->f;
  feedback=argv[2]->f;
  
  printf("creating delay with amount %f, time %f seconds, and feedback %f..\n",amt,time,feedback);
  
  dsp_create_delay(amt,time,feedback);
  
  return 0;
} /* osc_add_delay_handler */

int
osc_edit_delay_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  int module_no;
  float amt;
  float time;
  float feedback;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;
  amt=argv[1]->f;
  time=argv[2]->f;
  feedback=argv[3]->f;
  
  printf("module_no %d, editing delay of amount %f, time %f seconds, and feedback %f..\n",module_no,amt,time,feedback);
  
  dsp_edit_delay(module_no,amt,time,feedback);
  
  return 0;
} /* osc_edit_delay_handler */

int osc_add_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
				int argc, void *data, void *user_data)
{
  float amt;
  float shift;
  float mix;
  
  printf("path: <%s>\n", path);
  amt=argv[0]->f;
  shift=argv[1]->f;
  mix=argv[2]->f;
  
  printf("creating pitch shift with amount %f, shift %f, and mix %f..\n",amt,shift,mix);
  
  dsp_create_pitch_shift(amt,shift,mix);
  
  return 0;
} /* osc_add_pitch_shift_handler */

int
osc_edit_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  int module_no;
  float amt;
  float shift;
  float mix;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;
  amt=argv[1]->f;
  shift=argv[2]->f;
  mix=argv[3]->f;
  
  printf("module_no %d, editing delay of amount %f, shift %f of 1octave, mix %f..\n",module_no,amt,shift,mix);
  
  dsp_edit_pitch_shift(module_no,amt,shift,mix);
  
  return 0;
} /* osc_edit_pitch_shift_handler */

int
osc_add_vocoder_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  float freq;
  float amp;

  freq=argv[0]->f;
  amp=argv[1]->f;
  
  printf("path: <%s>\n", path);  
  printf("creating vocoder..\n");

  dsp_create_vocoder(freq,amp);
  
  return 0;
} /* osc_add_vocoder_handler */

int
osc_edit_vocoder_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  int module_no;
  float freq;
  float amp;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;
  freq=argv[1]->f;
  amp=argv[2]->f;
  
  printf("module_no %d, editing vocoder freq %f amp %f..\n",module_no,freq,amp);
  
  dsp_edit_vocoder(module_no,freq,amp);
  
  return 0;
} /* osc_edit_vocoder_handler */

