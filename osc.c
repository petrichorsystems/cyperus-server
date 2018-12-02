/* osc.c
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
#include "osc.h"
#include "osc_handlers.h"

lo_address lo_addr_send;
lo_server_thread lo_thread;

int osc_setup(char *osc_port_in, char *osc_port_out, char *addr_out) {
  lo_addr_send = lo_address_new("127.0.0.1",osc_port_out);
  lo_thread = lo_server_thread_new(osc_port_in, osc_error);

  /* below is for debug, add method that will match any path and args */
  /* lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL); */
  
  lo_server_thread_add_method(lo_thread, "/cyperus/remove", "i", osc_remove_module_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/list", "i", osc_list_modules_handler, NULL);
  
  lo_server_thread_add_method(lo_thread, "/cyperus/add/sine", "fff", osc_add_sine_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/edit/sine", "ifff", osc_edit_sine_handler, NULL);

  lo_server_thread_add_method(lo_thread, "/cyperus/add/square", "ff", osc_add_square_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/edit/square", "iff", osc_edit_square_handler, NULL);
  
  lo_server_thread_add_method(lo_thread, "/cyperus/add/pinknoise", NULL, osc_add_pinknoise_handler, NULL);

  lo_server_thread_add_method(lo_thread, "/cyperus/add/butterworth_biquad_lowpass", "ff", osc_add_butterworth_biquad_lowpass_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/edit/butterworth_biquad_lowpass", "iff", osc_edit_butterworth_biquad_lowpass_handler, NULL);

  lo_server_thread_add_method(lo_thread, "/cyperus/add/delay", "fff", osc_add_delay_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/edit/delay", "ifff", osc_edit_delay_handler, NULL);

  lo_server_thread_add_method(lo_thread, "/cyperus/add/pitch_shift", "fff", osc_add_pitch_shift_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/edit/pitch_shift", "ifff", osc_edit_pitch_shift_handler, NULL);

  lo_server_thread_add_method(lo_thread, "/cyperus/add/vocoder", "ff", osc_add_vocoder_handler, NULL);
  lo_server_thread_add_method(lo_thread, "/cyperus/edit/vocoder", "iff", osc_edit_vocoder_handler, NULL);
  
  lo_server_thread_start(lo_thread);
}
