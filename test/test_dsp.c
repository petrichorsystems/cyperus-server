/* test_dsp.c
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

#include "../cyperus.h"
#include "../rtqueue.h"
#include "../libcyperus.h"
#include "../dsp.h"
#include "../dsp_types.h"
#include "../dsp_ops.h"


char*
strconcat(char *str_prefix, char *str_suffix) {
  char *full_str;  
  full_str = (char*)malloc(sizeof(char)*(strlen(str_suffix)+strlen(str_prefix)+1));
  snprintf(full_str, strlen(str_suffix)+strlen(str_prefix)+1, "%s%s", str_prefix, str_suffix);
  return full_str;
}

void
test_dsp_add_busses() {
  fprintf(stderr, "  >> starting test_dsp_add_busses()\n");

  char *bus_path;
  char *temp_bus_path, *delay_bus_path, *left_bus_path;
  dsp_global_bus_head = NULL;
  struct dsp_bus *temp_bus_main, *temp_bus_delay, *temp_bus_left = NULL;

  /* create initial busses */
  temp_bus_main = dsp_bus_init("main");  
  dsp_add_bus("/", temp_bus_main, NULL, NULL);
  temp_bus_delay = dsp_bus_init("delay");
  temp_bus_path = strconcat("/", temp_bus_main->id);
  dsp_add_bus(temp_bus_path, temp_bus_delay, NULL, NULL);
  delay_bus_path = strconcat(temp_bus_path, "/");
  left_bus_path = strconcat(delay_bus_path, temp_bus_delay->id);
  temp_bus_left = dsp_bus_init("left");
  dsp_add_bus(left_bus_path, temp_bus_left, "in", "out");
  left_bus_path = strconcat(left_bus_path, "/");
  temp_bus_path = strconcat(left_bus_path, temp_bus_left->id);
  temp_bus_main = dsp_parse_bus_path(temp_bus_path);
  free(temp_bus_path);
  if( strcmp(temp_bus_main->id, temp_bus_left->id) == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
  return;
  }

void
test_dsp_add_modules() {
  fprintf(stderr, "  >> starting test_dsp_add_modules()\n");
  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus;
  char *main_id, *delay_id, *left_id;
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);  
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);  
  temp_bus = dsp_parse_bus_path(bus_path);
  dsp_create_block_processor(temp_bus);
  if( strcmp(temp_bus->dsp_module_head->name, "block_processor") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");  
  return;
}

