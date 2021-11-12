/* osc_modules_motion_envelope_segment.c
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

#include "osc_modules_motion_envelope_segment.h"

void _parse_envelope_segments(lo_arg **argv,
                              float *levels,
                              float *times,
                              float *shape,
                              float *curve,
                              int *release_node,
                              int *loop_node,
                              int *offset,
                              float *gate,
                              float *level_scale,
                              float *level_bias,
                              float *time_scale,
                              int *num_stages) {
  int num_levels = argv[2]->i;
  int idx;

  levels = malloc(sizeof(float) * num_levels);
    for(idx=3; idx < num_levels + 3; idx++)
      levels[idx - 3] = argv[idx]->f;

  int num_times = argv[idx]->i;  
  times = malloc(sizeof(float)*num_times);
  for(idx=idx+1; idx < num_levels + 3 + num_times + 1; idx++)
    times[idx - 3 - num_times - 1] = argv[idx]->f;

  int num_shape = argv[idx]->i;  
  shape = malloc(sizeof(float)*num_shape);
  for(idx=idx+1; idx < num_levels + 3 + num_times + 1 + num_shape + 1; idx++)
    shape[idx - 3 - num_times - 1 - num_levels - 1] = argv[idx]->f;

  int num_curve = argv[idx]->i;  
  curve = malloc(sizeof(float)*num_curve);
  for(idx=idx+1; idx < num_levels + 3 + num_times + 1 + num_shape + 1 + num_curve + 1; idx++)
    curve[idx - 3 - num_times - 1 - num_levels - 1 - num_curve - 1] = argv[idx]->f;
  
  idx += 1;
  memcpy(release_node, &argv[idx]->i, sizeof(int));

  idx += 1;
  memcpy(loop_node, &argv[idx]->i, sizeof(int));

  idx += 1;
  memcpy(offset, &argv[idx]->i, sizeof(int));

  idx += 1;
  memcpy(gate, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(level_scale, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(level_bias, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(time_scale, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(num_stages, &argv[idx]->i, sizeof(int));
} /* _parse_envelope_segments */

