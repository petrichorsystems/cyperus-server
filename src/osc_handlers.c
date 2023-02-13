/* osc_handlers.c
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

#include "osc_handlers.h"

void osc_error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}

int osc_address_handler(const char *path, const char *types, lo_arg **argv,
			int argc, void *data, void *user_data) {
  char *request_id = (char *) argv[0];
  char *new_host_out = (char *)argv[1];
  char *new_port_out = (char *)argv[2];
  printf("hit osc_address_handler\n");
  osc_change_address(request_id, new_host_out, new_port_out);

  return 0;
} /* osc_address_handler */

int osc_list_main_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  printf("osc_list_main_handler..\n");
  char *request_id = NULL;
  struct dsp_port_out *temp_port_out;
  struct dsp_port_in *temp_port_in;

  request_id = (char *)argv[0];
  char *mains_str = malloc(sizeof(char) * ((44 * (jackcli_channels_in +
						  jackcli_channels_out)) +
					   4 + /* strlen("in:\n") */
					   5 + /* strlen("out:\n") */
					   1));
  strcpy(mains_str, "in:\n");
  /* process main inputs */
  temp_port_out = dsp_main_ins;
  while(temp_port_out != NULL) {
    strcat(mains_str, "/mains{");
    strcat(mains_str, temp_port_out->id);
    strcat(mains_str, "\n");
    temp_port_out = temp_port_out->next;
  }
  strcat(mains_str, "out:\n");
  /* process main outputs */
  temp_port_in = dsp_main_outs;
  while(temp_port_in != NULL) {
    strcat(mains_str, "/mains}");
    strcat(mains_str, temp_port_in->id);
    strcat(mains_str, "\n");
    temp_port_in = temp_port_in->next;
  }
   
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/list/main", "sis", request_id, 0, mains_str);
  free(mains_str);
  free(lo_addr_send);
  return 0;
} /* osc_list_main_handler */

char *int_to_str(int x) {
  char *buffer = malloc(sizeof(char) * 13);
  if(buffer)
    sprintf(buffer, "%d", x);
  return buffer;
} /* int_to_str */

char *build_bus_list_str(struct dsp_bus *head_bus,
			 int root_level,
			 const char *separator,
			 int single,
			 int descendants) {
  struct dsp_bus *temp_bus = head_bus;
  char *single_result_str, *result_str = NULL;
  size_t single_result_str_size, result_str_size = 0;
  struct dsp_bus_port *temp_bus_port = NULL;
  int count_bus_ports;;
  char *bus_ins_str, *bus_outs_str;

  if( temp_bus != NULL )
    if(!root_level)
      if(descendants)
	temp_bus = temp_bus->down;
      else
	while(temp_bus->prev != NULL)
	  temp_bus = temp_bus->prev;

  /* else */
      /* 	temp_bus = temp_bus->next; */
  
  while(temp_bus != NULL) {
    /* parse inputs */
    count_bus_ports = 0;
    temp_bus_port = temp_bus->ins;
    while(temp_bus_port != NULL) {
      count_bus_ports += 1;
      temp_bus_port = temp_bus_port->next;
    }
    bus_ins_str = int_to_str(count_bus_ports);
    
    /* parse_outputs */
    count_bus_ports = 0;
    temp_bus_port = temp_bus->outs;
    while(temp_bus_port != NULL) {
      count_bus_ports += 1;
      temp_bus_port = temp_bus_port->next;
    }
    bus_outs_str = int_to_str(count_bus_ports);

    /* construct result string */
    single_result_str_size = strlen(temp_bus->id) + 1 + strlen(temp_bus->name) + 1 +
      strlen(bus_ins_str) + 1 + strlen(bus_outs_str) + 2;
    result_str_size += single_result_str_size;
    single_result_str = malloc(sizeof(char) * single_result_str_size);
    strcpy(single_result_str, temp_bus->id);
    strcat(single_result_str, separator);
    strcat(single_result_str, temp_bus->name);
    strcat(single_result_str, separator);
    strcat(single_result_str, bus_ins_str);
    strcat(single_result_str, separator);
    strcat(single_result_str, bus_outs_str);
    strcat(single_result_str, "\n");
    free(bus_ins_str);
    free(bus_outs_str);

    if(result_str == NULL) {
      result_str = malloc(sizeof(char) * single_result_str_size);
      strcpy(result_str, single_result_str);
    } else {
      result_str = realloc(result_str, sizeof(char) * (result_str_size + 1));
      strcat(result_str, single_result_str);
    }
    free(single_result_str);

    if(single)
      break;
    if(descendants)
      temp_bus = temp_bus->down;
    else
      temp_bus = temp_bus->next;
  }

  return result_str;
} /* build_bus_list_str */
			 
