/* osc_modules_dsp_filter_varslope_lowpass.c
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

#include <math.h>

#include "osc_modules_audio_oscillator_pulse.h"

int osc_add_module_oscillator_pulse_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_module_oscillator_pulse_handler()..\n");
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float frequency, pulse_width, mul, add;
  
  printf("path: <%s>\n", path);

  bus_path = (char *)argv[0];

  frequency = argv[1]->f;
  pulse_width = argv[2]->f;
  mul = argv[3]->f;
  add = argv[4]->f;
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  dsp_create_oscillator_pulse(target_bus, frequency, pulse_width, mul, add);
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/audio/oscillator/pulse","sffff", module_id, frequency, pulse_width, mul, add);
  free(lo_addr_send);

  return 0;
} /* osc_add_module_oscillator_pulse_handler */


int
osc_edit_module_oscillator_pulse_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data)
{  
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float frequency, pulse_width, mul, add;
  int count;
  
  printf("path: <%s>\n", path);
  
  module_path = (char *)argv[0];
  frequency = argv[1]->f;
  pulse_width = argv[2]->f;
  mul = argv[3]->f;
  add = argv[4]->f;
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);
  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  dsp_edit_oscillator_pulse(target_module, frequency, pulse_width, mul, add);
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/audio/oscillator/pulse","sffff", module_id, frequency, pulse_width, mul, add);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_module_oscillator_pulse_handler */