void
test_dsp_parse_path_bus() {
  fprintf(stderr, "  >> starting test_dsp_parse_path_bus()\n");
  char *result[3]; 
  dsp_parse_path(result, "/main/delay/left");
  if(strcmp(result[0], "/") == 0 &&
     strcmp(result[1], "/main/delay/left") == 0 &&
     strcmp(result[2], "left") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
}

void
test_dsp_parse_path_bus_port() {
  fprintf(stderr, "  >> starting test_dsp_parse_path_bus_port()\n");
  char *result[3];
  dsp_parse_path(result, "/main/delay/left:bus_port");
  if(strcmp(result[0], ":") == 0 &&
     strcmp(result[1], "/main/delay/left") == 0 &&
     strcmp(result[2], "bus_port") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
  free(result[1]);
}

void
test_dsp_parse_path_module() {
  fprintf(stderr, "  >> starting test_dsp_parse_path_module()\n");
  char *result[3];
  dsp_parse_path(result, "/main/delay/left?module");
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/left") == 0 &&
     strcmp(result[2], "module") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
  free(result[1]);
  free(result[2]);
}

void
test_dsp_parse_path_input_port() {
  fprintf(stderr, "  >> starting test_dsp_parse_path_input_port()\n");
  char *result[3];
  dsp_parse_path(result, "/main/delay/left<input0");
  if(strcmp(result[0], "<") == 0 &&
     strcmp(result[1], "/main/delay/left") ==0 &&
     strcmp(result[2], "input0") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
  free(result[1]);
  free(result[2]);
}

void
test_dsp_parse_path_output_port() {
  fprintf(stderr, "  >> starting test_dsp_parse_path_output_port()\n");
  char *result[3];
  dsp_parse_path(result, "/main/delay/right>output0");
  if(strcmp(result[0], ">") == 0 &&
     strcmp(result[1], "/main/delay/right") ==0 &&
     strcmp(result[2], "output0") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
  free(result[1]);
  free(result[2]);
}

void
test_dsp_parse_path_mains_in() {
  fprintf(stderr, "  >> starting test_dsp_parse_mains_in()\n");
  char *result[3]; 
  dsp_parse_path(result, "/mains{90401ba5-a77d-4115-b5d2-a67776aa4448");
  if(strcmp(result[0], "{") == 0 &&
     strcmp(result[1], "/mains")  == 0 &&
     strcmp(result[2], "90401ba5-a77d-4115-b5d2-a67776aa4448") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
}

void
test_dsp_parse_path_mains_out() {
  fprintf(stderr, "  >> starting test_dsp_parse_mains_out()\n");
  char *result[3]; 
  dsp_parse_path(result, "/mains}90401ba5-a77d-4115-b5d2-a67776aa4448");
  if(strcmp(result[0], "}") == 0 &&
     strcmp(result[1], "/mains")  == 0 &&
     strcmp(result[2], "90401ba5-a77d-4115-b5d2-a67776aa4448") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
}

void
test_dsp_parse_path_module_output_port() {
  fprintf(stderr, "  >> starting test_dsp_parse_path_module_output_port()\n");
  char *result[3];
  char *sub_path;
  dsp_parse_path(result, "/main/delay/right?delay>output0");
  if(strcmp(result[0], ">") == 0 &&
     strcmp(result[1], "/main/delay/right?delay") ==0 &&
     strcmp(result[2], "output0") == 0)
    {}
  else {
    fprintf(stderr, " >> failed..\n");
    return;
  }
  sub_path = (char *)malloc(strlen(result[1]) + 1);
  memset(sub_path, '\0', sizeof(sub_path));
  strcpy(sub_path, result[1]);
  free(result[1]);
  free(result[2]);
  dsp_parse_path(result, sub_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/right") ==0 &&
     strcmp(result[2], "delay") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
}

void
test_dsp_find_module() {
  fprintf(stderr, "  >> starting test_dsp_find_module()\n");
  char *result[3];
  struct dsp_module *temp_module, *final_module;

  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path, *temp_module_path, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus, *final_bus;
  char *main_id, *delay_id, *left_id, *module_id;

  /* grab created busses */
  main_bus = dsp_global_bus_head;

  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;

  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);

  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);

  temp_bus = dsp_parse_bus_path(bus_path);
  temp_module = temp_bus->dsp_module_head;
  module_id = temp_module->id;
  temp_module_path = strconcat(left_path, "?");
  module_path = strconcat(temp_module_path, module_id);

  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");

  final_bus = dsp_parse_bus_path(result[1]);
  final_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);

  if( strcmp(final_module->id, result[2]) == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");

  free(result[1]);
  free(result[2]);
}

void
test_dsp_find_port_out() {
  fprintf(stderr, "  >> starting test_dsp_find_port_out()\n");
  struct dsp_port_out *temp_port_out, *head_port_out;
  char *result[3];
  struct dsp_module *temp_module, *final_module;
  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path, *temp_module_path, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus, *final_bus;
  char *main_id, *delay_id, *left_id, *module_id, *port_out_id;

  /* grab created busses */
  main_bus = dsp_global_bus_head;

  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;

  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);

  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);

  temp_bus = dsp_parse_bus_path(bus_path);
  temp_module = temp_bus->dsp_module_head;
  module_id = temp_module->id;
  temp_module_path = strconcat(left_path, "?");
  module_path = strconcat(temp_module_path, module_id);

  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");

  final_bus = dsp_parse_bus_path(result[1]);
  final_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);

  if( strcmp(final_module->id, result[2]) == 0) {
  } else
    fprintf(stderr, " >> failed!\n");

  free(result[1]);
  free(result[2]);

  head_port_out = temp_module->outs;
  port_out_id = head_port_out->id;

  temp_port_out = dsp_find_port_out(temp_module->outs, port_out_id);
  if( strcmp(temp_port_out->name, "out") == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");
}

void
test_dsp_find_port_in() {
  fprintf(stderr, "  >> starting test_dsp_find_port_in()\n");
  struct dsp_port_in *temp_port_in, *head_port_in;
  char *result[3];
  struct dsp_module *temp_module, *final_module;
  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path, *temp_module_path, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus, *final_bus;
  char *main_id, *delay_id, *left_id, *module_id, *port_in_id;

  /* grab created busses */
  main_bus = dsp_global_bus_head;

  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;

  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);

  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);

  temp_bus = dsp_parse_bus_path(bus_path);
  temp_module = temp_bus->dsp_module_head;
  module_id = temp_module->id;
  temp_module_path = strconcat(left_path, "?");
  module_path = strconcat(temp_module_path, module_id);

  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");

  final_bus = dsp_parse_bus_path(result[1]);
  final_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);

  if( strcmp(final_module->id, result[2]) == 0) {
  } else
    fprintf(stderr, " >> failed!\n");

  free(result[1]);
  free(result[2]);

  head_port_in = temp_module->ins;
  port_in_id = head_port_in->id;

  temp_port_in = dsp_find_port_in(temp_module->ins, port_in_id);
  if( strcmp(temp_port_in->name, "in") == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");
}

void
test_dsp_block_processor() {
  fprintf(stderr, "  >> starting test_dsp_block_processor()\n");
  char *result[3];
  struct dsp_module *temp_module, *final_module;
  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path, *temp_module_path, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus, *final_bus;
  char *main_id, *delay_id, *left_id, *module_id, *port_in_id;

  float insample;

  /* grab created busses */
  main_bus = dsp_global_bus_head;

  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;

  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);

  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);

  temp_bus = dsp_parse_bus_path(bus_path);
  temp_module = temp_bus->dsp_module_head;
  module_id = temp_module->id;
  temp_module_path = strconcat(left_path, "?");
  module_path = strconcat(temp_module_path, module_id);

  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");

  final_bus = dsp_parse_bus_path(result[1]);
  final_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);

  if( strcmp(final_module->id, result[2]) == 0) {
  } else
    fprintf(stderr, " >> failed!\n");

  insample = 0.543210;
  rtqueue_enq(final_module->ins->values, insample);
  dsp_block_processor(result[1], final_module, 1, 48000);
  if( final_module->outs->value == insample )
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed!\n");
  
  free(result[1]);
  free(result[2]);
}