int osc_list_bus_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  char *request_id = NULL;
  char *path_str, *result_str = NULL;
  int list_type = 0;
  size_t result_str_size = 0;

  char *partial_result_str = NULL;
  size_t partial_result_str_size = 0;
  
  struct dsp_bus *head_bus = NULL;
  struct dsp_bus_port *temp_bus_port = NULL;
  int count_bus_ports;;
  char *bus_ins_str, *bus_outs_str;
  int root_level = 0;

  char *part_result_str = NULL;
  int more = 0;
  int current_index, last_break, last_cutoff, copy_index = 0;

  request_id = (char *)argv[0];
  path_str = (char *)argv[1];
  list_type = argv[2]->i;

  printf("path: <%s>\n", path);
  printf("path_str: %s\n", path_str);
  printf("list_type: %d\n", list_type);
  
  /* list types
     0 - peer
     1 - all peers
     2 - direct descendant
     3 - all descendants */

  if( !strcmp(path_str, "/") ||
      !strcmp(path_str, "") ) {
    head_bus = dsp_global_bus_head;
    root_level = 1;
  }
  else
    head_bus = dsp_parse_bus_path(path_str);

  if(head_bus == NULL) {
    /* no buses, return new-line char-as-str */
    result_str = "\n";
  } else {
    switch(list_type) {
    case 0: /* list peer */
      result_str = build_bus_list_str(head_bus, root_level, "|", 1, 0);
      break;
    case 1: /* list all peers */
      result_str = build_bus_list_str(head_bus, root_level, "|", 0, 0);
      break;
    case 2: /* list direct descendant */
      result_str = build_bus_list_str(head_bus, root_level, "|", 1, 1);
      break;
    case 3: /* list all direct descendants */
      result_str = build_bus_list_str(head_bus, root_level, "|", 0, 1);
      break;
    default: /* ? */
      break;
    }
  }

  if(result_str == NULL)
    result_str = "\n";

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  
  /* split bus list into 768-byte chunks */
  if(strlen(result_str) > 768) {
    more = 1;
    for(current_index=0; current_index<strlen(result_str); current_index++) {
      if(result_str[current_index] == '\n')
	last_break = current_index;
      if((current_index - last_cutoff) > 768) {	
	part_result_str = malloc(sizeof(char) * (last_break - last_cutoff + 1));
	for(copy_index=last_cutoff; copy_index<last_break; copy_index++)
	  part_result_str[copy_index - last_cutoff] = result_str[copy_index];
	part_result_str[copy_index] = '\0';
	last_cutoff = last_break;
	if(last_cutoff == strlen(result_str - 1))
	   more = 0;
	lo_send(lo_addr_send,"/cyperus/list/bus", "sisiis", request_id, 0, path_str, list_type, more, part_result_str);
	free(part_result_str);
      }
    }
  }
  if(more) {
    for(current_index=last_cutoff; current_index<strlen(result_str); current_index++) {
      result_str[current_index - last_cutoff] = result_str[current_index];
    }
    result_str[current_index - last_cutoff] = '\0';
  }
  lo_send(lo_addr_send,"/cyperus/list/bus", "sisiis", request_id, 0, path_str, list_type, 0, result_str);
  free(lo_addr_send);
  if(strcmp(result_str, "\n"))
    free(result_str);
  return 0;
} /* osc_list_bus_handler */

			 
int osc_list_bus_port_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  struct dsp_bus *temp_bus = NULL;
  char *request_id, *path_str, *result_str = NULL;
  size_t result_str_size = 0;
  struct dsp_bus_port *temp_bus_port = NULL;

  request_id = (char *)argv[0];
  path_str = (char *)argv[1];

  printf("path: <%s>\n", path);
  printf("path_str: %s\n", path_str);

  temp_bus = dsp_parse_bus_path(path_str);

  result_str_size = 4;
  result_str = malloc(sizeof(char) * (result_str_size + 1));
  strcpy(result_str, "in:\n");
  /* process main inputs */
  temp_bus_port = temp_bus->ins;
  while(temp_bus_port != NULL) {
    result_str_size += strlen(temp_bus_port->id) + 1 + strlen(temp_bus_port->name) + 2;
    result_str = realloc(result_str, sizeof(char) * result_str_size);
    strcat(result_str, temp_bus_port->id);
    strcat(result_str, "|");
    strcat(result_str, temp_bus_port->name);
    strcat(result_str, "\n");
    temp_bus_port = temp_bus_port->next;
  }

  result_str_size += 4;
  result_str = realloc(result_str, sizeof(char) * (result_str_size) + 1);
  strcat(result_str, "out:\n");
  /* process main outputs */
  temp_bus_port = temp_bus->outs;
  while(temp_bus_port != NULL) {
    result_str_size += strlen(temp_bus_port->id) + 1 + strlen(temp_bus_port->name) + 2;
    result_str = realloc(result_str, sizeof(char) * result_str_size);
    strcat(result_str, temp_bus_port->id);
    strcat(result_str, "|");
    strcat(result_str, temp_bus_port->name);
    strcat(result_str, "\n");
    temp_bus_port = temp_bus_port->next;
  }
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/list/bus_port", "siss", request_id, 0, path_str, result_str);
  free(lo_addr_send);

  free(result_str);
  return 0;
} /* osc_list_bus_port_handler */

