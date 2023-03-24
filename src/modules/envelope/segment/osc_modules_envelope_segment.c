/* osc_modules_envelope_segment.c
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

#include "osc_modules_envelope_segment.h"

int osc_add_modules_envelope_segment_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_modules_envelope_segment_handler()..\n");
  char *request_id, *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  char *shape = NULL;
  float rate, min, max, auto_reset;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  bus_path = (char *)argv[1];

  rate = argv[2]->f;
  shape = (char *)argv[3];
  min = argv[4]->f;
  max = argv[5]->f;
  auto_reset = argv[6]->f;
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  dsp_create_envelope_segment(target_bus,
                           rate,
                           shape,
                           min,
                           max,
                           auto_reset);
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(
          lo_addr_send,
          "/cyperus/add/module/envelope/segment","sisfsfff",
          request_id,
          0,
          module_id,
          rate,
          shape,
          min,
          max,
          auto_reset);
  free(lo_addr_send);

  return 0;
} /* osc_add_modules_envelope_segment_handler */


int
osc_edit_modules_envelope_segment_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data)
{  
  char *request_id, *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  float reset, start, step_size, min, max, direction, auto_reset;

  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];

  reset = argv[2]->f;
  start = argv[3]->f;
  step_size = argv[4]->f;
  min = argv[5]->f;
  max = argv[6]->f;
  direction = argv[7]->f;
  auto_reset = argv[8]->f;
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);
  bus_path[strlen(module_path)-37] = 0;
  
  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  module_id[strlen(module_path) - 36] = 0;
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  dsp_edit_envelope_segment(target_module,
                         reset,
                         start,
                         step_size,
                         min,
                         max,
                         direction,
                         auto_reset);
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(
          lo_addr_send,
          "/cyperus/edit/module/envelope/segment","sisfffffff",
          request_id,
          0,
          module_id,
          reset,
          start,
          step_size,
          min,
          max,
          direction,
          auto_reset);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_modules_envelope_segment_handler */

