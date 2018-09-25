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

void
test_dsp_add_busses() {
  fprintf(stderr, "  >> starting test_dsp_add_busses()\n");
  char *bus_path;

  dsp_global_bus_head = NULL;
  struct dsp_bus *temp_bus = NULL;

  /* create initial busses */
  fprintf(stderr, "adding bus 0\n");
  
  temp_bus = dsp_bus_init("main");
  dsp_add_bus("/", temp_bus, NULL, NULL);
  fprintf(stderr, "adding bus 1\n");  
  temp_bus = dsp_bus_init("delay");
  dsp_add_bus("/main", temp_bus, NULL, NULL);
  fprintf(stderr, "adding bus 2\n");
  temp_bus = dsp_bus_init("left");
  dsp_add_bus("/main/delay", temp_bus, NULL, NULL);
  /* grab created busses */
  bus_path = "/main/delay/left";
  temp_bus = dsp_parse_bus_path(bus_path);

  if( strcmp(temp_bus->name, "left") == 0)
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed..\n");
  return;
  }

void
test_dsp_add_modules() {
  fprintf(stderr, "  >> starting test_dsp_add_modules()\n");
  char *bus_path;
  struct dsp_bus *temp_bus;
  /* grab created busses */
  bus_path = "/main/delay/left";
  temp_bus = dsp_parse_bus_path(bus_path);
  fprintf(stderr, "temp_bus->name '%s'\n", temp_bus->name);
  dsp_create_block_processor(temp_bus);
  fprintf(stderr, "%s\n", temp_bus->dsp_module_head->name);

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
  fprintf(stderr, "resul[0]: %s, result[1]: %s, result[2]: %s\n,", result[0], result[1], result[2]);
  if(strcmp(result[0], "/") == 0 &&
     strcmp(result[1], "/main/delay/left") == 0 &&
     result[2] == "left")
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
  char *module_name = "left";
  char *result[3];
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;
  dsp_parse_path(result, "/main/delay/left?block_processor");
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/left") ==0 &&
     strcmp(result[2], "block_processor") == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");
  temp_bus = dsp_parse_bus_path(result[1]);
  temp_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);
  if( strcmp(temp_module->name, result[2]) == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");
  free(result[1]);
  free(result[2]);
}

void
test_dsp_find_port_out() {
  fprintf(stderr, "  >> starting test_dsp_find_port_out()\n");
  char *module_name = "left";
  char *result[3];
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;
  struct dsp_port_out *temp_port_out;
  dsp_parse_path(result, "/main/delay/left?block_processor");
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/left") ==0 &&
     strcmp(result[2], "block_processor") == 0)
    {}
  else {
    fprintf(stderr, " >> failed..\n");
    return;
  }
  temp_bus = dsp_parse_bus_path(result[1]);
  temp_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);
  if( strcmp(temp_module->name, result[2]) == 0)
    {}
  else {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  free(result[1]);
  free(result[2]);
  temp_port_out = dsp_find_port_out(temp_module->outs, "out");
  if( strcmp(temp_port_out->name, "out") == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");
}

void
test_dsp_find_port_in() {
  fprintf(stderr, "  >> starting test_dsp_find_port_in()\n");
  char *module_name = "left";
  char *result[3];
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;
  struct dsp_port_in *temp_port_in;
  dsp_parse_path(result, "/main/delay/left?block_processor");
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/left") ==0 &&
     strcmp(result[2], "block_processor") == 0)
    {}
  else {
    fprintf(stderr, " >> failed..\n");
    return;
  }
  temp_bus = dsp_parse_bus_path(result[1]);
  temp_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);
  if( strcmp(temp_module->name, result[2]) == 0)
    {}
  else {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  
  free(result[1]);
  free(result[2]);
  temp_port_in = dsp_find_port_in(temp_module->ins, "in");

  if( strcmp(temp_port_in->name, "in") == 0) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n");
}

void
test_dsp_block_processor() {
  fprintf(stderr, "  >> starting test_dsp_block_processor()\n");
  char *module_name = "left";
  char *result[3];
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;
  float insample;
  dsp_parse_path(result, "/main/delay/left?block_processor");
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/left") ==0 &&
     strcmp(result[2], "block_processor") == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");
  temp_bus = dsp_parse_bus_path(result[1]);
  temp_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);
  if( strcmp(temp_module->name, result[2]) == 0) {
    {}
  } else
    fprintf(stderr, " >> failed!\n");
  free(result[1]);
  free(result[2]);

  insample = 0.543210;
  rtqueue_enq(temp_module->ins->values, insample);
  dsp_block_processor(result[1], temp_module, 1, 48000);
  if( temp_module->outs->value == insample )
    fprintf(stderr, " >> success!\n");
  else
    fprintf(stderr, " >> failed!\n");
}