int osc_add_bus_handler(const char *path, const char *types, lo_arg **argv,
                               int argc, void *data, void *user_data)
{
  int i;
  struct dsp_bus *temp_bus;
  char *request_id, *path_str, *bus_str, *ins_str, *outs_str, *new_id = NULL;

  struct dsp_bus *new_bus;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  path_str = (char *)argv[1];
  bus_str = (char *)argv[2];
  ins_str = (char *)argv[3];
  outs_str = (char *)argv[4];
  
  printf("path_str: %s\n", path_str);
  printf("bus_str: %s\n", bus_str);
  printf("ins_str: %s\n", ins_str);
  printf("outs_str: %s\n", outs_str);

  for(i=0; i < strlen(ins_str); i++)
    if(ins_str[i] == '|')
      ins_str[i] = ',';
  for(i=0; i < strlen(outs_str); i++)
    if(outs_str[i] == '|')
      outs_str[i] = ',';

  printf("ins_str: %s\n", ins_str);
  printf("outs_str: %s\n", outs_str);
  
  new_bus = dsp_bus_init(bus_str);
  dsp_add_bus(path_str, new_bus, ins_str, outs_str);

  new_id = malloc(sizeof(char) * strlen(new_bus->id));
  strcpy(new_id, new_bus->id);
  
  for(i=0; i < strlen(ins_str); i++)
    if(ins_str[i] == ',')
      ins_str[i] = '|';
  for(i=0; i < strlen(outs_str); i++)
    if(outs_str[i] == ',')
      outs_str[i] = '|';
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/bus", "sisssssi", request_id, 0, path_str, bus_str, ins_str, outs_str, new_id,
	  strcmp(new_bus->name, bus_str));
  free(lo_addr_send);
  free(new_id);
  
  return 0;
} /* osc_add_bus_handler */

int osc_remove_module_handler(const char *path, const char *types, lo_arg ** argv,
                                     int argc, void *data, void *user_data)
{
  int voice;
  int module_no;
  
  printf("path: <%s>\n", path);
  module_no=argv[0]->i;

  printf("removing module #%d..\n",module_no);
  
  dsp_remove_module(0,module_no);
  
  return 0;
} /* osc_remove_module_handler */


