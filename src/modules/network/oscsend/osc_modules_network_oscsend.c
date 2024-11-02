/* osc_modules_network_oscsend.c
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

#include "osc_modules_network_oscsend.h"

int osc_add_modules_network_oscsend_handler(const char *path, const char *types, lo_arg ** argv,
                                                    int argc, void *data, void *user_data)
{
  char *request_id, *bus_id, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  char *hostname_ip, *osc_path;
  int port, multipart_no;
  float freq_div;  
  
  printf("path: <%s>\n", path);
  request_id = (char *)argv[0];
  bus_id = (char *)argv[1];

  hostname_ip = (char *)argv[2];
  port = argv[3]->i;
  osc_path = (char *)argv[4];
  freq_div = argv[5]->f;
  
  target_bus = dsp_find_bus(bus_id);
  
  dsp_create_network_oscsend(target_bus,
			     hostname_ip,
			     port,
			     osc_path,
			     freq_div);          
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  multipart_no = 0;
  osc_send_broadcast(
          "/cyperus/add/module/network/oscsend",
          "siissisf",
          request_id,
          0,
	  multipart_no,
          module_id,
	  hostname_ip,
	  port,
	  osc_path,
	  freq_div);
  
  return 0;
} /* osc_add_modules_network_oscsend_handler */


int
osc_edit_modules_network_oscsend_handler(const char *path, const char *types, lo_arg ** argv,
                                               int argc, void *data, void *user_data)
{  
  char *request_id, *module_id;
  struct dsp_module *target_module;

  char *hostname_ip, *osc_path;
  int port, multipart_no;
  float freq_div;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_id = (char *)argv[1];

  hostname_ip = (char *)argv[2];
  port = argv[3]->i;
  osc_path = (char *)argv[4];
  freq_div = argv[5]->f;
  
  target_module = dsp_find_module(module_id);
  dsp_edit_network_oscsend(target_module,
			   hostname_ip,
			   port,
			   osc_path,
			   freq_div);                                          

  multipart_no = 0;
  osc_send_broadcast(
          "/cyperus/edit/module/envelope/follower",
          "siissisf",
          request_id,
          0,
	  multipart_no,
          module_id,
	  hostname_ip,
	  port,
	  osc_path,
	  freq_div);
  
  return 0;
} /* osc_edit_modules_network_oscsend_handler */