void
test_dsp_bus_port() {
  fprintf(stderr, "  >> starting test_dsp_bus_port()\n");
  struct dsp_bus_port *bus_port_in, *bus_port_out;
  struct dsp_bus *target_bus, *new_bus, *temp_bus;
  new_bus = dsp_bus_init("aux");
  dsp_add_bus("/main", new_bus, "in", "out");
  target_bus = dsp_parse_bus_path("/main/aux");
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
  struct dsp_bus_port *bus_port_in, *bus_port_out;
  struct dsp_bus *target_bus, *new_bus, *temp_bus;
  struct dsp_port_in *port_in;
  struct dsp_port_out *port_out;
  new_bus = dsp_bus_init("right");
  dsp_add_bus("/main/delay", new_bus, "in", "out");
  target_bus = dsp_parse_bus_path("/main/delay/right");
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
  struct dsp_bus *target_bus;
  target_bus = dsp_parse_bus_path("/main/delay/right");
  bus_port_out = target_bus->outs;
  port_out = target_bus->outs->out;

  target_bus->outs->out->value = 6.66;
  
  if( strcmp(port_out->name, "out") == 0) {
    {}
  } else
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
  bus_port_in = target_bus->ins;
  port_in = target_bus->ins->in;
  float final_value, test_value;

  test_value = 6.66;
  rtqueue_enq(target_bus->ins->in->values, test_value);
  
  if( strcmp(port_in->name, "in") == 0) {
    {}
  } else
    fprintf(stderr, " >> failed!\n");
  final_value = rtqueue_deq(target_bus->ins->in->values);
  /* account for rounding errors.. */
  if(final_value > 6.659999 && final_value < 6.660001) {
    fprintf(stderr, " >> success!\n");
  } else
    fprintf(stderr, " >> failed!\n"); 
}

void
test_dsp_bus_connection_types() {
  fprintf(stderr, "  >> starting test_dsp_bus_connection_typs()\n");
  struct dsp_bus *target_bus;
  struct dsp_bus_port *temp_bus_port;
  struct dsp_port_in *temp_port_in;
  struct dsp_port_out *temp_port_out;

  target_bus = dsp_parse_bus_path("/main/aux");  
}

void
test_dsp_add_connection() {
  fprintf(stderr, "  >> starting test_dsp_add_connection()\n");
  struct dsp_bus *aux_bus;
  struct dsp_bus *right_bus;
  struct dsp_bus_port *temp_bus_port;
  struct dsp_port_in *temp_port_in;
  struct dsp_port_out *temp_port_out;
  struct dsp_connection *connection;
  
  aux_bus = dsp_parse_bus_path("/main/aux");
  right_bus = dsp_parse_bus_path("/main/delay/right");  

  dsp_add_connection("/main/aux?out",
		     "/main/delay/right?in");

  connection = dsp_global_connection_graph;
  if( (strcmp(connection->id_out, "/main/aux?out") != 0) &&
      (strcmp(connection->id_in, "/main/delay/right?in") != 0) ) {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  fprintf(stderr, " >> success!\n");
}

void
test_dsp_feed_connections_bus() {
  fprintf(stderr, " >> starting test_dsp_feed_connections_bus()\n");
  struct dsp_bus *aux_bus;
  struct dsp_bus *right_bus;
  struct dsp_bus_port *temp_bus_port;
  struct dsp_port_in *temp_port_in;
  struct dsp_port_out *temp_port_out;
  struct dsp_connection *connection;

  aux_bus = dsp_parse_bus_path("/main/aux");
  right_bus = dsp_parse_bus_path("/main/delay/right");

  rtqueue_enq(aux_bus->outs->in->values, 0.12345);
  dsp_feed_connections_bus("/main/aux/", aux_bus->outs);
  dsp_feed_connections_bus("/main/delay/right/", right_bus->ins);
  fprintf(stderr, "right_bus->ins->out->value: %f\n", right_bus->ins->out->value);
  if( right_bus->ins->out->value != (float)0.12345 ) {
    fprintf(stderr, " >> failed!\n");
    return;
  }
  fprintf(stderr, " >> success!\n");
}

void
test_dsp_sum_inputs() {
  fprintf(stderr, " >> starting test_dsp_sum_inputs()\n");

  char *module_name = "left";
  char *result[3];
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;

  float insample = 0.12345;
  float outsample;
  
  dsp_parse_path(result, "/main/delay/left?block_processor");
  if(strcmp(result[0], "?") == 0 &&
     strcmp(result[1], "/main/delay/left") ==0 &&
     strcmp(result[2], "block_processor") == 0)
    {}
  else
    fprintf(stderr, " >> failed..\n");
  temp_bus = dsp_parse_bus_path(result[1]);
  temp_module = dsp_find_module(temp_bus->dsp_module_head, result[2]);

  rtqueue_enq(temp_module->ins->values, insample);
  outsample = dsp_sum_input(temp_module->ins);

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

  char *bus_path;
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module_in, *temp_module_out;
    
  /* grab created busses */
  bus_path = "/main/delay/right";
  temp_bus = dsp_parse_bus_path(bus_path);
  fprintf(stderr, "temp_bus->name '%s'\n", temp_bus->name);
  dsp_create_block_processor(temp_bus);
  fprintf(stderr, "%s\n", temp_bus->dsp_module_head->name);
  temp_module_in = temp_bus->dsp_module_head;
  if( strcmp(temp_module_in->name, "block_processor") == 0)
    fprintf(stderr, " >> successfully added block_processor 0!\n");
  else
    fprintf(stderr, " >> failed..\n");    

  dsp_create_block_processor(temp_bus);

  temp_module_out = temp_module_in->next;
  fprintf(stderr, "%s\n", temp_module_out->name);

  if( strcmp(temp_module_out->name, "block_processor") == 0)
    fprintf(stderr, " >> successfully added block_processor 1!\n");
  else
    fprintf(stderr, " >> failed..\n");  
  
  fprintf(stderr, " >> success!\n");
}i

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
  test_dsp_bus_connection_types();
  test_dsp_add_connection();
  test_dsp_feed_connections_bus();
  test_dsp_sum_inputs();
  test_dsp_feed_outputs();
  test_recurse_dsp_graph();
  exit(0);
}

