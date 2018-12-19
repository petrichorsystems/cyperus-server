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


int osc_list_bus_port_handler(const char *path, const char *types, lo_arg **argv,
				     int argc, void *data, void *user_data)
{
  char *bus_port_path_str = argv[0];
  char *result_str = argv[1];
  incoming_message = malloc(sizeof(char) * (strlen(result_str) + 1));
  strcpy(incoming_message, result_str);
  return 0;
} /* osc_list_bus_port_handler */

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

int main(void) {
  lo_server_thread st = lo_server_thread_new("97217", error);
  lo_server_thread_add_method(st, "/cyperus/list/main", "s", osc_list_main_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/list/bus", "siis", osc_list_single_bus_handler, NULL);
  lo_server_thread_add_method(st, "/cyperus/list/bus_port", "ss", osc_list_bus_port_handler, NULL);
  lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);
  lo_server_thread_start(st);

  t = lo_address_new("127.0.0.1", "97211");
  
  //test_single_channel_passthru();
  test_single_channel_single_bus_passthru();

  while(1) {
  usleep(1000);
  }
  
  exit(0);
}

