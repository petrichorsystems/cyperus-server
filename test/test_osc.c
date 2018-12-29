/* test_osc.c
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
#include <stdlib.h> //exit(0);
#include <string.h>
#include <unistd.h>
#include <lo/lo.h>

lo_address t;

char *incoming_message;

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data) {
    int i;
    printf("path: <%s>\n", path);
    for (i = 0; i < argc; i++) {
        printf("arg %d '%c' ", i, types[i]);
        lo_arg_pp((lo_type)types[i], argv[i]);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);
    return 1;
}

int osc_list_main_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  char *msg_str = argv[0];
  incoming_message = malloc(sizeof(char) * (strlen(msg_str) + 1));
  strcpy(incoming_message, msg_str);
  return 0;
} /* osc_list_main_handler */


int osc_list_single_bus_handler(const char *path, const char *types, lo_arg **argv,
			 int argc, void *data, void *user_data)
{
  char *bus_path = argv[0];
  int list_type = argv[1]->i;
  int more = argv[2]->i;
  char *result_str = argv[3];
  incoming_message = malloc(sizeof(char) * (strlen(result_str) + 1));
  strcpy(incoming_message, result_str);
  return 0;
} /* osc_list_single_bus_handler */


int osc_add_block_processor_handler(const char *path, const char *types, lo_arg **argv,
				    int argc, void *data, void *user_data)
{
  char *block_processor_id = argv[0];
  incoming_message = malloc(sizeof(char) * (strlen(block_processor_id) + 1));
  strcpy(incoming_message, block_processor_id);
  return 0;
} /* osc_add_block_processor_handler */

int osc_add_module_sine_handler(const char *path, const char *types, lo_arg **argv,
				    int argc, void *data, void *user_data)
{
  char *sine_id = argv[0];
  incoming_message = malloc(sizeof(char) * (strlen(sine_id) + 1));
  strcpy(incoming_message, sine_id);
  return 0;
} /* osc_add_module_sine_handler */


int osc_add_module_delay_handler(const char *path, const char *types, lo_arg **argv,
				    int argc, void *data, void *user_data)
{
  printf("add_module_delay_handler!!\n");
  
  char *delay_id = argv[0];
  incoming_message = malloc(sizeof(char) * (strlen(delay_id) + 1));
  strcpy(incoming_message, delay_id);
  return 0;
} /* osc_add_module_delay_handler */


int osc_add_module_envelope_follower_handler(const char *path, const char *types, lo_arg **argv,
				    int argc, void *data, void *user_data)
{
  printf("add_module_envelope_follower_handler!!\n");
  
  char *envelope_follower_id = argv[0];
  incoming_message = malloc(sizeof(char) * (strlen(envelope_follower_id) + 1));
  strcpy(incoming_message, envelope_follower_id);
  return 0;
} /* osc_add_module_envelope_follower_handler */


int osc_list_bus_port_handler(const char *path, const char *types, lo_arg **argv,
				     int argc, void *data, void *user_data)
{
  char *bus_port_path_str = argv[0];
  char *result_str = argv[1];
  incoming_message = malloc(sizeof(char) * (strlen(result_str) + 1));
  strcpy(incoming_message, result_str);
  return 0;
} /* osc_list_bus_port_handler */


int osc_list_module_port_handler(const char *path, const char *types, lo_arg **argv,
				 int argc, void *data, void *user_data)
{
  char *module_path_str = argv[0];
  char *result_str = argv[1];
  incoming_message = malloc(sizeof(char) * (strlen(result_str) + 1));
  strcpy(incoming_message, result_str);
  return 0;
} /* osc_list_module_port_handler */

void error(int num, const char *msg, const char *path) {
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

void test_single_channel_passthru() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }
  
  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, main_out_0);
  printf("sent.\n");

  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_passthru */

