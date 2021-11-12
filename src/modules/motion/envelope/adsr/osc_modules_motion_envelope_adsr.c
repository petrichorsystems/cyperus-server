/* osc_modules_motion_envelope_adsr.c
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

#include "osc_modules_motion_envelope_adsr.h"

int osc_add_module_motion_envelope_adsr_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_module_envelope_adsr_handler()..\n");
  char *request_id, *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  int gate;
  float attack_rate, decay_rate, release_rate, sustain_level, target_ratio_a, target_ratio_dr,
    mul, add;
  
  printf("path: <%s>\n", path);
  request_id = (char *)argv[0];
  bus_path = (char *)argv[1];

  gate = argv[2]->i;
  attack_rate = argv[3]->f;
  decay_rate = argv[4]->f;
  release_rate = argv[5]->f;
  sustain_level = argv[6]->f;
  target_ratio_a = argv[7]->f;
  target_ratio_dr = argv[8]->f;
  mul = argv[9]->f;
  add = argv[10]->f;
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  dsp_create_motion_envelope_adsr(target_bus,
                                    gate,
                                    attack_rate,
                                    decay_rate,
                                    release_rate,
                                    sustain_level,
                                    target_ratio_a,
                                    target_ratio_dr,
                                    mul,
                                    add);          

  printf("now doing module stuff\n");
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("about to send msg\n");
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,
          "/cyperus/add/module/motion/envelope/adsr",
          "ssiffffffff",
          request_id,
          module_id,
          gate,
          attack_rate,
          decay_rate,
          release_rate,
          sustain_level,
          target_ratio_a,
          target_ratio_dr,
          mul,
          add);
  free(lo_addr_send);

  printf("free'd\n");
  
  return 0;
} /* osc_add_module_motion_envelope_adsr_handler */


int
osc_edit_module_motion_envelope_adsr_handler(const char *path, const char *types, lo_arg ** argv,
                                               int argc, void *data, void *user_data)
{  
  char *request_id, *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  int gate;
  float attack_rate, decay_rate, release_rate, sustain_level, target_ratio_a, target_ratio_dr,
    mul, add;

  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];

  gate = argv[2]->i;
  attack_rate = argv[3]->f;
  decay_rate = argv[4]->f;
  release_rate = argv[5]->f;
  sustain_level = argv[6]->f;
  target_ratio_a = argv[7]->f;
  target_ratio_dr = argv[8]->f;
  mul = argv[9]->f;
  add = argv[10]->f;
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);
  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  dsp_edit_motion_envelope_adsr(target_module,
                                  gate,
                                  attack_rate,
                                  decay_rate,
                                  release_rate,
                                  sustain_level,
                                  target_ratio_a,
                                  target_ratio_dr,
                                  mul,
                                  add);                                          

  printf("about to send osc msg\n");
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,
          "/cyperus/edit/module/motion/envelope/adsr",
          "ssiffffffff",
          request_id,
          module_id,
          gate,
          attack_rate,
          decay_rate,
          release_rate,
          sustain_level,
          target_ratio_a,
          target_ratio_dr,
          mul,
          add);

  printf("send osc msg\n");
  free(lo_addr_send);
  printf("free'd\n");
  
  return 0;
} /* osc_edit_module_motion_envelope_adsr_handler */

