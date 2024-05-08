/* jackcli.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under 5Bthe terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK jackcli_client for learning about software synthesis

Copyright 2015 murray foster */

#include "jackcli.h"


unsigned int jackcli_samplerate_pos;

int jackcli_samplerate;
int jackcli_buffer_size;
int jackcli_channels_in;
int jackcli_channels_out;
int jackcli_fifo_size;

const size_t jackcli_sample_size = sizeof (jack_default_audio_sample_t) ;

jack_client_t *jackcli_client = NULL;
jack_port_t **jackcli_ports_input;
jack_port_t **jackcli_ports_output;
jack_default_audio_sample_t **jackcli_outs;
jack_default_audio_sample_t **jackcli_ins;

int jackcli_process_callback(jack_nframes_t nframes, void *arg)
{  
  unsigned int i, n, p = 0; 

  struct dsp_bus *temp_bus;

  struct dsp_operation *temp_op;
  
  struct dsp_operation *temp_main_in = NULL;
  struct dsp_operation *temp_main_out = NULL;
  
  struct timespec mt1, mt2;
  long int tt;
  long process_time = (1.0f / jackcli_samplerate) * nframes * 1000000000;

  float *temp_sample_block = malloc(sizeof(float) * dsp_global_period);
  
  for(i = 0; i < jackcli_channels_in; i++)
    jackcli_ins[i] = jack_port_get_buffer(jackcli_ports_input[i], nframes);
  
  for(i=0; i < jackcli_channels_out; i++)
    {
      jackcli_outs[i] = jack_port_get_buffer(jackcli_ports_output[i], nframes);
      memset(jackcli_outs[i], 0, nframes * jackcli_sample_size);
    }
  
  for ( i = 0; i < nframes; i++) {
    /* process main inputs */
    temp_main_in = dsp_optimized_main_ins;
    for (n = 0; n < jackcli_channels_in; n++) {
      temp_main_in->outs->sample->value[i] = jackcli_ins[n][i];
      temp_main_in = temp_main_in->next;
    }
  }
  
  clock_gettime (CLOCK_REALTIME, &mt1);
  dsp_process(dsp_global_operation_head,
              jackcli_samplerate,
              jackcli_samplerate_pos);
      
  clock_gettime (CLOCK_REALTIME, &mt2);
  tt=1000000000*(mt2.tv_sec - mt1.tv_sec)+(mt2.tv_nsec - mt1.tv_nsec);
  dsp_global_load = (double)tt/(double)process_time; 
  
  temp_main_out = dsp_optimized_main_outs;

  for (n = 0; n < jackcli_channels_out; n++) {
    dsp_sum_summands(temp_sample_block, temp_main_out->ins->summands);
   
    for ( i = 0; i < nframes; i++) {
      jackcli_outs[n][i] = temp_sample_block[i];
    }    
    temp_main_out = temp_main_out->next;
  }

  if( jackcli_samplerate_pos  >= jackcli_samplerate - 1 )
    jackcli_samplerate_pos = 0;
  else
    jackcli_samplerate_pos += dsp_global_period;

  if( dsp_global_new_operation_graph ) {
    /* we will want to deallocate dsp_optimized_main_outs before continuing */
    dsp_optimized_main_outs = dsp_rebuilt_optimized_main_outs;
    dsp_rebuilt_optimized_main_outs = NULL;
    
    dsp_global_operation_head = dsp_global_operation_head_processing;
    dsp_global_new_operation_graph = 0;
    dsp_global_operation_head_processing = NULL;
  }  
  return 0 ;
} /* jackcli_process_callback */


void jackcli_allocate_ports(int channels_in, int channels_out)
{
  int i = 0;
  char name [256];

  size_t in_size = channels_in * sizeof (jack_default_audio_sample_t*);
  jackcli_ports_input = (jack_port_t **) malloc (sizeof (jack_port_t *) * channels_in);
  jackcli_ins = (jack_default_audio_sample_t **) malloc (in_size);
  memset(jackcli_ins, 0, in_size);
  for( i = 0; i < channels_in; i++)
    {
      snprintf(name, sizeof(name), "in_%d", i);
      jackcli_ports_input[i] = jack_port_register(jackcli_client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
    }

  jackcli_ports_output = calloc (channels_out, sizeof (jack_port_t *));
  jackcli_outs = calloc (channels_out, sizeof (jack_default_audio_sample_t *));
  for (i = 0 ; i < channels_out; i++)
    {     
      snprintf(name, sizeof(name), "out_%d", i) ;
      jackcli_ports_output [i] = jack_port_register(jackcli_client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
    }
} /* jackcli_allocate_ports */


void jackcli_shutdown_callback(void *arg)
{
  exit(1);
} /* jackcli_shutdown_callback */

int jackcli_set_callbacks()
{
  jack_set_process_callback(jackcli_client, jackcli_process_callback, NULL);
  jack_on_shutdown(jackcli_client, jackcli_shutdown_callback, 0);
  return 0;
} /* jackcli_set_callbacks */

int jackcli_activate_client()
{
  if (jack_activate (jackcli_client))
    {	
      printf("Cannot activate jackcli_client.\n");
      return 1;
    }
  return 0;
} /* jackcli_activate_client */

int jackcli_open(char *jackcli_client_name)
{
  /* create jack jackcli_client */
  if ((jackcli_client = jack_client_open(jackcli_client_name, JackNullOption,NULL)) == 0)
    {
      printf("Jack server not running?\n");
      return 1;
    }
  jackcli_samplerate = jack_get_sample_rate(jackcli_client);
  jackcli_buffer_size = jack_get_buffer_size(jackcli_client);

  jackcli_samplerate_pos = 0;

  return 0;
} /* jackcli_open */

int jackcli_close()
{  
  jack_client_close(jackcli_client);
  free (jackcli_ins);
  free (jackcli_outs);
  free (jackcli_ports_output);
  free (jackcli_ports_input);
} /* jackcli_close() */

int jackcli_setup(char *jackcli_client_name, int bit_depth, int channels_in, int channels_out)
{
  jackcli_channels_in = channels_in;
  jackcli_channels_out = channels_out;
  
  jackcli_open(jackcli_client_name);
  jackcli_set_callbacks();

  jackcli_allocate_ports(channels_in, channels_out);
  if (jackcli_activate_client() == 1)
    return 1;
  return 0;

} /* jackcli_close */

int jackcli_teardown()
{
  jackcli_close();
  return 0;
} /* jackcli_teardown */