int osc_add_connection_handler(const char *path, const char *types, lo_arg **argv,
                                      int argc, void *data, void *user_data)
{
  char *request_id, *path_out, *path_in;
  int failed = 0;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  path_out = (char *)argv[1];
  path_in = (char *)argv[2];

  failed = dsp_add_connection(path_out, path_in);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/connection", "sissi", request_id, 0, path_out, path_in, failed);
  lo_address_free(lo_addr_send);

  printf("done osc_add_connection_handler()\n");
  
  return 0;
} /* osc_add_connection_handler */

int osc_remove_connection_handler(const char *path, const char *types, lo_arg **argv,
                                         int argc, void *data, void *user_data)
{
  char *request_id, *path_out, *path_in;
  int failed = 0;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  path_out = (char *)argv[1];
  path_in = (char *)argv[2];

  failed = dsp_remove_connection(path_out, path_in);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/remove/connection", "sissi", request_id, 0, path_out, path_in, failed);
  lo_address_free(lo_addr_send);

  printf("done osc_remove_connection_handler()\n");
  
  return 0;
} /* osc_remove_connection_handler */

int osc_list_modules_handler(const char *path, const char *types, lo_arg ** argv,
                                    int argc, void *data, void *user_data)
{
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *target_module = NULL;
  char *request_id = NULL;
  char *path_str = NULL;
  char *module_list = NULL;
  char *result_str = NULL;
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  path_str = (char *)argv[1];
  
  target_bus = dsp_parse_bus_path(path_str);
  target_module = target_bus->dsp_module_head;

  if(target_module) {
    result_str = malloc(sizeof(char) * 38);
    strcpy(result_str, target_module->id);
    strcat(result_str, "\n");
    target_module = target_module->next;
    while(target_module != NULL) {
      printf("'in while' -- target_module->id: %s\n", target_module->id);
      result_str = realloc(result_str, sizeof(char) * (strlen(result_str) + strlen(target_module->id) + 2));

      strcat(result_str, target_module->id);
      if( target_module->next != NULL )
        strcat(result_str, "\n");
      target_module = target_module->next;
    }
  } else {
    result_str = "";
  }

  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/list/module","sis", request_id, 0, result_str);
  free(lo_addr_send);
  if( !strcmp(result_str, "") )
    free(result_str);
  return 0;
  
} /* osc_list_modules_handler */

int osc_list_module_port_handler(const char *path, const char *types, lo_arg ** argv,
                                        int argc, void *data, void *user_data)
{
  printf("cyperus::osc_handlers.c::osc_list_module_port_handler()\n");
  int count;
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module = NULL;
  char *request_id, *bus_path, *module_id, *result_str = NULL;
  size_t result_str_size = 0;
  struct dsp_port_in *temp_port_in;
  struct dsp_port_out *temp_port_out;
  char *module_path;

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];

  printf("module_path: <%s>\n", module_path);
  
  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  snprintf(bus_path, strlen(module_path) - 36, "%s", module_path);

  printf("strlen(module_path) - 37: %d\n", (int)strlen(module_path) - 37);
  printf("bus_path: %s\n", bus_path);
  
  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);

  printf("module_id: %s\n", module_id);

  temp_bus = dsp_parse_bus_path(bus_path);

  printf("about to find module\n");
  
  
  temp_module = dsp_find_module(temp_bus->dsp_module_head, module_id);
  printf("appending result_str (or somethin)\n");
  
  result_str_size = 4;
  result_str = malloc(sizeof(char) * (result_str_size + 1));
  strcpy(result_str, "in:\n");

  printf("in\n");
  
  if(temp_module == NULL)
    printf(" NULL DUDE \n");

  temp_port_in = temp_module->ins;
  
  printf("iterating over ins\n");
  
  while(temp_port_in != NULL) {
    result_str_size += strlen(temp_port_in->id) + 1 + strlen(temp_port_in->name) + 2;
    result_str = realloc(result_str, sizeof(char) * result_str_size);
    strcat(result_str, temp_port_in->id);
    strcat(result_str, "|");
    strcat(result_str, temp_port_in->name);
    strcat(result_str, "\n");
    temp_port_in = temp_port_in->next;
  }

  result_str_size += 4;
  result_str = realloc(result_str, sizeof(char) * (result_str_size) + 1);
  strcat(result_str, "out:\n");
  
  temp_port_out = temp_module->outs;
  while(temp_port_out != NULL) {
    result_str_size += strlen(temp_port_out->id) + 1 + strlen(temp_port_out->name) + 2;
    result_str = realloc(result_str, sizeof(char) * result_str_size);
    strcat(result_str, temp_port_out->id);
    strcat(result_str, "|");
    strcat(result_str, temp_port_out->name);
    strcat(result_str, "\n");
    temp_port_out = temp_port_out->next;
  }
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/list/module_port", "siss", request_id, 0, module_path, result_str);
  lo_address_free(lo_addr_send);
  free(result_str);
  
  return 0;
  
} /* osc_list_module_port_handler */

