/* test_osc.c
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
#include <jack/jack.h> 
#include <math.h>
#include <lo/lo.h>
#include <fftw3.h>
#include <signal.h>
#include <unistd.h>

#include "cyperus.h"
#include "rtqueue.h"
#include "dsp.h"
#include "dsp_ops.h"
#include "osc_handlers.h"

static jack_default_audio_sample_t ** outs ;
static jack_default_audio_sample_t ** ins ;
int samples_can_process = 0;

lo_address lo_addr_send;

rtqueue_t *fifo_in;
rtqueue_t *fifo_out;

jack_client_t *client=NULL;
int jack_sr;
const size_t sample_size = sizeof (jack_default_audio_sample_t) ;

jack_port_t **output_port ;
static jack_port_t **input_port ;

static int
process(jack_nframes_t nframes, void *arg)
{
  float sample = 0; 
  unsigned i, n, x; 
  int sample_count = 0;

  /* allocate all output buffers */
  for(i = 0; i < 1; i++)
    {
      outs [i] = jack_port_get_buffer (output_port[i], nframes);
      memset(outs[i], 0, nframes * sample_size);
      ins [i] = jack_port_get_buffer (input_port[i], nframes);
    }
  
  for ( i = 0; i < nframes; i++) {
    if( rtqueue_isfull(fifo_in) == 0)
      rtqueue_enq(fifo_in,ins[0][i]);
    
    if( rtqueue_isempty(fifo_out) == 0){
      outs[0][i] = rtqueue_deq(fifo_out);
    }
    else {
      outs[0][i]=0;
    } 
  }
  return 0 ;
} /* process */

static void
jack_shutdown (void *arg)
{
  (void) arg ;
  exit (1) ;
} /* jack_shutdown */

void
allocate_ports(int channels, int channels_in)
{
  int i = 0;
  char name [16];
  /* allocate output ports */
  output_port = calloc (channels, sizeof (jack_port_t *)) ;
  outs = calloc (channels, sizeof (jack_default_audio_sample_t *)) ;
  for (i = 0 ; i < channels; i++)
    {     
      snprintf (name, sizeof (name), "out_%d", i + 1) ;
      output_port [i] = jack_port_register (client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0) ;
    }
  
  /* allocate input ports */
  size_t in_size = channels_in * sizeof (jack_default_audio_sample_t*);
  input_port = (jack_port_t **) malloc (sizeof (jack_port_t *) * channels_in);
  ins = (jack_default_audio_sample_t **) malloc (in_size);
  memset(ins, 0, in_size);
  
  for( i = 0; i < channels_in; i++)
    {      
      snprintf( name, sizeof(name), "in_%d", i + 1);
      input_port[i] = jack_port_register(client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    }
} /* allocate_ports */

int
set_callbacks()
{
  /* Set up callbacks. */
  jack_set_process_callback (client, process, NULL) ;
  jack_on_shutdown (client, jack_shutdown, 0) ;
  return 0;
} /* set_callbacks */

int
jack_setup(char *client_name)
{
  /* create jack client */
  if ((client = jack_client_open(client_name, JackNullOption,NULL)) == 0)
    {
      fprintf (stderr, "Jack server not running?\n") ;
      return 1 ;
    } ;

  /* store jack server's samplerate */
  jack_sr = jack_get_sample_rate (client) ;

  fifo_out = rtqueue_init(jack_sr*4);
  fifo_in = rtqueue_init(jack_sr*4);
  
  return 0;
} /* jack_setup */

int
activate_client()
{
  /* Activate client. */
  if (jack_activate (client))
    {	
      fprintf (stderr, "Cannot activate client.\n") ;
      return 1 ;
    }
  return 0;
} /* activate_client */

void error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
  int i;
  
  fprintf(stdout,"path: <%s>\n", path);
  for (i = 0; i < argc; i++) {
    fprintf(stderr,"arg %d '%c' ", i, types[i]);
    lo_arg_pp((lo_type)types[i], argv[i]);
    fprintf(stdout,"\n");
  }
  return 0;
}

int osc_remove_module_handler(const char *path, const char *types, lo_arg ** argv,
			      int argc, void *data, void *user_data)
{
  int voice;
  int module_no;
  
  fprintf(stdout, "path: <%s>\n", path);
  module_no=argv[0]->i;

  fprintf(stderr, "removing module #%d..\n",module_no);
  
  dsp_remove_module(0,module_no);
  
  return 0;
} /* osc_remove_module_handler */

int osc_list_modules_handler(const char *path, const char *types, lo_arg ** argv,
			     int argc, void *data, void *user_data)
{
  int voice_no;
  char *module_list;
  char return_string[100];
  
  fprintf(stdout, "path: <%s>\n", path);
  voice_no=argv[0]->i;

  module_list = dsp_list_modules(voice_no);
  
  fprintf(stderr, "listing modules for voice #%d..\n",voice_no);

  lo_send(lo_addr_send,"/cyperus/list","s",module_list);

  free(module_list);
  return 0;
  
} /* osc_list_modules_handler */

int main_not(void)
{
  jack_setup("cyperus");
  set_callbacks();
  if (activate_client() == 1)
    return 1;
  allocate_ports(1, 1);

  char *osc_port_in = "97217";
  char *osc_port_out = "97211";

  lo_addr_send = lo_address_new("127.0.0.1",osc_port_out);
  
  lo_server_thread st = lo_server_thread_new(osc_port_in, error);
  /* add method that will match any path and args */
  /* lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL); */
  
  /* non-generic methods */
  lo_server_thread_add_method(st, "/cyperus/remove", "i", osc_remove_module_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/list", "i", osc_list_modules_handler, NULL);
  
  lo_server_thread_add_method(st, "/cyperus/add/sine", "fff", osc_add_sine_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/edit/sine", "ifff", osc_edit_sine_handler, NULL);

  lo_server_thread_add_method(st, "/cyperus/add/square", "ff", osc_add_square_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/edit/square", "iff", osc_edit_square_handler, NULL);
  
  lo_server_thread_add_method(st, "/cyperus/add/pinknoise", NULL, osc_add_pinknoise_handler, NULL);

  lo_server_thread_add_method(st, "/cyperus/add/butterworth_biquad_lowpass", "ff", osc_add_butterworth_biquad_lowpass_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/edit/butterworth_biquad_lowpass", "iff", osc_edit_butterworth_biquad_lowpass_handler, NULL);

  lo_server_thread_add_method(st, "/cyperus/add/delay", "fff", osc_add_delay_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/edit/delay", "ifff", osc_edit_delay_handler, NULL);

  lo_server_thread_add_method(st, "/cyperus/add/pitch_shift", "fff", osc_add_pitch_shift_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/edit/pitch_shift", "ifff", osc_edit_pitch_shift_handler, NULL);

  lo_server_thread_add_method(st, "/cyperus/add/vocoder", "ff", osc_add_vocoder_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/edit/vocoder", "iff", osc_edit_vocoder_handler, NULL);

  lo_server_thread_start(st);
  
  pthread_t dsp_thread_id;
  pthread_create(&dsp_thread_id, NULL, dsp_thread, NULL);
  pthread_detach(dsp_thread_id);
  
  while(1){sleep(1);};

  return 0;
}