void test_single_channel_single_bus_passthru() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  char *bus_id;
  char *bus_ports;
  char *bus_port_in, *bus_port_out;
  char *bus_port_in_path, *bus_port_out_path;
  char *bus_path = NULL;
  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }

  printf("sending /cyperus/add/bus / main0 in out ... \n");
  lo_send(t, "/cyperus/add/bus", "ssss", "/", "main0", "in", "out");
  printf("sent.\n");
  
  printf("sending /cyperus/list/bus / 1 ... \n");
  lo_send(t, "/cyperus/list/bus", "si", "/", 1);
  printf("sent.\n");
  usleep(500);
  bus_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_id, incoming_message);
  free(incoming_message);

  bus_path = malloc(sizeof(char) * 38);
  bus_path[0] = '/';
  for(count=0; count < 37; count++)
    bus_path[count+1] = bus_id[count];
  bus_path[count] = '\0';
  
  printf("sending /cyperus/list/bus_port %s ... \n", bus_path);
  lo_send(t, "/cyperus/list/bus_port", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  bus_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_ports, incoming_message);
  free(incoming_message);

  bus_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    bus_port_in[count - 4] = bus_ports[count];
  }

  subptr = malloc(sizeof(char) * (strlen(bus_ports) + 1));
  subptr = strstr(bus_ports, "out:");
  out_pos = subptr - bus_ports;
  bus_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    bus_port_out[count - 5 - out_pos] = bus_ports[count];
  }

  bus_port_in_path = malloc(sizeof(char) * (36 * 2 + 2));
  bus_port_out_path = malloc(sizeof(char) * (36 * 2 + 2));

  strcpy(bus_port_in_path, bus_path);
  strcat(bus_port_in_path, ":");
  strcat(bus_port_in_path, bus_port_in);
  strcpy(bus_port_out_path, bus_path);
  strcat(bus_port_out_path, ":");
  strcat(bus_port_out_path, bus_port_out);

  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, bus_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, bus_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_in_path, bus_port_out_path);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_in_path, bus_port_out_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_out_path, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_out_path, main_out_0);
  printf("sent.\n");

  free(bus_id);
  free(bus_path);
  free(bus_ports);
  free(bus_port_in);
  free(bus_port_out);
  free(bus_port_in_path);
  free(bus_port_out_path);
  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_single_bus_passthru */

void test_single_channel_single_bus_block_processor() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  char *bus_id;
  char *bus_ports;
  char *bus_port_in, *bus_port_out;
  char *bus_port_in_path, *bus_port_out_path;
  char *bus_path = NULL;
  char *block_processor_id = NULL;
  char *module_path = NULL;
  char *module_ports = NULL;
  char *module_port_in, *module_port_out;
  char *module_port_in_path, *module_port_out_path;
  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }

  printf("sending /cyperus/add/bus / main0 in out ... \n");
  lo_send(t, "/cyperus/add/bus", "ssss", "/", "main0", "in", "out");
  printf("sent.\n");
  
  printf("sending /cyperus/list/bus / 1 ... \n");
  lo_send(t, "/cyperus/list/bus", "si", "/", 1);
  printf("sent.\n");
  usleep(500);
  bus_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_id, incoming_message);
  free(incoming_message);

  bus_path = malloc(sizeof(char) * 38);
  bus_path[0] = '/';
  for(count=0; count < 37; count++)
    bus_path[count+1] = bus_id[count];
  bus_path[count] = '\0';
  
  printf("sending /cyperus/list/bus_port %s ... \n", bus_path);
  lo_send(t, "/cyperus/list/bus_port", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  bus_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_ports, incoming_message);
  free(incoming_message);

  bus_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    bus_port_in[count - 4] = bus_ports[count];
  }

  subptr = malloc(sizeof(char) * (strlen(bus_ports) + 1));
  subptr = strstr(bus_ports, "out:");
  out_pos = subptr - bus_ports;
  bus_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    bus_port_out[count - 5 - out_pos] = bus_ports[count];
  }

  bus_port_in_path = malloc(sizeof(char) * (36 * 2 + 2));
  bus_port_out_path = malloc(sizeof(char) * (36 * 2 + 2));

  strcpy(bus_port_in_path, bus_path);
  strcat(bus_port_in_path, ":");
  strcat(bus_port_in_path, bus_port_in);
  strcpy(bus_port_out_path, bus_path);
  strcat(bus_port_out_path, ":");
  strcat(bus_port_out_path, bus_port_out);

  printf("sending /cyperus/add/module/block_processor %s ... \n", bus_path);
  lo_send(t, "/cyperus/add/module/block_processor", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  block_processor_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(block_processor_id, incoming_message);
  free(incoming_message);

  module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(module_path, bus_path);
  strcat(module_path, "?");
  strcat(module_path, block_processor_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", module_path);
  lo_send(t, "/cyperus/list/module_port", "s", module_path);
  printf("sent.\n");
  usleep(500);
  module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(module_ports, incoming_message);
  free(incoming_message);

  printf("module_ports: %s\n", module_ports);
  /* add block_processor and associated ports */
  
  module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    module_port_in[count - 4] = module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(module_ports) + 1));
  subptr = strstr(module_ports, "out:");
  out_pos = subptr - module_ports;
  module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    module_port_out[count - 5 - out_pos] = module_ports[count];
  }

  module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(module_port_in_path, module_path);
  strcat(module_port_in_path, "<");
  strcat(module_port_in_path, module_port_in);
  strcpy(module_port_out_path, module_path);
  strcat(module_port_out_path, ">");
  strcat(module_port_out_path, module_port_out);

  printf("module_port_in_path: %s\n", module_port_in_path);
  printf("module_port_out_path: %s\n", module_port_out_path);
  
  
  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, bus_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, bus_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_in_path, module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_in_path, module_port_in_path);
  printf("sent.\n");

  printf("sending /cyperus/add/connection %s %s ... \n", module_port_out_path, bus_port_out_path);
  lo_send(t, "/cyperus/add/connection", "ss", module_port_out_path, bus_port_out_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_out_path, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_out_path, main_out_0);
  printf("sent.\n");

  free(bus_id);
  free(bus_path);
  free(bus_ports);
  free(bus_port_in);
  free(bus_port_out);
  free(bus_port_in_path);
  free(bus_port_out_path);
  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_single_bus_block_processor */