void
test_dsp_bus_port() {
  fprintf(stderr, "  >> starting test_dsp_bus_port()\n");
  char *main_path, *temp_path, *bus_path;
  struct dsp_bus *main_bus;
  char *main_id, *module_id, *port_in_id;
  struct dsp_bus_port *bus_port_in, *bus_port_out;
  struct dsp_bus *target_bus, *new_bus;
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  new_bus = dsp_bus_init("aux");
  dsp_add_bus(main_path, new_bus, "in", "out");
  temp_path = strconcat(main_path, "/");
  bus_path = strconcat(temp_path, new_bus->id);
  target_bus = dsp_parse_bus_path(bus_path);
  bus_port_in = target_bus->ins;
  bus_port_out = target_bus->outs;
  if( strcmp(bus_port_out->name, "out") == 0) {
    {}
  } else
    fprintf(stderr, " >> failed!\n");
  if(strcmp(bus_port_in->name, "in") == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n"); 
}

void
test_dsp_bus_port_ports() {
  fprintf(stderr, "  >> starting test_dsp_bus_port_ports()\n");
  char *main_path, *temp_path, *bus_path;
  struct dsp_bus *main_bus;
  char *main_id, *module_id, *port_in_id;
  struct dsp_bus_port *bus_port_in, *bus_port_out;
  struct dsp_port_in *port_in;
  struct dsp_port_out *port_out;
  struct dsp_bus *target_bus, *new_bus;
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  new_bus = dsp_bus_init("aux");
  dsp_add_bus(main_path, new_bus, "in", "out");
  temp_path = strconcat(main_path, "/");
  bus_path = strconcat(temp_path, new_bus->id);
  target_bus = dsp_parse_bus_path(bus_path);
  bus_port_in = target_bus->ins;
  bus_port_out = target_bus->outs;
  port_in = target_bus->ins->in;
  port_out = target_bus->outs->out;
  if( strcmp(port_out->name, "out") == 0) {
    {}
  } else
    fprintf(stderr, " >> failed!\n");
  if(strcmp(port_in->name, "in") == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n"); 
}

void
test_dsp_bus_port_port_out() {
  fprintf(stderr, "  >> starting test_dsp_bus_port_port_out()\n");
  struct dsp_port_out *port_out;
  struct dsp_bus_port *bus_port_out;
  char *result[3];
  struct dsp_module *temp_module, *final_module;
  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path, *temp_module_path, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus, *final_bus;
  char *main_id, *delay_id, *left_id, *module_id, *port_in_id;
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);
  temp_bus = dsp_parse_bus_path(bus_path);
  bus_port_out = temp_bus->outs;
  port_out = temp_bus->outs->out;
  temp_bus->outs->out->value = 6.66;
  if( strcmp(port_out->name, "out") == 0)
    {}
  else
    fprintf(stderr, " >> failed!\n");
  /* account for rounding errors.. */
  if(port_out->value > 6.659999 && port_out->value < 6.660001) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n"); 
}
 
void
test_dsp_bus_port_port_in() {
  fprintf(stderr, "  >> starting test_dsp_bus_port_port_in()\n");
  struct dsp_port_in *port_in;
  struct dsp_bus_port *bus_port_in;
  struct dsp_bus *target_bus;
  target_bus = dsp_parse_bus_path("/main/delay/right");
  struct dsp_module *temp_module, *final_module;
  char *main_path, *temp_delay_path, *delay_path, *left_path, *bus_path, *temp_module_path, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *temp_bus, *final_bus;
  char *main_id, *delay_id, *left_id, *module_id, *port_in_id;
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  temp_delay_path = strconcat(main_path, "/");
  delay_path = strconcat(temp_delay_path, delay_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path = strconcat(delay_path, "/");
  bus_path = strconcat(left_path, left_id);
  temp_bus = dsp_parse_bus_path(bus_path);
  bus_port_in = temp_bus->ins;
  port_in = temp_bus->ins->in;
  float final_value, test_value;
  test_value = 6.66;
  rtqueue_enq(temp_bus->ins->in->values, test_value);
  if( strcmp(port_in->name, "in") == 0) {
    {}
  } else
    fprintf(stderr, " >> failed!\n");
  final_value = rtqueue_deq(temp_bus->ins->in->values);
  /* account for rounding errors.. */
  if(final_value > 6.659999 && final_value < 6.660001) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n"); 
}

void
test_dsp_add_connection() {
  fprintf(stderr, "  >> starting test_dsp_add_connection()\n");
  char *main_path, *delay_path_temp, *delay_path, *left_path_temp, *left_path, *aux_path_temp, *aux_path,
    *main_aux_out_path_temp, *main_aux_out_path, *delay_left_in_path_temp, *delay_left_in_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *aux_bus;
  char *main_id, *delay_id, *aux_id, *left_id;
  struct dsp_bus_port *bus_port_out, *bus_port_in;
  struct dsp_connection *connection;

  /* tres importante yo!! */
  dsp_global_connection_graph = NULL;
  
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  delay_path_temp = strconcat(main_path, "/");
  delay_path = strconcat(delay_path_temp, delay_id);
  aux_bus = main_bus->down->next;
  aux_id = aux_bus->id;
  aux_path = strconcat(delay_path_temp, aux_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path_temp = strconcat(delay_path, "/");
  left_path = strconcat(left_path_temp, left_id);
  left_bus = dsp_parse_bus_path(left_path);

  bus_port_out = aux_bus->outs;
  bus_port_in = left_bus->ins;

  main_aux_out_path_temp = strconcat(aux_path, ":");
  main_aux_out_path = strconcat(main_aux_out_path_temp, aux_bus->outs->id);

  delay_left_in_path_temp = strconcat(left_path, ":");
  delay_left_in_path = strconcat(delay_left_in_path_temp, left_bus->ins->id);

  /* construct id paths */
  dsp_add_connection(main_aux_out_path,
		     delay_left_in_path);

  connection = dsp_global_connection_graph;

  if( (strcmp(connection->id_out, main_aux_out_path) != 0) &&
      (strcmp(connection->id_in, delay_left_in_path) != 0) ) {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  fprintf(stderr, " >> success!\n");
}

void
test_dsp_feed_connections_bus() {
  fprintf(stderr, "  >> starting test_dsp_feed_connections_bus()\n");

  char *main_path, *delay_path_temp, *delay_path, *left_path_temp, *left_path, *aux_path_temp, *aux_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus, *aux_bus;
  char *main_id, *delay_id, *aux_id, *left_id;
  
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  delay_path_temp = strconcat(main_path, "/");
  delay_path = strconcat(delay_path_temp, delay_id);
  aux_bus = main_bus->down->next;
  aux_id = aux_bus->id;
  aux_path = strconcat(delay_path_temp, aux_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path_temp = strconcat(delay_path, "/");
  left_path = strconcat(left_path_temp, left_id);
  left_bus = dsp_parse_bus_path(left_path);
  
  rtqueue_enq(aux_bus->outs->in->values, 0.12345);
  dsp_feed_connections_bus(aux_path, aux_bus->outs);
  dsp_feed_connections_bus(left_path, left_bus->ins);
  if( left_bus->ins->out->value != (float)0.12345 ) {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  fprintf(stderr, " >> success!\n");
}

void
test_dsp_sum_inputs() {
  fprintf(stderr, "  >> starting test_dsp_sum_inputs()\n");

  char *result[3];
  struct dsp_module *module;
  float insample = 0.12345;
  float outsample;

  char *main_path, *delay_path_temp, *delay_path, *left_path_temp, *left_path, *module_path_temp, *module_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus;
  char *main_id, *delay_id, *left_id, *module_id;
  
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  delay_path_temp = strconcat(main_path, "/");
  delay_path = strconcat(delay_path_temp, delay_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path_temp = strconcat(delay_path, "/");
  left_path = strconcat(left_path_temp, left_id);
  left_bus = dsp_parse_bus_path(left_path);

  module = left_bus->dsp_module_head;
  module_id = module->id;

  module_path_temp = strconcat(left_path, "?");
  module_path = strconcat(module_path_temp, module_id);
  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else {
    fprintf(stderr, " >> failed..\n");
    exit(1);
  }
  
  rtqueue_enq(module->ins->values, insample);
  outsample = dsp_sum_input(module->ins);

  if( outsample == (float)0.12345 ) {
  } else
    fprintf(stderr, " >> failed!\n");
  
  free(result[1]);
  free(result[2]);  

  fprintf(stderr, " >> success!\n");
}

void
test_dsp_feed_outputs() {
  fprintf(stderr, "  >> starting test_dsp_feed_outputs()\n");

  char *result[3];
  struct dsp_module *module, *module1;
  struct dsp_connection *connection;
  float insample = 0.12345;
  float outsample;

  char *main_path, *delay_path_temp, *delay_path, *left_path_temp, *left_path, *module_path_temp, *module_path,
    *module1_path_temp, *module1_path, *module_out_path_temp, *module_out_path, *module1_in_path_temp, *module1_in_path;;
  struct dsp_bus *main_bus, *delay_bus, *left_bus;
  char *main_id, *delay_id, *left_id, *module_id, *module1_id, *module_out_id, *module1_in_id;;
  
  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  delay_path_temp = strconcat(main_path, "/");
  delay_path = strconcat(delay_path_temp, delay_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path_temp = strconcat(delay_path, "/");
  left_path = strconcat(left_path_temp, left_id);
  left_bus = dsp_parse_bus_path(left_path);

  module = left_bus->dsp_module_head;
  module_id = module->id;

  module_path_temp = strconcat(left_path, "?");
  module_path = strconcat(module_path_temp, module_id);
  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else {
    fprintf(stderr, " >> failed..\n");
    exit(1);
  }
  
  dsp_create_block_processor(left_bus);
  if( strcmp(left_bus->dsp_module_head->next->name, "block_processor") == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");  

  module1 = left_bus->dsp_module_head->next;
  module1_id = module1->id;

  module1_path_temp = strconcat(left_path, "?");
  module1_path = strconcat(module1_path_temp, module1_id);

  module_out_id = module->outs->id;
  module1_in_id = module1->ins->id;

  module_out_path_temp = strconcat(module_path, ">");
  module_out_path = strconcat(module_out_path_temp, module_out_id);

  module1_in_path_temp = strconcat(module1_path, "<");
  module1_in_path = strconcat(module1_in_path_temp, module1_in_id);
  
  dsp_add_connection(module_out_path,
		     module1_in_path);

  rtqueue_enq(module->ins->values, insample);
  outsample = dsp_sum_input(module->ins);
  module->outs->value = outsample;
  dsp_feed_outputs(left_path, module_id, module->outs);
  outsample = dsp_sum_input(module1->ins);

  if( outsample == (float)0.12345 )
    {}
  else
    fprintf(stderr, " >> failed!\n");
  
  free(result[1]);
  free(result[2]);  

  fprintf(stderr, " >> success!\n");
}

void
test_dsp_mains_allocate() {
  fprintf(stderr, "  >> starting test_dsp_allocate_mains()\n");

  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  int i;

  int channels_in = 8;
  int channels_out = 4;
  int fifo_size = 512;

  dsp_mains_allocate(channels_in, channels_out, fifo_size);

  for(i=0; i<channels_in; i++) {
    if( i == 0 ) {
      temp_port_out = dsp_main_ins;
    } else {
      temp_port_out = temp_port_out->next;
    }
    if( strlen(temp_port_out->id) != 36 ) {
      fprintf(stderr, " >> failed..\n");
      exit(1);
    }
  }
  for(i=0; i<channels_out; i++) {
    if( i == 0 ) {
      temp_port_in = dsp_main_outs;
    } else {
      temp_port_in = temp_port_in->next;
    }
    if( strlen(temp_port_in->id) != 36 ) {
      fprintf(stderr, " >> failed..\n");
      exit(1);
    }
  }
  fprintf(stderr, " >> success!\n");
}

void
test_dsp_feed_mains() {
  fprintf(stderr, "  >> starting test_dsp_feed_mains()\n");

  char *result[3];
  struct dsp_module *module, *module1;
  struct dsp_connection *connection;
  float insample = 0.12345;
  float outsample;

  char *main_path, *delay_path_temp, *delay_path, *left_path_temp, *left_path, *module_path_temp, *module_path,
    *module1_path_temp, *module1_path, *module_out_path_temp, *module_out_path, *module1_in_path_temp, *module1_in_path, *main_in_path, *main_out_path, *module1_out_path_temp, *module1_out_path;
  struct dsp_bus *main_bus, *delay_bus, *left_bus;
  char *main_id, *delay_id, *left_id, *module_id, *module1_id, *module_out_id, *module1_in_id, *module1_out_id;

  /* grab created busses */
  main_bus = dsp_global_bus_head;
  main_id = main_bus->id;
  main_path = strconcat("/", main_id);
  delay_bus = main_bus->down;
  delay_id = delay_bus->id;
  delay_path_temp = strconcat(main_path, "/");
  delay_path = strconcat(delay_path_temp, delay_id);
  left_bus = delay_bus->down;
  left_id = left_bus->id;
  left_path_temp = strconcat(delay_path, "/");
  left_path = strconcat(left_path_temp, left_id);
  left_bus = dsp_parse_bus_path(left_path);

  module = left_bus->dsp_module_head;
  module_id = module->id;

  module_path_temp = strconcat(left_path, "?");
  module_path = strconcat(module_path_temp, module_id);
  dsp_parse_path(result, module_path);
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], left_path) == 0 &&
     strcmp(result[2], module_id) == 0)
    {}
  else {
    fprintf(stderr, " >> failed..\n");
    exit(1);
  }
  
  if( strcmp(left_bus->dsp_module_head->next->name, "block_processor") == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");  

  module1 = left_bus->dsp_module_head->next;
  module1_id = module1->id;

  module1_path_temp = strconcat(left_path, "?");
  module1_path = strconcat(module1_path_temp, module1_id);

  module_out_id = module->outs->id;
  module1_in_id = module1->ins->id;
  
  module_out_path_temp = strconcat(module_path, ">");
  module_out_path = strconcat(module_out_path_temp, module_out_id);

  module1_in_path_temp = strconcat(module1_path, "<");
  module1_in_path = strconcat(module1_in_path_temp, module1_in_id);

  /* dsp_main_inputs */
  main_in_path = strconcat("/mains{", dsp_main_ins->id);
  dsp_add_connection(main_in_path,
		     module1_in_path);

  dsp_main_ins->value = insample;
  dsp_feed_main_inputs(dsp_main_ins);  
  outsample = dsp_sum_input(module1->ins);

  module1->outs->value = outsample;

  module1_out_path_temp = strconcat(module1_path, ">");
  module1_out_id = module1->outs->id;
  module1_out_path = strconcat(module1_out_path_temp, module1_out_id);
  main_out_path = strconcat("/mains}", dsp_main_outs->id);
  dsp_add_connection(module1_out_path,
		     main_out_path);
  dsp_feed_outputs(left_path, module1_id, module1->outs);

  /* sum main in */
  outsample = 0.0;
  outsample = dsp_sum_input(dsp_main_outs);

  if( outsample == (float)0.12345 )
    {}
  else
    fprintf(stderr, " >> failed!\n");
  
  free(result[1]);
  free(result[2]);  

  fprintf(stderr, " >> success!\n");
}

void
test_recurse_dsp_graph() {
  fprintf(stderr, "  >> starting test_recurse_dsp_graph()\n");

  char *temp_path;
  
  struct dsp_bus *bus_recurse;
  char *path_bus_recurse;
  char *path_bus_recurse_module0;
  char *path_bus_recurse_in0;
  char *path_bus_recurse_module0_in;
  char *path_bus_recurse_module0_out;
  char *path_bus_recurse_out0;

  struct dsp_bus *bus0a, *bus0b, *bus0c;
  char *path_bus0a, *path_bus0b, *path_bus0c;
  char *path_bus0a_in0;
  char *path_bus0a_out0;
  
  struct dsp_bus *bus1a;
  char *path_bus1a;
  char *path_bus1a_in0;
  char *path_bus1a_module0;
  char *path_bus1a_out0;
  
  struct dsp_bus *bus2a, *bus2b;
  char *path_bus2a, *path_bus2b;
  char *path_bus2a_in0;
  char *path_bus2a_out0;
  
  struct dsp_bus *bus3a, *bus3b;
  char *path_bus3a, *path_bus3b;
  char *path_bus3a_in0;
  char *path_bus3a_module0, *path_bus3a_module1;
  char *path_bus3a_out0;
  char *path_bus3b_in0;
  char *path_bus3b_module0;
  char *path_bus3b_out0;

  char *path_mains_in0;
  char *path_mains_out0;

  float mains_out_sample0 = 0.0;

  /* cleanup dsp connection graph in preparation for tests */
  dsp_connection_list_reverse(dsp_global_connection_graph, &dsp_connection_terminate);
  dsp_global_connection_graph = NULL;

  /* create dsp busses and modules */
  bus_recurse = dsp_bus_init("recurse");
  dsp_add_bus("/", bus_recurse, "recurse_in", "recurse_out");
  
  path_bus_recurse = strconcat("/", bus_recurse->id);

  bus0a = dsp_bus_init("bus0a");
  dsp_add_bus(path_bus_recurse, bus0a, "bus0a_in", "bus0a_out");
  bus0b = dsp_bus_init("bus0b");
  dsp_add_bus(path_bus_recurse, bus0b, "bus0b_in", "bus0b_out");
  bus0c = dsp_bus_init("bus0c");
  dsp_add_bus(path_bus_recurse, bus0c, "bus0c_in", "bus0c_out");

  temp_path = strconcat(path_bus_recurse, "/");
  path_bus0a = strconcat(temp_path, bus0a->id);
  free(temp_path);
  
  bus1a = dsp_bus_init("bus1a");
  dsp_add_bus(path_bus0a, bus1a, "bus1a_in", "bus1a_out");

  temp_path = strconcat(path_bus0a, "/");
  path_bus1a = strconcat(temp_path, bus1a->id);
  free(temp_path);

  bus2a = dsp_bus_init("bus2a");
  dsp_add_bus(path_bus1a, bus2a, "bus2a_in", "bus2a_out");
  bus2b = dsp_bus_init("bus2b");
  dsp_add_bus(path_bus1a, bus2b, "bus2b_in", "bus2b_out");

  temp_path = strconcat(path_bus1a, "/");
  path_bus2a = strconcat(temp_path, bus2a->id);
  free(temp_path);
  
  temp_path = strconcat(path_bus1a, "/");
  path_bus2b = strconcat(temp_path, bus2b->id);
  free(temp_path);

  bus3a = dsp_bus_init("bus3a");
  dsp_add_bus(path_bus2b, bus3a, "bus3a_in", "bus3a_out");
  bus3b = dsp_bus_init("bus3b");
  dsp_add_bus(path_bus2b, bus3b, "bus3b_in", "bus3b_out");

  temp_path = strconcat(path_bus2b, "/");
  path_bus3a = strconcat(temp_path, bus3a->id);
  free(temp_path);

  temp_path = strconcat(path_bus2b, "/");
  path_bus3b = strconcat(temp_path, bus3b->id);
  free(temp_path);
  
  dsp_create_block_processor(bus_recurse);
  temp_path = strconcat(path_bus_recurse, "?");
  path_bus_recurse_module0 = strconcat(temp_path, bus_recurse->dsp_module_head->id);
  free(temp_path);

  dsp_create_block_processor(bus1a);
  temp_path = strconcat(path_bus1a, "?");
  path_bus1a_module0 = strconcat(temp_path, bus1a->dsp_module_head->id);
  free(temp_path);

  dsp_create_block_processor(bus3a);
  temp_path = strconcat(path_bus3a, "?");
  path_bus3a_module0 = strconcat(temp_path, bus3a->dsp_module_head->id);
  
  dsp_create_block_processor(bus3a);
  path_bus3a_module1 = strconcat(temp_path, bus3a->dsp_module_head->next->id);
  free(temp_path);
  
  dsp_create_block_processor(bus3b);
  temp_path = strconcat(path_bus3b, "?");
  path_bus3b_module0 = strconcat(temp_path, bus3b->dsp_module_head->id);
  free(temp_path);

  /* add dsp connections to the global graph */
  path_mains_in0 = strconcat("/mains{", dsp_main_ins->id);

  temp_path = strconcat(path_bus_recurse, ":");
  path_bus_recurse_in0 = strconcat(temp_path, bus_recurse->ins->id);
  free(temp_path);
  dsp_add_connection(path_mains_in0,
		     path_bus_recurse_in0);

  temp_path = strconcat(path_bus_recurse_module0, "<");
  path_bus_recurse_module0_in = strconcat(temp_path, bus_recurse->dsp_module_head->ins->id);
  free(temp_path);
  dsp_add_connection(path_bus_recurse_in0,
		     path_bus_recurse_module0_in);

  temp_path = strconcat(path_bus_recurse_module0, ">");
  path_bus_recurse_module0_out = strconcat(temp_path, bus_recurse->dsp_module_head->outs->id);
  free(temp_path);
  temp_path = strconcat(path_bus_recurse, ":");
  path_bus_recurse_out0 = strconcat(temp_path, bus_recurse->outs->id);
  dsp_add_connection(path_bus_recurse_module0_out,
		     path_bus_recurse_out0);

  temp_path = strconcat(path_bus0a, ":");
  path_bus0a_in0 = strconcat(temp_path, bus0a->ins->id);
  free(temp_path);
  dsp_add_connection(path_bus_recurse_out0,
		     path_bus0a_in0);

  temp_path = strconcat(path_bus0a, ":");
  path_bus0a_out0 = strconcat(temp_path, bus0a->outs->id);
  free(temp_path);
  dsp_add_connection(path_bus0a_in0,
		     path_bus0a_out0);

  temp_path = strconcat(path_bus1a, ":");
  path_bus1a_in0 = strconcat(temp_path, bus1a->ins->id);
  free(temp_path);
  dsp_add_connection(path_bus0a_out0,
		     path_bus1a_in0);

  temp_path = strconcat(path_bus1a, ":");
  path_bus1a_out0 = strconcat(temp_path, bus1a->outs->id);
  free(temp_path);
  dsp_add_connection(path_bus1a_in0,
		     path_bus1a_out0);

  temp_path = strconcat(path_bus2a, ":");
  path_bus2a_in0 = strconcat(temp_path, bus2a->ins->id);
  free(temp_path);
  dsp_add_connection(path_bus1a_out0,
		     path_bus2a_in0);

  temp_path = strconcat(path_bus2a, ":");
  path_bus2a_out0 = strconcat(temp_path, bus2a->outs->id);
  free(temp_path);
  dsp_add_connection(path_bus2a_in0,
		     path_bus2a_out0);

  temp_path = strconcat(path_bus3a, ":");
  path_bus3a_in0 = strconcat(temp_path, bus3a->ins->id);
  free(temp_path);
  dsp_add_connection(path_bus2a_out0,
		     path_bus3a_in0);

  temp_path = strconcat(path_bus3a, ":");
  path_bus3a_out0 = strconcat(temp_path, bus3a->outs->id);
  free(temp_path);
  dsp_add_connection(path_bus3a_in0,
		     path_bus3a_out0);

  temp_path = strconcat(path_bus3b, ":");
  path_bus3b_in0 = strconcat(temp_path, bus3b->ins->id);
  free(temp_path);
  dsp_add_connection(path_bus3a_out0,
		     path_bus3b_in0);

  temp_path = strconcat(path_bus3b, ":");
  path_bus3b_out0 = strconcat(temp_path, bus3b->outs->id);
  free(temp_path);
  dsp_add_connection(path_bus3b_in0,
		     path_bus3b_out0);

  path_mains_out0 = strconcat("/mains}", dsp_main_outs->id);
  dsp_add_connection(path_bus3b_out0,
		     path_mains_out0);

  /* add sample data to main inputs and recurse dsp graph */
  dsp_main_ins->value = 0.666;
  dsp_feed_main_inputs(dsp_main_ins);
  recurse_dsp_graph(bus_recurse, "/", 48000, 1);

  /* dequeue sample from main output and verify */
  mains_out_sample0 = 0.0;
  mains_out_sample0 = dsp_sum_input(dsp_main_outs);
  if( mains_out_sample0 > 0.665 &&
      mains_out_sample0 < 0.667)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
}

int
main(void) {
  /* don't futz with the order of these!
     tests rely on actions from previous test steps,
     necessary to test state consistency (?) */
  test_dsp_add_busses();
  test_dsp_add_modules();
  test_dsp_parse_path_bus();
  test_dsp_parse_path_bus_port();
  test_dsp_parse_path_module();
  test_dsp_parse_path_input_port();
  test_dsp_parse_path_output_port();
  test_dsp_parse_path_mains_in();
  test_dsp_parse_path_mains_out();
  test_dsp_parse_path_module_output_port();
  test_dsp_find_module();
  test_dsp_find_port_out();
  test_dsp_find_port_in();
  test_dsp_block_processor();
  test_dsp_bus_port();
  test_dsp_bus_port_ports();
  test_dsp_bus_port_port_out();
  test_dsp_bus_port_port_in();
  test_dsp_add_connection();
  test_dsp_feed_connections_bus();
  test_dsp_sum_inputs();
  test_dsp_feed_outputs();
  test_dsp_mains_allocate();
  test_dsp_feed_mains();
  test_recurse_dsp_graph();
  exit(0);
}

