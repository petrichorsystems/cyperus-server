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

int osc_list_mains_handler(const char *path, const char *types, lo_arg **argv,
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
  free(mains_str);
  return 0;
} /* osc_list_mains_handler */

char *int_to_str(int x) {
  char *buffer = malloc(sizeof(char) * 13);
  if(buffer)
    sprintf(buffer, "%d", x);
  return buffer;
} /* int_to_str */

char *build_bus_list_str(struct dsp_bus *head_bus,
			 const char *separator,
			 int single,
			 int descendants) {
  struct dsp_bus *temp_bus = head_bus;
  char *single_result_str, *result_str = NULL;
  size_t single_result_str_size, result_str_size = 0;
  struct dsp_bus_port *temp_bus_port = NULL;
  int count_bus_ports;;
  char *bus_ins_str, *bus_outs_str;

  while(temp_bus != NULL) {
    /* parse inputs */
    count_bus_ports = 0;
    temp_bus_port = temp_bus->ins;
    while(temp_bus_port != NULL) {
      count_bus_ports += 1;
      temp_bus_port = temp_bus_port->next;
    }
    bus_ins_str = int_to_str(count_bus_ports);
    
    /* parse_outputs */
    count_bus_ports = 0;
    temp_bus_port = temp_bus->outs;
    while(temp_bus_port != NULL) {
      count_bus_ports += 1;
      temp_bus_port = temp_bus_port->next;
    }
    bus_outs_str = int_to_str(count_bus_ports);

    /* construct result string */
    single_result_str_size = strlen(temp_bus->id) + 1 + strlen(temp_bus->name) + 1 +
      strlen(bus_ins_str) + 1 + strlen(bus_outs_str) + 2;
    result_str_size += single_result_str_size;
    single_result_str = malloc(sizeof(char) * single_result_str_size);
    strcpy(single_result_str, temp_bus->id);
    strcat(single_result_str, separator);
    strcat(single_result_str, temp_bus->name);
    strcat(single_result_str, separator);
    strcat(single_result_str, bus_ins_str);
    strcat(single_result_str, separator);
    strcat(single_result_str, bus_outs_str);
    strcat(single_result_str, "\n");
    free(bus_ins_str);
    free(bus_outs_str);

    if(result_str == NULL) {
      result_str = malloc(sizeof(char) * single_result_str_size);
      strcpy(result_str, single_result_str);
    } else {
      result_str = realloc(result_str, sizeof(char) * (result_str_size + 1));
      strcat(result_str, single_result_str);
    }
    free(single_result_str);

    if(single)
      break;
    if(descendants)
      temp_bus = temp_bus->down;
    else
      temp_bus = temp_bus->next;
  }

  return result_str;
} /* build_bus_list_str */
			 
int osc_list_buses_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  struct dsp_bus *temp_bus;
  char *path_str, *result_str = NULL;
  int list_type = 0;
  size_t result_str_size = 0;
  struct dsp_bus *head_bus = NULL;
  struct dsp_bus_port *temp_bus_port = NULL;
  int count_bus_ports;;
  char *bus_ins_str, *bus_outs_str;

  path_str = argv[0];
  list_type = argv[1]->i;

  printf("path: <%s>\n", path);
  printf("path_str: %s\n", path_str);
  printf("list_type: %d\n", list_type);
  
  /* list types
     0 - peer
     1 - all peers
     2 - direct descendant
     3 - all descendants */

  if( !strcmp(path_str, "/") ||
      !strcmp(path_str, "") )
    head_bus = dsp_global_bus_head;
  else
    head_bus = dsp_parse_bus_path(path_str);

  if(head_bus == NULL) {
    /* no buses, return new-line char-as-str */
    result_str = "\n";
  } else {
    switch(list_type) {
    case 0: /* list peer */
      result_str = build_bus_list_str(head_bus, "|", 1, 0);
      break;
    case 1: /* list all peers */
      result_str = build_bus_list_str(head_bus, "|", 0, 0);
      break;
    case 2: /* list direct descendant */
      result_str = build_bus_list_str(head_bus, "|", 1, 1);
      break;
    case 3: /* list all direct descendants */
      result_str = build_bus_list_str(head_bus, "|", 0, 1);
      break;
    default: /* ? */
      break;
    }
  }
  lo_send(lo_addr_send,"/cyperus/list/buses", "sis", path_str, list_type, result_str);

  if(strcmp(result_str, "\n"))
    free(result_str);
  return 0;
} /* osc_list_buses_handler */

int osc_add_bus_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  struct dsp_bus *temp_bus;
  char *path_str, *bus_str, *ins_str, *outs_str = NULL;

  struct dsp_bus *new_bus;
  
  printf("path: <%s>\n", path);

  path_str = argv[0];
  bus_str = argv[1];
  ins_str = argv[2];
  outs_str = argv[3];
  
  printf("path_str: %s\n", path_str);
  printf("bus_str: %s\n", bus_str);
  printf("ins_str: %s\n", ins_str);
  printf("outs_str: %s\n", outs_str);

  new_bus = dsp_bus_init(bus_str);
  dsp_add_bus(path_str, new_bus, ins_str, outs_str);
  
  lo_send(lo_addr_send,"/cyperus/add/bus", "ssssi", path_str, bus_str, ins_str, outs_str,
	  strcmp(new_bus->name, bus_str));  
  return 0;
} /* osc_add_bus_handler */

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

