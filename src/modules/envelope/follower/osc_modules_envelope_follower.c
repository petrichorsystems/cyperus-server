/* osc_modules_envelope_follower.c
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

#include "osc_modules_envelope_follower.h"

int osc_add_modules_envelope_follower_handler(const char *path, const char *types, lo_arg ** argv,
                                                    int argc, void *data, void *user_data)
{
  printf("osc_add_modules_envelope_follower_handler()..\n");
  char *request_id, *bus_id, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float attack, decay, scale;

  int multipart_no;
  
  printf("path: <%s>\n", path);
  request_id = (char *)argv[0];
  bus_id = (char *)argv[1];

  attack = argv[2]->f;
  decay = argv[3]->f;
  scale = argv[4]->f;
  
  target_bus = dsp_find_bus(bus_id);
  
  dsp_create_envelope_follower(target_bus,
                               attack,
                               decay,
                               scale);          
  
  printf("now doing module stuff\n");
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("about to send msg\n");

  multipart_no = 0;
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,
          "/cyperus/add/module/envelope/follower",
          "sisfff",
          request_id,
          0,
	  multipart_no,
          module_id,
          attack,
          decay,
          scale);
  free(lo_addr_send);

  printf("free'd\n");
  
  return 0;
} /* osc_add_modules_envelope_follower_handler */


int
osc_edit_modules_envelope_follower_handler(const char *path, const char *types, lo_arg ** argv,
                                               int argc, void *data, void *user_data)
{  
  char *request_id, *module_id;
  struct dsp_module *target_module;

  float attack, decay, scale;

  int multipart_no;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_id = (char *)argv[1];

  attack = argv[2]->f;
  decay = argv[3]->f;
  scale = argv[4]->f;

  target_module = dsp_find_module(module_id);
  dsp_edit_envelope_follower(target_module,
                                    attack,
                                    decay,
                                    scale);                                          

  printf("about to send osc msg\n");

  multipart_no = 0;
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,
          "/cyperus/edit/module/envelope/follower",
          "sisfff",
          request_id,
          0,
	  multipart_no,
          module_id,
          attack,
          decay,
          scale);

  printf("send osc msg\n");
  free(lo_addr_send);
  printf("free'd\n");
  
  return 0;
} /* osc_edit_modules_envelope_follower_handler */

