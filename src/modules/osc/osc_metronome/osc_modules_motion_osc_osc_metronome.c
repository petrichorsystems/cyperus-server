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

#include "../../../rtqueue.h"
#include "../../../dsp_math.h"
#include "../../../dsp.h"
#include "../../../dsp_types.h"
#include "../../../dsp_ops.h"
#include "../../../jackcli.h"
#include "../../../osc.h"

#include "ops_modules_motion_osc_osc_metronome.h"
#include "osc_modules_motion_osc_osc_metronome.h"

int osc_add_module_osc_osc_metronome_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_module_osc_metronome_handler()..\n");
  char *request_id, *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float beats_per_minute;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  bus_path = (char *)argv[1];
  beats_per_minute=argv[2]->f;

  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_osc_metronome(target_bus, beats_per_minute);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("add_module_osc_metronome_handler, module_id: %s\n", module_id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/osc_metronome","sisf", request_id, 0, module_id, beats_per_minute);
  free(lo_addr_send);
  return 0;
} /* osc_add_module_osc_osc_metronome_handler */


int
osc_edit_module_osc_osc_metronome_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data)
{
  char *request_id, *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float beats_per_minute;
  int count;

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];
  beats_per_minute=argv[2]->f;

  printf("osc_edit_module_osc_osc_metronome_handler::beats_per_minute: %f\n", beats_per_minute);
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);

  module_id = malloc(sizeof(char) * 37);  
  strncpy(module_id, module_path + strlen(module_path) - 36, 37); 

  target_bus = dsp_parse_bus_path(bus_path);  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);

  dsp_edit_osc_metronome(target_module, beats_per_minute);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/osc_metronome","sisf", request_id, 0, module_id, beats_per_minute);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_module_osc_osc_metronome_handler */