void test_single_channel_single_bus_delay() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  char *bus_id;
  char *bus_ports;
  char *bus_port_in, *bus_port_out;
  char *bus_port_in_path, *bus_port_out_path;
  char *bus_path = NULL;
  char *delay_id = NULL;
  char *module_path = NULL;
  char *module_ports = NULL;
  char *module_port_in, *module_port_out;
  char *module_port_in_path, *module_port_out_path;

  float amt = 1.0;
  float time = 0.75;
  float feedback = 0.5;

  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }

  printf("sending /cyperus/add/bus / main0 in out ... \n");
  lo_send(t, "/cyperus/add/bus", "ssss", "/", "main0", "in", "out");
  printf("sent.\n");
  
  printf("sending /cyperus/list/bus / 1 ... \n");
  lo_send(t, "/cyperus/list/bus", "si", "/", 1);
  printf("sent.\n");
  usleep(500);
  bus_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_id, incoming_message);
  free(incoming_message);

  bus_path = malloc(sizeof(char) * 38);
  bus_path[0] = '/';
  for(count=0; count < 37; count++)
    bus_path[count+1] = bus_id[count];
  bus_path[count] = '\0';
  
  printf("sending /cyperus/list/bus_port %s ... \n", bus_path);
  lo_send(t, "/cyperus/list/bus_port", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  bus_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_ports, incoming_message);
  free(incoming_message);

  bus_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    bus_port_in[count - 4] = bus_ports[count];
  }

  subptr = malloc(sizeof(char) * (strlen(bus_ports) + 1));
  subptr = strstr(bus_ports, "out:");
  out_pos = subptr - bus_ports;
  bus_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    bus_port_out[count - 5 - out_pos] = bus_ports[count];
  }

  bus_port_in_path = malloc(sizeof(char) * (36 * 2 + 2));
  bus_port_out_path = malloc(sizeof(char) * (36 * 2 + 2));

  strcpy(bus_port_in_path, bus_path);
  strcat(bus_port_in_path, ":");
  strcat(bus_port_in_path, bus_port_in);
  strcpy(bus_port_out_path, bus_path);
  strcat(bus_port_out_path, ":");
  strcat(bus_port_out_path, bus_port_out);

  printf("sending /cyperus/add/module/delay %s %f %f %f ... \n", bus_path, amt, time, feedback);
  lo_send(t, "/cyperus/add/module/delay", "sfff", bus_path, amt, time, feedback);
  printf("sent.\n");
  sleep(1);
  delay_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(delay_id, incoming_message);
  free(incoming_message);

  printf("delay_id: %s\n", delay_id);
  
  module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(module_path, bus_path);
  strcat(module_path, "?");
  strcat(module_path, delay_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", module_path);
  lo_send(t, "/cyperus/list/module_port", "s", module_path);
  printf("sent.\n");
  usleep(500);
  module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(module_ports, incoming_message);
  free(incoming_message);

  printf("module_ports: %s\n", module_ports);
  /* add delay and associated ports */
  
  module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    module_port_in[count - 4] = module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(module_ports) + 1));
  subptr = strstr(module_ports, "out:");
  out_pos = subptr - module_ports;
  module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    module_port_out[count - 5 - out_pos] = module_ports[count];
  }

  module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(module_port_in_path, module_path);
  strcat(module_port_in_path, "<");
  strcat(module_port_in_path, module_port_in);
  strcpy(module_port_out_path, module_path);
  strcat(module_port_out_path, ">");
  strcat(module_port_out_path, module_port_out);

  printf("module_port_in_path: %s\n", module_port_in_path);
  printf("module_port_out_path: %s\n", module_port_out_path);
  
  
  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, bus_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, bus_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_in_path, module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_in_path, module_port_in_path);
  printf("sent.\n");

  printf("sending /cyperus/add/connection %s %s ... \n", module_port_out_path, bus_port_out_path);
  lo_send(t, "/cyperus/add/connection", "ss", module_port_out_path, bus_port_out_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_out_path, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_out_path, main_out_0);
  printf("sent.\n");

  free(bus_id);
  free(bus_path);
  free(bus_ports);
  free(bus_port_in);
  free(bus_port_out);
  free(bus_port_in_path);
  free(bus_port_out_path);
  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_single_bus_delay */


