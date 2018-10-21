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
#include "../rtqueue.h"


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
    fprintf(stderr, " >> success!\n");
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
    fprintf(stderr, " >> success!\n");
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

  printf("CREATED CONNECTION--\nmain_aux_out_path: %s\ndelay_left_in_path: %s\n\n", main_aux_out_path,
	 delay_left_in_path);
  
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
  fprintf(stderr, " >> starting test_dsp_feed_connections_bus()\n");

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
  fprintf(stderr, "aux_bus->outs: %s:%s\n", aux_path, aux_bus->outs->id);
  dsp_feed_connections_bus(aux_path, aux_bus->outs);
  dsp_feed_connections_bus(left_path, left_bus->ins);
  fprintf(stderr, "right_bus->ins->out->value: %f\n", left_bus->ins->out->value);
  if( left_bus->ins->out->value != (float)0.12345 ) {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  fprintf(stderr, " >> success!\n");
}

void
test_dsp_sum_inputs() {
  fprintf(stderr, " >> starting test_dsp_sum_inputs()\n");

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

  fprintf(stderr, "module->id: %s\n", module->id);
  fprintf(stderr, "left_path: %s\n", left_path);
  module_path_temp = strconcat(left_path, "?");
  module_path = strconcat(module_path_temp, module_id);
  dsp_parse_path(result, module_path);
  fprintf(stderr, "type: %s, left_path: %s, module_id: %s\n", result[0], result[1], result[2]);
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

  fprintf(stderr, "outsample: %f\n", outsample);

  if( outsample == (float)0.12345 ) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");
  
  free(result[1]);
  free(result[2]);  

  fprintf(stderr, " >> success!\n");
}

void
test_dsp_feed_outputs() {
  fprintf(stderr, " >> starting test_dsp_feed_outputs()\n");

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

  fprintf(stderr, "module->id: %s\n", module->id);
  fprintf(stderr, "left_path: %s\n", left_path);
  module_path_temp = strconcat(left_path, "?");
  module_path = strconcat(module_path_temp, module_id);
  dsp_parse_path(result, module_path);
  fprintf(stderr, "type: %s, left_path: %s, module_id: %s\n", result[0], result[1], result[2]);
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
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");  

  module1 = left_bus->dsp_module_head->next;
  module1_id = module1->id;

  printf("module_id: %s, module1_id: %s\n", module->id, module1->id);

  module1_path_temp = strconcat(left_path, "?");
  module1_path = strconcat(module1_path_temp, module1_id);

  module_out_id = module->outs->id;
  module1_in_id = module1->ins->id;
  
  module_out_path_temp = strconcat(module_path, ">");
  module_out_path = strconcat(module_out_path_temp, module_out_id);

  module1_in_path_temp = strconcat(module1_path, "<");
  module1_in_path = strconcat(module1_in_path_temp, module1_in_id);

  printf("module_out_path: %s\n", module_out_path);
  printf("module1_in_path: %s\n", module1_in_path);
  
  dsp_add_connection(module_out_path,
		     module1_in_path);

  rtqueue_enq(module->ins->values, insample);
  outsample = dsp_sum_input(module->ins);
  module->outs->value = outsample;
  dsp_feed_outputs(left_path, module_id, module->outs);
  outsample = dsp_sum_input(module1->ins);
  fprintf(stderr, "outsample: %f\n", outsample);

  if( outsample == (float)0.12345 ) {
  } else
    fprintf(stderr, " >> failed!\n");
  
  free(result[1]);
  free(result[2]);  

  fprintf(stderr, " >> success!\n");
}

void
test_recurse_dsp_graph() {
  fprintf(stderr, " >> starting test_recurse_dsp_graph()\n");

  // do we have to map out the entire graph?
  // -- probably, need enumerated expectations
  
  fprintf(stderr, " >> success!\n");
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
  test_recurse_dsp_graph();
  exit(0);
}