void _parse_edit_envelope_segments(lo_arg **argv,
                                   int *release_node,
                                   int *loop_node,
                                   int *offset,
                                   float *gate,
                                   float *level_scale,
                                   float *level_bias,
                                   float *time_scale) {
  int idx = 2;
  memcpy(release_node, &argv[idx]->i, sizeof(int));

  idx += 1;
  memcpy(loop_node, &argv[idx]->i, sizeof(int));

  idx += 1;
  memcpy(offset, &argv[idx]->i, sizeof(int));

  idx += 1;
  memcpy(gate, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(level_scale, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(level_bias, &argv[idx]->f, sizeof(float));

  idx += 1;
  memcpy(time_scale, &argv[idx]->f, sizeof(float));  
} /* _parse_edit_envelope_segments */

void _parse_envelope_stdshapes(lo_arg **argv,
                              float **levels,
                              float **times,
                              float **shape,
                              float **curve,
                              int *release_node,
                              int *loop_node,
                              int *offset,
                              float *gate,
                              float *level_scale,
                              float *level_bias,
                               float *time_scale,
                              int *num_stages) {
  int stdshape = argv[2]->i;
  int idx = 0;
  int num_levels = 0;
  int num_times = 0;
  int num_shape = 0;
  int num_curve = 0;

  switch(stdshape) {
  case segment_stdshape_Perc:
    printf("segment_stdshape_Perc\n");
    float attack_time = argv[3]->f;
    float release_time = argv[4]->f;
    float level = argv[5]->f;
    float perc_curve = argv[6]->f;
    
    *levels = malloc(sizeof(float) * 3);
    (*levels)[0] = 0.0f;
    (*levels)[1] = level;
    (*levels)[2] = 0.0f;

    *times = malloc(sizeof(float) * 2);
    (*times)[0] = attack_time;
    (*times)[1] = release_time;

    *shape = malloc(sizeof(float));
    (*shape)[0] = shape_Curve;
    
    *curve = malloc(sizeof(float));
    (*curve)[0] = perc_curve;
    
    *release_node = -1;
    *loop_node = 0;
    *gate = -1.0f;
    *level_scale = 1.0f;
    *level_bias = 0.0f;
    *time_scale = 1.0f;
    *num_stages = 3;
    break;
  default:
    break;
  }
} /* _parse_envelope_stdshapes */


int osc_add_module_motion_envelope_segment_handler(const char *path, const char *types, lo_arg ** argv,
						   int argc, void *data, void *user_data)
{
  printf("osc_add_module_envelope_segment_handler()..\n");
  char *request_id, *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float *levels, *times, *shape, *curve;
  int release_node, loop_node, offset, num_stages;
  float gate, level_scale, level_bias, time_scale;

  printf("path: <%s>\n", path);
  request_id = (char *)argv[0];
  bus_path = (char *)argv[1];
  
  _parse_envelope_segments(argv,
                           levels,
                           times,
                           shape,
                           curve,
                           &release_node,
                           &loop_node,
                           &offset,
                           &gate,
                           &level_scale,
                           &level_bias,
                           &time_scale,
                           &num_stages);
  
  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_motion_envelope_segment(target_bus,
                                       levels,
                                       times,
                                       shape,
                                       curve,
                                       release_node,
                                       loop_node,
                                       offset,
                                       gate,
                                       level_scale,
                                       level_bias,
                                       time_scale,
                                       num_stages);
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  /* printf("about to send msg\n"); */
  
  /* lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out); */
  /* lo_send(lo_addr_send, */
  /*         "/cyperus/add/module/motion/envelope/segment", */
  /*         "siffffffff", */
  /*         module_id, */
  /*         gate, */
  /*         attack_rate, */
  /*         decay_rate, */
  /*         release_rate, */
  /*         sustain_level, */
  /*         target_ratio_a, */
  /*         target_ratio_dr, */
  /*         mul, */
  /*         add); */
  /* free(lo_addr_send); */

  printf("free'd\n");
  
  return 0;
} /* osc_add_module_motion_envelope_segment_handler */

int
osc_edit_module_motion_envelope_segment_handler(const char *path, const char *types, lo_arg ** argv,
                                               int argc, void *data, void *user_data)
{  
  char *request_id, *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  int release_node, loop_node, offset;
  float gate, level_scale, level_bias, time_scale;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];
  
  _parse_edit_envelope_segments(argv,
                                &release_node,
                                &loop_node,
                                &offset,
                                &gate,
                                &level_scale,
                                &level_bias,
                                &time_scale);
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  snprintf(bus_path, strlen(module_path)-37+1, "%s", module_path);

  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  dsp_edit_motion_envelope_segment(target_module,
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
          "/cyperus/edit/module/motion/envelope/segment", 
          "ssiiiffff",
          request_id,
          module_id, 
          release_node,
          loop_node,
          offset,
          gate,
          level_scale,
          level_bias,
          time_scale); 
  printf("send osc msg\n"); 
  free(lo_addr_send); 
  
  printf("free'd\n");
  
  return 0;
} /* osc_edit_module_motion_envelope_segment_handler */

int osc_add_module_motion_envelope_stdshape_handler(const char *path, const char *types, lo_arg ** argv,
                                                      int argc, void *data, void *user_data)
{
  printf("osc_add_module_envelope_stdshape_handler()..\n");
  char *request_id, *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float *levels, *times, *shape, *curve;
  int release_node, loop_node, offset, num_stages;
  float gate, level_scale, level_bias, time_scale;

  printf("path: <%s>\n", path);
  request_id = (char *)argv[0];
  bus_path = (char *)argv[1];
  
  _parse_envelope_stdshapes(argv,
                            &levels,
                            &times,
                            &shape,
                            &curve,
                            &release_node,
                            &loop_node,
                            &offset,
                            &gate,
                            &level_scale,
                            &level_bias,
                            &time_scale,
                            &num_stages);

  printf("what's up\n");
     printf(" -- attack_time: %f\n", times[0]); 
     printf(" -- release_time: %f\n", times[1]); 
     printf(" -- level: %f\n", levels[1]); 
     printf(" -- perc_curve: %f\n", curve[0]); 
  
  printf("bus_path: %s\n", bus_path); 
  target_bus = dsp_parse_bus_path(bus_path);
  
  dsp_create_motion_envelope_segment(target_bus,
                                     levels,
                                     times,
                                     shape,
                                     curve,
                                     release_node,
                                     loop_node,
                                     offset,
                                     gate,
                                     level_scale,
                                     level_bias,
                                     time_scale,
                                     num_stages);
  
  printf("now doing module stuff\n");
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37); 
  snprintf(module_id, strlen(target_module->id)+1, "%s", target_module->id);
  
  printf("about to send msg\n");
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,
          "/cyperus/add/module/motion/envelope/stdshape",
          "ssiffff",
          request_id,
          module_id,
          argv[1]->i,
          argv[2]->f,
          argv[3]->f,
          argv[4]->f,
          argv[5]->f);
  free(lo_addr_send);

  printf("free'd\n");
  
  return 0;
} /* osc_add_module_motion_envelope_stdshape_handler */