void test_single_channel_single_bus_sine() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  char *bus_id;
  char *bus_ports;
  char *bus_port_in, *bus_port_out;
  char *bus_port_in_path, *bus_port_out_path;
  char *bus_path = NULL;
  char *sine_id = NULL;
  char *module_path = NULL;
  char *module_ports = NULL;
  char *module_port_in, *module_port_out;
  char *module_port_in_path, *module_port_out_path;

  float freq = 330;
  float amp = 1.0;
  float phase = 0.0;

  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }

  printf("sending /cyperus/add/bus / main0 in out ... \n");
  lo_send(t, "/cyperus/add/bus", "ssss", "/", "main0", "in", "out");
  printf("sent.\n");
  
  printf("sending /cyperus/list/bus / 1 ... \n");
  lo_send(t, "/cyperus/list/bus", "si", "/", 1);
  printf("sent.\n");
  usleep(500);
  bus_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_id, incoming_message);
  free(incoming_message);

  bus_path = malloc(sizeof(char) * 38);
  bus_path[0] = '/';
  for(count=0; count < 37; count++)
    bus_path[count+1] = bus_id[count];
  bus_path[count] = '\0';
  
  printf("sending /cyperus/list/bus_port %s ... \n", bus_path);
  lo_send(t, "/cyperus/list/bus_port", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  bus_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_ports, incoming_message);
  free(incoming_message);

  bus_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    bus_port_in[count - 4] = bus_ports[count];
  }

  subptr = malloc(sizeof(char) * (strlen(bus_ports) + 1));
  subptr = strstr(bus_ports, "out:");
  out_pos = subptr - bus_ports;
  bus_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    bus_port_out[count - 5 - out_pos] = bus_ports[count];
  }

  bus_port_in_path = malloc(sizeof(char) * (36 * 2 + 2));
  bus_port_out_path = malloc(sizeof(char) * (36 * 2 + 2));

  strcpy(bus_port_in_path, bus_path);
  strcat(bus_port_in_path, ":");
  strcat(bus_port_in_path, bus_port_in);
  strcpy(bus_port_out_path, bus_path);
  strcat(bus_port_out_path, ":");
  strcat(bus_port_out_path, bus_port_out);

  printf("sending /cyperus/add/module/sine %s %f %f %f ... \n", bus_path, freq, amp, phase);
  lo_send(t, "/cyperus/add/module/sine", "sfff", bus_path, freq, amp, phase);
  printf("sent.\n");
  sleep(1);
  sine_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(sine_id, incoming_message);
  free(incoming_message);

  printf("sine_id: %s\n", sine_id);
  
  module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(module_path, bus_path);
  strcat(module_path, "?");
  strcat(module_path, sine_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", module_path);
  lo_send(t, "/cyperus/list/module_port", "s", module_path);
  printf("sent.\n");
  usleep(500);
  module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(module_ports, incoming_message);
  free(incoming_message);

  printf("module_ports: %s\n", module_ports);
  /* add sine and associated ports */
  
  module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    module_port_in[count - 4] = module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(module_ports) + 1));
  subptr = strstr(module_ports, "out:");
  out_pos = subptr - module_ports;
  module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    module_port_out[count - 5 - out_pos] = module_ports[count];
  }

  module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(module_port_in_path, module_path);
  strcat(module_port_in_path, "<");
  strcat(module_port_in_path, module_port_in);
  strcpy(module_port_out_path, module_path);
  strcat(module_port_out_path, ">");
  strcat(module_port_out_path, module_port_out);

  printf("module_port_in_path: %s\n", module_port_in_path);
  printf("module_port_out_path: %s\n", module_port_out_path);
  
  
  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, bus_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, bus_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_in_path, module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_in_path, module_port_in_path);
  printf("sent.\n");

  printf("sending /cyperus/add/connection %s %s ... \n", module_port_out_path, bus_port_out_path);
  lo_send(t, "/cyperus/add/connection", "ss", module_port_out_path, bus_port_out_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_out_path, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_out_path, main_out_0);
  printf("sent.\n");

  free(bus_id);
  free(bus_path);
  free(bus_ports);
  free(bus_port_in);
  free(bus_port_out);
  free(bus_port_in_path);
  free(bus_port_out_path);
  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_single_bus_sine */

