/* jackcli.h
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

#ifndef JACKCLI_H
#define JACKCLI_H

#include <jack/jack.h>

#include "rtqueue.h"

/* ==== BEGIN to-be-defined cyperus params ==== */
int jackcli_channels_in = 8;
int jackcli_channels_out = 8;
int jackcli_fifo_size = 2048;
/* ====  END  to-be-defined cyperus params ==== */

int jackcli_samplerate;
const size_t jackcli_sample_size = sizeof (jack_default_audio_sample_t) ;

jack_client_t *jackcli_client = NULL;
jack_port_t **jackcli_ports_input;
jack_port_t **jackcli_ports_output;
jack_default_audio_sample_t **jackcli_outs;
jack_default_audio_sample_t **jackcli_ins;
rtqueue_t *fifo_main_ins[jackcli_channels_in];
rtqueue_t *fifo_main_outs[jackcli_channels_out];

static int _jackcli_process_callback(jack_nframes_t nframes, void *arg);
void _jackcli_allocate_ports(int channels_in, int channels_out);
int _jackcli_fifo_setup();
static void _jackcli_shutdown_callback(void *arg);
int _jackcli_set_callbacks();
int _jackcli_activate_client();
int _jackcli_open(char *jackcli_client_name);
int _jackcli_close();
int jackcli_setup(char *jackcli_client_name, int bit_depth);
int jackcli_teardown();

#endif
