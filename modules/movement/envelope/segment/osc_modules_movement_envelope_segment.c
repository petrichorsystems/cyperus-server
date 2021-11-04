/* osc_modules_movement_envelope_segment.c
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

#include "osc_modules_movement_envelope_segment.h"

void _parse_envelope_segments(lo_arg **argv,
                              float *levels,
                              float *times,
                              float *curve,
                              int *release_node,
                              int *loop_node,
                              int *offset,
                              float *gate,
                              float *level_scale,
                              float *level_bias,
                              float *time_scale
                              ) {
  int num_levels = argv[1]->i;
  levels = malloc(sizeof(float) * num_levels);
  for(int idx=2; idx < num_levels + 2; idx++)
    levels[idx - 2] = argv[idx]->f;

  int num_times = argv[idx]->i;  
  times = malloc(sizeof(float)*num_times);
  for(idx=idx+1; idx < num_levels + 2 + num_times + 1; idx++)
    times[idx - 2 - num_times - 1] = argv[idx]->f;
  
  int num_curve = argv[idx]->i;  
  curve = malloc(sizeof(float)*num_curve);
  for(idx=idx+1; idx < num_levels + 2 + num_times + 1 + num_curve + 1; idx++)
    curve[idx - 2 - num_times - 1 - num_levels - 1] = argv[idx]->f;

  idx += 1;
  release_node = argv[idx]->i;

  idx += 1;
  loop_node = argv[idx]->i;

  idx += 1;
  offset = argv[idx]->i;

  idx += 1;
  gate = argv[idx]->f;

  idx += 1;
  level_scale = argv[idx]->f;

  idx += 1;
  level_bias = argv[idx]->f;

  idx += 1;
  time_scale = argv[idx]->f;

} /* _parse_envelope_segments */

int osc_add_module_movement_envelope_segment_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_module_envelope_segment_handler()..\n");
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float *levels, *times, *curve;
  int release_node, loop_node, offset;
  float gate, level_scale, level_bias, time_scale;

  printf("path: <%s>\n", path);
  bus_path = (char *)argv[0];
  
  _parse_envelope_segments(argv,
                           levels,
                           times,
                           curve,
                           &release_node,
                           &loop_node,
                           &offset,
                           &gate,
                           &level_scale,
                           &level_bias,
                           &time_scale);
  
  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_movement_envelope_segment(target_bus,
                                       levels,
                                       times,
                                       curve,
                                       release_node,
                                       loop_node,
                                       offset,
                                       gate,
                                       level_scale,
                                       level_bias,
                                       time_scale);

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
          "/cyperus/add/module/movement/envelope/segment",
          "siffffffff",
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
} /* osc_add_module_movement_envelope_segment_handler */


int
osc_edit_module_movement_envelope_segment_handler(const char *path, const char *types, lo_arg ** argv,
                                               int argc, void *data, void *user_data)
{  
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  float *levels, *times, *curve;
  int release_node, loop_node, offset;
  float gate, level_scale, level_bias, time_scale;

  printf("path: <%s>\n", path);
  
  module_path = (char *)argv[0];
  _parse_envelope_segments(argv,
                           levels,
                           times,
                           curve,
                           &release_node,
                           &loop_node,
                           &offset,
                           &gate,
                           &level_scale,
                           &level_bias,
                           &time_scale);
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);
  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  dsp_edit_movement_envelope_segment(target_module,
                                     levels,
                                     times,
                                     curve,
                                     release_node,
                                     loop_node,
                                     offset,
                                     gate,
                                     level_scale,
                                     level_bias,
                                     time_scale);
  
  printf("about to send osc msg\n");
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,
          "/cyperus/edit/module/movement/envelope/segment",
          "siffffffff",
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
} /* osc_edit_module_movement_envelope_segment_handler */

