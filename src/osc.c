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

osc_handler_user_defined_t* osc_handler_user_defined_init(char *osc_path, char *type_str, int num_module_ports, char **module_ports) {
  int idx;
  osc_handler_user_defined_t *new_handler = (osc_handler_user_defined_t *)malloc(sizeof(osc_handler_user_defined_t));

  new_handler->osc_path = malloc(sizeof(char) * (strlen(osc_path) + 1));
  snprintf(new_handler->osc_path, strlen(osc_path)+1, "%s", osc_path);
  
  new_handler->type_str = malloc(sizeof(char) * (strlen(type_str) + 1));
  snprintf(new_handler->type_str, strlen(type_str)+1, "%s", type_str);

  new_handler->module_ports = malloc(sizeof(char*) * num_module_ports);
  /* populate module_ports */
  for(idx=0; idx<num_module_ports; idx++) {
    new_handler->module_ports[idx] = malloc(sizeof(char) * (strlen(module_ports[idx]) + 1));
    snprintf(new_handler->module_ports[idx], strlen(module_ports[idx])+1, "%s", module_ports[idx]);
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
} /* osc_hange_address */

int osc_setup(char *osc_port_in, char *osc_port_out, char *addr_out) {
  global_osc_handlers_user_defined = NULL;
  
  send_host_out = malloc(sizeof(char) * 10);
  strcpy(send_host_out, "127.0.0.1");

  send_port_out = malloc(sizeof(char) * 6);
  strcpy(send_port_out, osc_port_out);

  lo_server_thread lo_thread = lo_server_thread_new(osc_port_in, osc_error);
  lo_server_thread_add_method(lo_thread, NULL, NULL, cyperus_osc_handler, NULL);

  lo_server_thread_start(lo_thread);
} /* osc_setup */
