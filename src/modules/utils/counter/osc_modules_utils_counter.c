/* osc_modules_utils_counter.c
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

#include "osc_modules_utils_counter.h"

int osc_add_modules_utils_counter_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  char *request_id, *bus_id, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float reset, start, step_size, min, max, direction, auto_reset;

  int multipart;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  bus_id = (char *)argv[1];

  reset = argv[2]->f;
  start = argv[3]->f;
  step_size = argv[4]->f;
  min = argv[5]->f;
  max = argv[6]->f;
  direction = argv[7]->f;
  auto_reset = argv[8]->f;
  
  target_bus = dsp_find_bus(bus_id);
  
  dsp_create_utils_counter(target_bus,
                           reset,
                           start,
                           step_size,
                           min,
                           max,
                           direction,
                           auto_reset);
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  multipart = 0;
  osc_send_broadcast("/cyperus/add/module/utils/counter",
		     "siisfffffff",
		     request_id,
		     0,
		     multipart,
		     module_id,
		     reset,
		     start,
		     step_size,
		     min,
		     max,
		     direction,
		     auto_reset);
  
  return 0;
} /* osc_add_modules_utils_counter_handler */


int
osc_edit_modules_utils_counter_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data)
{  
  char *request_id, *module_id;
  struct dsp_module *target_module;

  float reset, start, step_size, min, max, direction, auto_reset;

  int multipart;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_id = (char *)argv[1];

  reset = argv[2]->f;
  start = argv[3]->f;
  step_size = argv[4]->f;
  min = argv[5]->f;
  max = argv[6]->f;
  direction = argv[7]->f;
  auto_reset = argv[8]->f;
  
  target_module = dsp_find_module(module_id);
  dsp_edit_utils_counter(target_module,
                         reset,
                         start,
                         step_size,
                         min,
                         max,
                         direction,
                         auto_reset);

  multipart = 0;
  osc_send_broadcast("/cyperus/edit/module/utils/counter",
		     "siisfffffff",
		     request_id,
		     0,
		     multipart,
		     module_id,
		     reset,
		     start,
		     step_size,
		     min,
		     max,
		     direction,
		     auto_reset);
  
  return 0;
} /* osc_edit_modules_utils_counter_handler */