void test_single_channel_single_bus_sine_envelope_follower() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  char *bus_id;
  char *bus_ports;
  char *bus_port_in, *bus_port_out;
  char *bus_port_in_path, *bus_port_out_path;
  char *bus_path = NULL;

  /* sine */
  char *sine_id = NULL;
  char *module_path = NULL;
  char *module_ports = NULL;
  char *module_port_in, *module_port_out;
  char *module_port_in_path, *module_port_out_path;

  /* envelope follower */
  char *follower_id = NULL;
  char *follower_module_path = NULL;
  char *follower_module_ports = NULL;
  char *follower_module_port_in, *follower_module_port_out;
  char *follower_module_port_in_path, *follower_module_port_out_path;

  
  float freq = 0.25;
  float amp = 1.0;
  float phase = 0.0;

  float attack = 1.0;
  float decay = 1.0;
  float scale = 1.0;
  
  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }

  printf("sending /cyperus/add/bus / main0 in out ... \n");
  lo_send(t, "/cyperus/add/bus", "ssss", "/", "main0", "in", "out");
  printf("sent.\n");
  
  printf("sending /cyperus/list/bus / 1 ... \n");
  lo_send(t, "/cyperus/list/bus", "si", "/", 1);
  printf("sent.\n");
  usleep(500);
  bus_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_id, incoming_message);
  free(incoming_message);

  bus_path = malloc(sizeof(char) * 38);
  bus_path[0] = '/';
  for(count=0; count < 37; count++)
    bus_path[count+1] = bus_id[count];
  bus_path[count] = '\0';
  
  printf("sending /cyperus/list/bus_port %s ... \n", bus_path);
  lo_send(t, "/cyperus/list/bus_port", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  bus_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_ports, incoming_message);
  free(incoming_message);

  bus_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    bus_port_in[count - 4] = bus_ports[count];
  }

  subptr = malloc(sizeof(char) * (strlen(bus_ports) + 1));
  subptr = strstr(bus_ports, "out:");
  out_pos = subptr - bus_ports;
  bus_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    bus_port_out[count - 5 - out_pos] = bus_ports[count];
  }

  bus_port_in_path = malloc(sizeof(char) * (36 * 2 + 2));
  bus_port_out_path = malloc(sizeof(char) * (36 * 2 + 2));

  strcpy(bus_port_in_path, bus_path);
  strcat(bus_port_in_path, ":");
  strcat(bus_port_in_path, bus_port_in);
  strcpy(bus_port_out_path, bus_path);
  strcat(bus_port_out_path, ":");
  strcat(bus_port_out_path, bus_port_out);

  printf("sending /cyperus/add/module/sine %s %f %f %f ... \n", bus_path, freq, amp, phase);
  lo_send(t, "/cyperus/add/module/sine", "sfff", bus_path, freq, amp, phase);
  printf("sent.\n");
  sleep(1);
  sine_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(sine_id, incoming_message);
  free(incoming_message);

  printf("sine_id: %s\n", sine_id);
  
  module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(module_path, bus_path);
  strcat(module_path, "?");
  strcat(module_path, sine_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", module_path);
  lo_send(t, "/cyperus/list/module_port", "s", module_path);
  printf("sent.\n");
  sleep(1);
  module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(module_ports, incoming_message);
  free(incoming_message);

  printf("module_ports: %s\n", module_ports);
  /* add sine and associated ports */
  
  module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    module_port_in[count - 4] = module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(module_ports) + 1));
  subptr = strstr(module_ports, "out:");
  out_pos = subptr - module_ports;
  module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    module_port_out[count - 5 - out_pos] = module_ports[count];
  }

  module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(module_port_in_path, module_path);
  strcat(module_port_in_path, "<");
  strcat(module_port_in_path, module_port_in);
  strcpy(module_port_out_path, module_path);
  strcat(module_port_out_path, ">");
  strcat(module_port_out_path, module_port_out);

  printf("module_port_in_path: %s\n", module_port_in_path);
  printf("module_port_out_path: %s\n", module_port_out_path);

  /* start envelope follower */
  
  printf("sending /cyperus/add/module/envelope_follower %s %f %f %f ... \n", bus_path, freq, amp, phase);
  lo_send(t, "/cyperus/add/module/envelope_follower", "sfff", bus_path, freq, amp, phase);
  printf("sent.\n");
  sleep(1);
  sine_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(sine_id, incoming_message);
  free(incoming_message);

  printf("sine_id: %s\n", sine_id);
  
  follower_module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(follower_module_path, bus_path);
  strcat(follower_module_path, "?");
  strcat(follower_module_path, sine_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", follower_module_path);
  lo_send(t, "/cyperus/list/module_port", "s", follower_module_path);
  printf("sent.\n");
  sleep(1);
  follower_module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(follower_module_ports, incoming_message);
  free(incoming_message);

  printf("follower_module_ports: %s\n", follower_module_ports);
  /* add sine and associated ports */
  
  follower_module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    follower_module_port_in[count - 4] = follower_module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(follower_module_ports) + 1));
  subptr = strstr(follower_module_ports, "out:");
  out_pos = subptr - follower_module_ports;
  follower_module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    follower_module_port_out[count - 5 - out_pos] = follower_module_ports[count];
  }

  follower_module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  follower_module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(follower_module_port_in_path, follower_module_path);
  strcat(follower_module_port_in_path, "<");
  strcat(follower_module_port_in_path, follower_module_port_in);
  strcpy(follower_module_port_out_path, follower_module_path);
  strcat(follower_module_port_out_path, ">");
  strcat(follower_module_port_out_path, follower_module_port_out);

  printf("follower_module_port_in_path: %s\n", follower_module_port_in_path);
  printf("follower_module_port_out_path: %s\n", follower_module_port_out_path);

  /* end envelope follower */
  
  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, bus_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, bus_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_in_path, module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_in_path, module_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", module_port_out_path, follower_module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", module_port_out_path, follower_module_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", follower_module_port_out_path, bus_port_out_path);
  lo_send(t, "/cyperus/add/connection", "ss", follower_module_port_out_path, bus_port_out_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_out_path, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_out_path, main_out_0);
  printf("sent.\n");

  free(bus_id);
  free(bus_path);
  free(bus_ports);
  free(bus_port_in);
  free(bus_port_out);
  free(bus_port_in_path);
  free(bus_port_out_path);
  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_single_bus_sine_envelope_follower */


