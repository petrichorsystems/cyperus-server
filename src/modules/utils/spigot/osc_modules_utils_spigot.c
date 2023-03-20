/* osc_modules_osc_utils_spigot.c
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

#include "ops_modules_utils_spigot.h"
#include "osc_modules_utils_spigot.h"

int osc_add_modules_utils_spigot_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_modules_utils_spigot_handler()..\n");
  char *request_id, *bus_id, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float open;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  bus_id = (char *)argv[1];
  open=argv[2]->f;

  target_bus = dsp_find_bus(bus_id);  
  dsp_create_utils_spigot(target_bus, open);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("osc_add_modules_utils_spigot_handler, module_id: %s\n", module_id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/utils/spigot","sisf", request_id, 0, module_id, open);
  free(lo_addr_send);
  return 0;
} /* osc_add_modules_utils_spigot_handler */


int
osc_edit_modules_utils_spigot_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data)
{
  char *request_id, *module_id;
  struct dsp_module *target_module;
  float open;
  int count;

  request_id = (char *)argv[0];
  module_id = (char *)argv[1];
  open=argv[2]->f;

  printf("osc_edit_modules_utils_spigot_handler::open: %f\n", open);

  target_module = dsp_find_module(module_id);
  dsp_edit_utils_spigot(target_module, open);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/utils/spigot","sisf", request_id, 0, module_id, open);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_modules_utils_spigot_handler */

