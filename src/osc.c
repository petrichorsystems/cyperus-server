/* osc.c
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

#include "osc.h"

char *send_host_out;
char *send_port_out;
lo_server_thread lo_thread;

osc_handler_user_defined_t *global_osc_handlers_user_defined;
pthread_mutex_t global_osc_handlers_user_defined_lock;

osc_handler_user_defined_t* osc_handler_user_defined_init(char *osc_path, char *type_str, int num_ports, char **ports) {
  int idx;
  osc_handler_user_defined_t *new_handler = (osc_handler_user_defined_t *)malloc(sizeof(osc_handler_user_defined_t));

  new_handler->osc_path = malloc(sizeof(char) * (strlen(osc_path) + 1));
  snprintf(new_handler->osc_path, strlen(osc_path)+1, "%s", osc_path);
  
  new_handler->type_str = malloc(sizeof(char) * (strlen(type_str) + 1));
  snprintf(new_handler->type_str, strlen(type_str)+1, "%s", type_str);

  new_handler->ports = malloc(sizeof(char*) * num_ports);
  /* populate ports */
  for(idx=0; idx<num_ports; idx++) {
    new_handler->ports[idx] = malloc(sizeof(char) * (strlen(ports[idx]) + 1));
    snprintf(new_handler->ports[idx], strlen(ports[idx])+1, "%s", ports[idx]);
  }
  return new_handler;
} /* osc_handler_user_defined_init */

void osc_handler_user_defined_insert_tail(osc_handler_user_defined_t *head_handler, osc_handler_user_defined_t *new_handler) {
  osc_handler_user_defined_t *temp_handler = head_handler;
  if(temp_handler == NULL) {
    head_handler = new_handler;
    return;
  }
  while(temp_handler->next != NULL) {
    temp_handler = temp_handler->next;
  }
  temp_handler->next = new_handler;
  new_handler->prev = temp_handler;
} /* osc_handler_user_defined_insert_tail */

struct dsp_port_out *parse_module_port_out(char *port_path) {
  char *bus_path, *module_path, *module_id, *temp_port_path;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *target_module = NULL;
  struct dsp_port_out *temp_port_out = NULL, *target_port_out = NULL;
  
  bus_path = malloc(sizeof(char) * (strlen(port_path) - 36 - 1 - 36));
  strncpy(bus_path, port_path, strlen(port_path) - 37 - 37);

  module_path = malloc(sizeof(char) * (strlen(port_path) - 36));
  strncpy(module_path, port_path, strlen(port_path) - 37);

  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  
  target_bus = dsp_parse_bus_path(bus_path);
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);

  temp_port_path = malloc(sizeof(char) * (strlen(port_path)+1));
  temp_port_out = target_module->outs;
  while(temp_port_out != NULL) {
    temp_port_out = temp_port_out->next;
    snprintf(temp_port_path, strlen(port_path)+1, "%s>%s", module_path, temp_port_out->id);
    if(strcmp(port_path, temp_port_path) == 0) {
      target_port_out = temp_port_out;
    }
  }
  return target_port_out;
} /* parse_module_port_out */

struct dsp_port_in *parse_module_port_in(char *port_path) {
  char *bus_path, *module_path, *module_id, *temp_port_path;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *target_module = NULL;
  struct dsp_port_in *temp_port_in = NULL, *target_port_in = NULL;
  
  bus_path = malloc(sizeof(char) * (strlen(port_path) - 36 - 1 - 36));
  strncpy(bus_path, port_path, strlen(port_path) - 37 - 37);

  module_path = malloc(sizeof(char) * (strlen(port_path) - 36));
  strncpy(module_path, port_path, strlen(port_path) - 37);

  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  
  target_bus = dsp_parse_bus_path(bus_path);
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);

  temp_port_path = malloc(sizeof(char) * (strlen(port_path)+1));
  temp_port_in = target_module->ins;
  while(temp_port_in != NULL) {
    temp_port_in = temp_port_in->next;
    snprintf(temp_port_path, strlen(port_path)+1, "%s<%s", module_path, temp_port_in->id);
    if(strcmp(port_path, temp_port_path) == 0) {
      target_port_in = temp_port_in;
    }
  }
  return target_port_in;
} /* parse_module_port_in */

void osc_execute_handler_parameter_assignment(osc_handler_user_defined_t *handler, lo_arg** argv) {
  int idx, error_code;
  char *request_id, *temp_port_path;

  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;
  
  request_id = (char *)argv[0];
  for(idx=0; idx<handler->num_ports; idx++) {
    temp_port_path = handler->ports[idx];
    if(strstr(temp_port_path, ":") != NULL) {
      /* print message, error out */      
    } else if((strstr(temp_port_path, "}") != NULL) ||
              (strstr(temp_port_path, "{") != NULL)) {
      /* print message, error out */
    } else if((strstr(temp_port_path, ">") != NULL) ||
             (strstr(temp_port_path, "<") != NULL)) {
      
    } else {
      /* throw message and error out,
         we shouldn't get here */
    }
    
    switch(handler->type_str[idx]) {
    case 'f':
      /* perform assignment of argv[idx + 1] */
      break;
    case 'i':
      /* perform assignment of (int)argv[idx + 1] */
      break;
    default:
      printf("not implemented.\n");
      break;
    }
  }

  /* send osc message reply */
  
} /* osc_execute_handler_parameter_assignment */

int osc_change_address(char *request_id, char *new_host_out, char *new_port_out) {
  free(send_host_out);
  free(send_port_out);

  send_host_out = malloc(sizeof(char) * strlen(new_host_out) + 1);
  strcpy(send_host_out, new_host_out);

  send_port_out = malloc(sizeof(char) * strlen(new_port_out) + 1);
  strcpy(send_port_out, new_port_out);
  
  lo_address lo_addr_send = lo_address_new((const char*)new_host_out, (const char*)new_port_out);
  lo_send(lo_addr_send,"/cyperus/address", "siss", request_id, 0, new_host_out, new_port_out);
  free(lo_addr_send);
  printf("changed osc server and port to: %s:%s\n", new_host_out, new_port_out);
  return 0;
} /* osc_change_address */

int osc_setup(char *osc_port_in, char *osc_port_out, char *addr_out) {
  global_osc_handlers_user_defined = NULL;

  if(pthread_mutex_init(&global_osc_handlers_user_defined_lock, NULL) != 0) {
      printf("\n mutex init failed\n");
      return 1;
  }
  
  send_host_out = malloc(sizeof(char) * 10);
  strcpy(send_host_out, "127.0.0.1");

  send_port_out = malloc(sizeof(char) * 6);
  strcpy(send_port_out, osc_port_out);

  lo_server_thread lo_thread = lo_server_thread_new(osc_port_in, osc_error);
  lo_server_thread_add_method(lo_thread, NULL, NULL, cyperus_osc_handler, NULL);

  lo_server_thread_start(lo_thread);
} /* osc_setup */