int osc_add_modules_osc_parameter_assignment_handler(const char *path, const char *types, lo_arg ** argv, int argc, void *data, void *user_data)
{
  printf("osc_add_modules_osc_parameter_assignment()..\n");

  int idx, num_params, temp_port_path_len;
  char *request_id, *osc_path, *type_str, *temp_port_path = NULL, **port_paths;
  osc_handler_user_defined_t* new_handler;
  
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  osc_path = (char *)argv[1];
  type_str = (char *)argv[2];
  
  num_params = strlen(type_str);

  port_paths = malloc(sizeof(char*)*num_params);
  for(idx=0; idx<num_params; idx++) {
    temp_port_path = (char *)argv[3+idx];
    temp_port_path_len = strlen(temp_port_path);
    snprintf(port_paths[idx], temp_port_path_len+1, "%s", temp_port_path);
  }
  new_handler = osc_handler_user_defined_init(osc_path, type_str, num_params, port_paths);
  
  pthread_mutex_lock(&global_osc_handlers_user_defined_lock);
  osc_handler_user_defined_insert_tail(global_osc_handlers_user_defined,
                                       new_handler);
  pthread_mutex_unlock(&global_osc_handlers_user_defined_lock);
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/osc_metronome","siss", request_id, 0, osc_path, type_str);
  free(lo_addr_send);
  return 0;
} /* osc_add_modules_osc_parameter_assigment_handler */

int
osc_edit_modules_osc_parameter_assigment_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data)
{
  char *request_id, *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float beats_per_minute;
  int count;

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];
  beats_per_minute=argv[2]->f;

  /* printf("osc_edit_modules_osc_parameter_assigment_handler::beats_per_minute: %f\n", beats_per_minute); */
  
  /* bus_path = malloc(sizeof(char) * (strlen(module_path) - 36)); */
  /* strncpy(bus_path, module_path, strlen(module_path) - 37); */

  /* module_id = malloc(sizeof(char) * 37);   */
  /* strncpy(module_id, module_path + strlen(module_path) - 36, 37);  */

  /* target_bus = dsp_parse_bus_path(bus_path);   */
  /* target_module = dsp_find_module(target_bus->dsp_module_head, module_id); */

  /* dsp_edit_osc_metronome(target_module, beats_per_minute); */

  /* lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out); */
  /* lo_send(lo_addr_send,"/cyperus/edit/module/osc_metronome","sisf", request_id, 0, module_id, beats_per_minute); */
  /* free(lo_addr_send); */
  
  return 0;
} /* osc_edit_modules_osc_parameter_assigment_handler */

int osc_add_modules_osc_transmit_handler(const char *path, const char *types, lo_arg ** argv,
                                               int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  char *request_id;
  char *host;
  char *port;
  char *osc_path;

  int samplerate_divisor;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  bus_path = (char *)argv[1];
  host = (char *)argv[2];
  port = (char *)argv[3];
  osc_path = (char *)argv[4];
  samplerate_divisor = argv[5]->i;

  printf("bus_path: %s\n", bus_path);
  printf("host: %s\n", host);
  printf("port: %s\n", port);
  printf("osc_path: %s\n", osc_path);
  printf("samplerate_divisor: %d\n", samplerate_divisor);

  target_bus = dsp_parse_bus_path(bus_path);
  dsp_create_osc_transmit(target_bus, host, port, osc_path, samplerate_divisor);
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);
  printf("add_module_osc_transmit_handler, module_id: %s\n", module_id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/osc_transmit","sissssi", request_id, 0, module_id, host, port, osc_path, samplerate_divisor);
  free(lo_addr_send);

  printf("done\n");

  return 0;
} /* osc_add_modules_osc_transmit_handler */