void test_single_channel_single_bus_sine_envelope_follower_delay() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  char *bus_id;
  char *bus_ports;
  char *bus_port_in, *bus_port_out;
  char *bus_port_in_path, *bus_port_out_path;
  char *bus_path = NULL;

  /* sine */
  char *sine_id = NULL;
  char *module_path = NULL;
  char *module_ports = NULL;
  char *module_port_in, *module_port_out;
  char *module_port_in_path, *module_port_out_path;

  /* envelope follower */
  char *follower_id = NULL;
  char *follower_module_path = NULL;
  char *follower_module_ports = NULL;
  char *follower_module_port_in, *follower_module_port_out;
  char *follower_module_port_in_path, *follower_module_port_out_path;

  /* delay */
  char *delay_id = NULL;
  char *delay_module_path = NULL;
  char *delay_module_ports = NULL;
  char *delay_module_port_in, *delay_module_port_out;
  char *delay_module_port_in_path, *delay_module_port_out_path;
  
  float freq = 0.25;
  float amp = 1.0;
  float phase = 0.0;

  float attack = 1.0;
  float decay = 1.0;
  float scale = 1.0;

  float amt = 1.0;
  float time = 1.0;
  float feedback = 0.8;
  
  int count;

  printf("sending /cyperus/list/main ...\n");
  lo_send(t, "/cyperus/list/main", NULL);
  printf("sent.\n");
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));

  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }

  printf("sending /cyperus/add/bus / main0 in out ... \n");
  lo_send(t, "/cyperus/add/bus", "ssss", "/", "main0", "in", "out");
  printf("sent.\n");
  
  printf("sending /cyperus/list/bus / 1 ... \n");
  lo_send(t, "/cyperus/list/bus", "si", "/", 1);
  printf("sent.\n");
  usleep(500);
  bus_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_id, incoming_message);
  free(incoming_message);

  bus_path = malloc(sizeof(char) * 38);
  bus_path[0] = '/';
  for(count=0; count < 37; count++)
    bus_path[count+1] = bus_id[count];
  bus_path[count] = '\0';
  
  printf("sending /cyperus/list/bus_port %s ... \n", bus_path);
  lo_send(t, "/cyperus/list/bus_port", "s", bus_path);
  printf("sent.\n");
  usleep(500);
  bus_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(bus_ports, incoming_message);
  free(incoming_message);

  bus_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    bus_port_in[count - 4] = bus_ports[count];
  }

  subptr = malloc(sizeof(char) * (strlen(bus_ports) + 1));
  subptr = strstr(bus_ports, "out:");
  out_pos = subptr - bus_ports;
  bus_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    bus_port_out[count - 5 - out_pos] = bus_ports[count];
  }

  bus_port_in_path = malloc(sizeof(char) * (36 * 2 + 2));
  bus_port_out_path = malloc(sizeof(char) * (36 * 2 + 2));

  strcpy(bus_port_in_path, bus_path);
  strcat(bus_port_in_path, ":");
  strcat(bus_port_in_path, bus_port_in);
  strcpy(bus_port_out_path, bus_path);
  strcat(bus_port_out_path, ":");
  strcat(bus_port_out_path, bus_port_out);

  printf("sending /cyperus/add/module/sine %s %f %f %f ... \n", bus_path, freq, amp, phase);
  lo_send(t, "/cyperus/add/module/sine", "sfff", bus_path, freq, amp, phase);
  printf("sent.\n");
  sleep(1);
  sine_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(sine_id, incoming_message);
  free(incoming_message);

  printf("sine_id: %s\n", sine_id);
  
  module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(module_path, bus_path);
  strcat(module_path, "?");
  strcat(module_path, sine_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", module_path);
  lo_send(t, "/cyperus/list/module_port", "s", module_path);
  printf("sent.\n");
  sleep(1);
  module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(module_ports, incoming_message);
  free(incoming_message);

  printf("module_ports: %s\n", module_ports);
  /* add sine and associated ports */
  
  module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    module_port_in[count - 4] = module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(module_ports) + 1));
  subptr = strstr(module_ports, "out:");
  out_pos = subptr - module_ports;
  module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    module_port_out[count - 5 - out_pos] = module_ports[count];
  }

  module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(module_port_in_path, module_path);
  strcat(module_port_in_path, "<");
  strcat(module_port_in_path, module_port_in);
  strcpy(module_port_out_path, module_path);
  strcat(module_port_out_path, ">");
  strcat(module_port_out_path, module_port_out);

  printf("module_port_in_path: %s\n", module_port_in_path);
  printf("module_port_out_path: %s\n", module_port_out_path);

  /* start envelope follower */
  
  printf("sending /cyperus/add/module/envelope_follower %s %f %f %f ... \n", bus_path, freq, amp, phase);
  lo_send(t, "/cyperus/add/module/envelope_follower", "sfff", bus_path, freq, amp, phase);
  printf("sent.\n");
  sleep(1);
  sine_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(sine_id, incoming_message);
  free(incoming_message);

  printf("sine_id: %s\n", sine_id);
  
  follower_module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(follower_module_path, bus_path);
  strcat(follower_module_path, "?");
  strcat(follower_module_path, sine_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", follower_module_path);
  lo_send(t, "/cyperus/list/module_port", "s", follower_module_path);
  printf("sent.\n");
  sleep(1);
  follower_module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(follower_module_ports, incoming_message);
  free(incoming_message);

  printf("follower_module_ports: %s\n", follower_module_ports);
  /* add sine and associated ports */
  
  follower_module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    follower_module_port_in[count - 4] = follower_module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(follower_module_ports) + 1));
  subptr = strstr(follower_module_ports, "out:");
  out_pos = subptr - follower_module_ports;
  follower_module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    follower_module_port_out[count - 5 - out_pos] = follower_module_ports[count];
  }

  follower_module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  follower_module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));
  
  strcpy(follower_module_port_in_path, follower_module_path);
  strcat(follower_module_port_in_path, "<");
  strcat(follower_module_port_in_path, follower_module_port_in);
  strcpy(follower_module_port_out_path, follower_module_path);
  strcat(follower_module_port_out_path, ">");
  strcat(follower_module_port_out_path, follower_module_port_out);

  printf("follower_module_port_in_path: %s\n", follower_module_port_in_path);
  printf("follower_module_port_out_path: %s\n", follower_module_port_out_path);

  /* end envelope follower */

  /* start delay module */

  printf("sending /cyperus/add/module/delay %s %f %f %f ... \n", bus_path, amt, time, feedback);
  lo_send(t, "/cyperus/add/module/delay", "sfff", bus_path, amt, time, feedback);
  printf("sent.\n");
  sleep(1);
  delay_id = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(delay_id, incoming_message);
  free(incoming_message);

  printf("delay_id: %s\n", delay_id);
  
  delay_module_path = malloc(sizeof(char) * (strlen(bus_path) + 38));
  strcpy(delay_module_path, bus_path);
  strcat(delay_module_path, "?");
  strcat(delay_module_path, delay_id);
  
  printf("sending /cyperus/list/module_port %s ... \n", delay_module_path);
  lo_send(t, "/cyperus/list/module_port", "s", delay_module_path);
  printf("sent.\n");
  usleep(500);
  delay_module_ports = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(delay_module_ports, incoming_message);
  free(incoming_message);

  printf("delay_module_ports: %s\n", delay_module_ports);
  /* add delay and associated ports */
  
  delay_module_port_in = malloc(sizeof(char) * 37);
  for(count=4; count<40; count++) {
    delay_module_port_in[count - 4] = delay_module_ports[count];
  }
  
  subptr = malloc(sizeof(char) * (strlen(delay_module_ports) + 1));
  subptr = strstr(delay_module_ports, "out:");
  out_pos = subptr - delay_module_ports;
  delay_module_port_out = malloc(sizeof(char) * 37);
  for(count=out_pos+5; count<out_pos+36+5; count++) {
    delay_module_port_out[count - 5 - out_pos] = delay_module_ports[count];
  }

  delay_module_port_in_path = malloc(sizeof(char) * (36 * 3 + 3));
  delay_module_port_out_path = malloc(sizeof(char) * (36 * 3 + 3));

  strcpy(delay_module_port_in_path, delay_module_path);
  strcat(delay_module_port_in_path, "<");
  strcat(delay_module_port_in_path, delay_module_port_in);
  strcpy(delay_module_port_out_path, delay_module_path);
  strcat(delay_module_port_out_path, ">");
  strcat(delay_module_port_out_path, delay_module_port_out);

  printf("delay_module_port_in_path: %s\n", delay_module_port_in_path);
  printf("delay_module_port_out_path: %s\n", delay_module_port_out_path);
  
  /* end delay module */
  
  printf("sending /cyperus/add/connection %s %s ... \n", main_in_0, bus_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, bus_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_in_path, module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_in_path, module_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", module_port_out_path, follower_module_port_in_path);
  lo_send(t, "/cyperus/add/connection", "ss", module_port_out_path, follower_module_port_in_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", follower_module_port_out_path, bus_port_out_path);
  lo_send(t, "/cyperus/add/connection", "ss", follower_module_port_out_path, bus_port_out_path);
  printf("sent.\n");
  
  printf("sending /cyperus/add/connection %s %s ... \n", bus_port_out_path, main_out_0);
  lo_send(t, "/cyperus/add/connection", "ss", bus_port_out_path, main_out_0);
  printf("sent.\n");

  free(bus_id);
  free(bus_path);
  free(bus_ports);
  free(bus_port_in);
  free(bus_port_out);
  free(bus_port_in_path);
  free(bus_port_out_path);
  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_single_channel_single_bus_sine_envelope_follower_delay */

int main(void) {
  lo_server_thread st = lo_server_thread_new("97217", error);
  lo_server_thread_add_method(st, "/cyperus/list/main", "s", osc_list_main_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/list/bus", "siis", osc_list_single_bus_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/list/bus_port", "ss", osc_list_bus_port_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/list/module_port", "ss", osc_list_module_port_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/add/module/block_processor", "s", osc_add_block_processor_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/add/module/delay", "sfff", osc_add_module_delay_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/add/module/sine", "sfff", osc_add_module_sine_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/add/module/envelope_follower", "sfff", osc_add_module_envelope_follower_handler, NULL);
  lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);
  lo_server_thread_start(st);

  t = lo_address_new("127.0.0.1", "97211");
  
  //test_single_channel_passthru();
  //test_single_channel_single_bus_passthru();
  //test_single_channel_single_bus_block_processor();
  //test_single_channel_single_bus_delay();
  //test_single_channel_single_bus_sine();
  //test_single_channel_single_bus_sine_envelope_follower();
  test_single_channel_single_bus_sine_envelope_follower_delay();

  usleep(1000);
  
  exit(0);
}