int
osc_edit_modules_osc_transmit_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  char *request_id;
  char *host;
  char *port;
  char *osc_path;

  int samplerate_divisor;
  
  printf("path: <%s>\n", path);

  request_id = (char *)argv[0];
  module_path = (char *)argv[1];
  host = (char *)argv[2];
  port = (char *)argv[3];
  osc_path = (char *)argv[4];
  samplerate_divisor = argv[5]->i;

  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  snprintf(bus_path, strlen(module_path) - 36, "%s", module_path);

  printf("strlen(module_path) - 37: %d\n", (int)strlen(module_path) - 37);
  printf("bus_path: %s\n", bus_path);
  
  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);

  printf("module_id: %s\n", module_id);
  
  target_bus = dsp_parse_bus_path(bus_path);  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);

  dsp_edit_osc_transmit(target_module, host, port, osc_path, samplerate_divisor);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/osc_transmit","sissssi", request_id, 0, module_id, host, port, osc_path, samplerate_divisor);
  free(lo_addr_send);
  return 0;
} /* osc_edit_modules_osc_transmit_handler */


int cyperus_osc_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
  /* int i; */
  /* printf("path: <%s>\n", path); */
  /* for (i = 0; i < argc; i++) { */
  /*   printf("arg %d '%c' ", i, types[i]); */
  /*   lo_arg_pp((lo_type)types[i], argv[i]); */
  /*   printf("\n"); */
  /* } */
  int (*handler_ptr)(const char *path, const char *types, lo_arg ** argv,
                     int argc, void *data, void *user_data);
  printf("types: %s\n", types);
  handler_ptr = NULL;
  if(strcmp(path, "/cyperus/address") == 0)
    handler_ptr = osc_address_handler;
  else if(strcmp(path, "/cyperus/list/main") == 0)
    handler_ptr = osc_list_main_handler;
  else if(strcmp(path, "/cyperus/add/bus") == 0)
    handler_ptr = osc_add_bus_handler;
  else if(strcmp(path, "/cyperus/list/bus") == 0)
    handler_ptr = osc_list_bus_handler;
  else if(strcmp(path, "/cyperus/list/bus_port") == 0)
    handler_ptr = osc_list_bus_port_handler;
  
  else if(strcmp(path, "/cyperus/add/connection") == 0)
    handler_ptr = osc_add_connection_handler;
  else if(strcmp(path, "/cyperus/remove/connection") == 0)
    handler_ptr = osc_remove_connection_handler;

  else if(strcmp(path, "/cyperus/list/modules") == 0)
    handler_ptr = osc_list_modules_handler;
  else if(strcmp(path, "/cyperus/list/module_port") == 0)
    handler_ptr = osc_list_module_port_handler;
    
  else if(strcmp(path, "/cyperus/add/module/delay/simple") == 0)
    handler_ptr = osc_add_modules_delay_simple_handler;
  else if(strcmp(path, "/cyperus/edit/module/delay/simple") == 0)
    handler_ptr = osc_edit_modules_delay_simple_handler;
  
  else if(strcmp(path, "/cyperus/add/module/envelope/follower") == 0)
    handler_ptr = osc_add_modules_envelope_follower_handler;
  else if(strcmp(path, "/cyperus/edit/module/envelope/follower") == 0)
    handler_ptr = osc_edit_modules_envelope_follower_handler;
  
  else if(strcmp(path, "/cyperus/add/module/osc_transmit") == 0)
    handler_ptr = osc_add_modules_osc_transmit_handler;
  else if(strcmp(path, "/cyperus/edit/module/osc_transmit") == 0)
    handler_ptr = osc_edit_modules_osc_transmit_handler;
  
  else if(strcmp(path, "/cyperus/add/module/analysis/transient_detector") == 0)
    handler_ptr = osc_add_modules_analysis_transient_detector_handler;
  else if(strcmp(path, "/cyperus/edit/module/analysis/transient_detector") == 0)
    handler_ptr = osc_edit_modules_analysis_transient_detector_handler;

  else if(strcmp(path, "/cyperus/add/module/filter/bandpass") == 0)
    handler_ptr = osc_add_modules_filter_bandpass_handler;
  else if(strcmp(path, "/cyperus/edit/module/filter/bandpass") == 0)
    handler_ptr = osc_edit_modules_filter_bandpass_handler;
  
  else if(strcmp(path, "/cyperus/add/module/filter/moogff") == 0)
    handler_ptr = osc_add_modules_filter_moogff_handler;
  else if(strcmp(path, "/cyperus/edit/module/filter/moogff") == 0)
    handler_ptr = osc_edit_modules_filter_moogff_handler;
  
  else if(strcmp(path, "/cyperus/add/module/filter/varslope_lowpass") == 0)
    handler_ptr = osc_add_modules_filter_varslope_lowpass_handler;
  else if(strcmp(path, "/cyperus/edit/module/filter/varslope_lowpass") == 0)
    handler_ptr = osc_edit_modules_filter_varslope_lowpass_handler;

  else if(strcmp(path, "/cyperus/add/module/oscillator/sine") == 0)
    handler_ptr = osc_add_modules_oscillator_sine_handler;
  else if(strcmp(path, "/cyperus/edit/module/oscillator/sine") == 0)
    handler_ptr = osc_edit_modules_oscillator_sine_handler;

  else if(strcmp(path, "/cyperus/add/module/oscillator/sawtooth") == 0)
    handler_ptr = osc_add_modules_oscillator_sawtooth_handler;
  else if(strcmp(path, "/cyperus/edit/module/oscillator/sawtooth") == 0)
    handler_ptr = osc_edit_modules_oscillator_sawtooth_handler;

  else if(strcmp(path, "/cyperus/add/module/oscillator/triangle") == 0)
    handler_ptr = osc_add_modules_oscillator_triangle_handler;
  else if(strcmp(path, "/cyperus/edit/module/oscillator/triangle") == 0)
    handler_ptr = osc_edit_modules_oscillator_triangle_handler;
  
  else if(strcmp(path, "/cyperus/add/module/oscillator/pulse") == 0)
    handler_ptr = osc_add_modules_oscillator_pulse_handler;
  else if(strcmp(path, "/cyperus/edit/module/oscillator/pulse") == 0)
    handler_ptr = osc_edit_modules_oscillator_pulse_handler;
  
  else if(strcmp(path, "/cyperus/add/module/envelope/adsr") == 0)
    handler_ptr = osc_add_modules_envelope_adsr_handler;
  else if(strcmp(path, "/cyperus/edit/module/envelope/adsr") == 0)
    handler_ptr = osc_edit_modules_envelope_adsr_handler;

  else if(strcmp(path, "/cyperus/add/module/envelope/segment") == 0)
    handler_ptr = osc_add_modules_envelope_segment_handler;
  else if(strcmp(path, "/cyperus/edit/module/envelope/segment") == 0)
    handler_ptr = osc_edit_modules_envelope_segment_handler;

  else if(strcmp(path, "/cyperus/add/module/osc/float") == 0)
    handler_ptr = osc_add_modules_osc_float_handler;
  else if(strcmp(path, "/cyperus/edit/module/osc/float") == 0)
    handler_ptr = osc_edit_modules_osc_float_handler;

  else if(strcmp(path, "/cyperus/add/module/envelope/stdshape") == 0)
    handler_ptr = osc_add_modules_envelope_stdshape_handler;

  if(handler_ptr)
    handler_ptr(path, types, argv, argc, data, user_data);
  else {
    printf("we should raise some warning message, default handling, etc\n");
    return 1;
  }

  if(global_osc_handlers_user_defined != NULL) {
    osc_handler_user_defined_t *temp_handler;
    temp_handler = global_osc_handlers_user_defined;
    while(temp_handler != NULL) {
      if(strcmp(path, temp_handler->osc_path) == 0)
        osc_execute_handler_parameter_assignment(temp_handler, argv);
      temp_handler = temp_handler->next;
    }
  }
  
  return 0;
}
