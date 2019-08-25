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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);

#include "cyperus.h"
#include "rtqueue.h"
#include "dsp_math.h"
#include "dsp.h"
#include "dsp_types.h"
#include "dsp_ops.h"
#include "jackcli.h"
#include "osc.h"
#include "osc_handlers.h"

void osc_error(int num, const char *msg, const char *path)
{
  printf("liblo server error %d in path %s: %s\n", num, path, msg);
  fflush(stdout);
}

/* for debugging,
 * catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
  int i;
  
  printf("path: <%s>\n", path);
  for (i = 0; i < argc; i++) {
    printf("arg %d '%c' ", i, types[i]);
    lo_arg_pp((lo_type)types[i], argv[i]);
    printf("\n");
  }
  return 0;
}


int osc_address_handler(const char *path, const char *types, lo_arg **argv,
			int argc, void *data, void *user_data)
{
  char *new_host_out = argv[0];
  char *new_port_out = argv[1];
  printf("hit osc_address_handler\n");
  osc_change_address(new_host_out, new_port_out);
  return 0;
} /* osc_address_handler */

int osc_list_main_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  struct dsp_port_out *temp_port_out;
  struct dsp_port_in *temp_port_in;
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
  lo_send(lo_addr_send,"/cyperus/list/main", "s", mains_str);
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

  if(!root_level)
    if(descendants)
      temp_bus = temp_bus->down;
    else
      temp_bus = temp_bus->next;
  
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
  
  path_str = argv[0];
  list_type = argv[1]->i;

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
	lo_send(lo_addr_send,"/cyperus/list/bus", "siis", path_str, list_type, more, part_result_str);
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
  lo_send(lo_addr_send,"/cyperus/list/bus", "siis", path_str, list_type, 0, result_str);
  free(lo_addr_send);
  if(strcmp(result_str, "\n"))
    free(result_str);
  return 0;
} /* osc_list_bus_handler */

			 
int osc_list_bus_port_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  struct dsp_bus *temp_bus = NULL;
  char *path_str, *result_str = NULL;
  size_t result_str_size = 0;
  struct dsp_bus_port *temp_bus_port = NULL;

  path_str = argv[0];

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
  lo_send(lo_addr_send,"/cyperus/list/bus_port", "ss", path_str, result_str);
  free(lo_addr_send);

  free(result_str);
  return 0;
} /* osc_list_bus_port_handler */

int osc_add_bus_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  int i;
  struct dsp_bus *temp_bus;
  char *path_str, *bus_str, *ins_str, *outs_str = NULL;

  struct dsp_bus *new_bus;
  
  printf("path: <%s>\n", path);

  path_str = argv[0];
  bus_str = argv[1];
  ins_str = argv[2];
  outs_str = argv[3];
  
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

  for(i=0; i < strlen(ins_str); i++)
    if(ins_str[i] == ',')
      ins_str[i] = '|';
  for(i=0; i < strlen(outs_str); i++)
    if(outs_str[i] == ',')
      outs_str[i] = '|';
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/bus", "ssssi", path_str, bus_str, ins_str, outs_str,
	  strcmp(new_bus->name, bus_str));
  free(lo_addr_send);
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
  char *path_out, *path_in;
  int failed = 0;
  
  printf("path: <%s>\n", path);

  path_out = argv[0];
  path_in = argv[1];

  failed = dsp_add_connection(path_out, path_in);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/connection", "ssi", path_out, path_in, failed);
  lo_address_free(lo_addr_send);

  printf("done osc_add_connection_handler()\n");
  
  return 0;
} /* osc_add_connection_handler */

int osc_list_modules_handler(const char *path, const char *types, lo_arg ** argv,
			     int argc, void *data, void *user_data)
{
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *target_module = NULL;
  char *path_str = NULL;
  char *module_list = NULL;
  char *result_str = NULL;
  printf("path: <%s>\n", path);

  path_str = argv[0];
  
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
  lo_send(lo_addr_send,"/cyperus/list/module","s", result_str);
  free(lo_addr_send);
  if( !strcmp(result_str, "") )
    free(result_str);
  return 0;
  
} /* osc_list_modules_handler */

int osc_list_module_port_handler(const char *path, const char *types, lo_arg ** argv,
			     int argc, void *data, void *user_data)
{
  int count;
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module = NULL;
  char *path_str, *bus_path, *module_id, *result_str = NULL;
  size_t result_str_size = 0;
  struct dsp_port_in *temp_port_in;
  struct dsp_port_out *temp_port_out;

  path_str = argv[0];

  bus_path = malloc(sizeof(char) * (strlen(path_str) - 37));
  for(count=0; count<strlen(path_str)-37; count++)
    bus_path[count] = path_str[count];

  module_id = malloc(sizeof(char) * 37);
  printf("strlen(path_str): %d\n", strlen(path_str));
  for(count=strlen(path_str)-36; count<strlen(path_str); count++) {
    module_id[count - 38] = path_str[count];
  }
  printf("\n");
  module_id[36] = '\0';

  
  temp_bus = dsp_parse_bus_path(bus_path);
  temp_module = dsp_find_module(temp_bus->dsp_module_head, module_id);
  
  result_str_size = 4;
  result_str = malloc(sizeof(char) * (result_str_size + 1));
  strcpy(result_str, "in:\n");
  
  temp_port_in = temp_module->ins;
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
  lo_send(lo_addr_send,"/cyperus/list/module_port", "ss", path_str, result_str);
  lo_address_free(lo_addr_send);
  free(result_str);
  
  return 0;
  
} /* osc_list_module_port_handler */

int osc_add_module_block_processor_handler(const char *path, const char *types, lo_arg ** argv,
					   int argc, void *data, void *user_data)
{
  char *bus_path, *module_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;
  printf("path: <%s>\n", path);

  bus_path = argv[0];
  
  target_bus = dsp_parse_bus_path(bus_path);
  dsp_create_block_processor(target_bus);
    
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/block_processor","s", module_id);
  free(lo_addr_send);
  return 0;
} /* osc_add_module_block_processor_handler */


int osc_add_module_delay_handler(const char *path, const char *types, lo_arg ** argv,
				 int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float amt;
  float time;
  float feedback;
  
  printf("path: <%s>\n", path);

  bus_path = argv[0];
  amt=argv[1]->f;
  time=argv[2]->f;
  feedback=argv[3]->f;

  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_delay(target_bus, amt, time, feedback);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("add_module_delay_handler, module_id: %s\n", module_id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/delay","sfff", module_id, amt, time, feedback);
  free(lo_addr_send);
  return 0;
} /* osc_add_module_delay_handler */


int
osc_edit_module_delay_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float amt;
  float time;
  float feedback;
  int count;

  printf("path: <%s>\n", path);
  
  module_path = argv[0];
  amt=argv[1]->f;
  time=argv[2]->f;
  feedback=argv[3]->f;

  printf("module_path: %s\n", module_path);
  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);

  module_id = malloc(sizeof(char) * 37);  
  strncpy(module_id, module_path + strlen(module_path) - 36, 37); 

  target_bus = dsp_parse_bus_path(bus_path);  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);

  dsp_edit_delay(target_module, amt, time, feedback);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/delay","sfff", module_id, amt, time, feedback);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_module_delay_handler */

int osc_add_module_sine_handler(const char *path, const char *types, lo_arg ** argv,
				 int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float freq;
  float amp;
  float phase;
  
  printf("path: <%s>\n", path);

  bus_path = argv[0];
  freq=argv[1]->f;
  amp=argv[2]->f;
  phase=argv[3]->f;

  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_sine(target_bus, freq, amp, phase);
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("add_module_sine_handler, module_id: %s\n", module_id);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/sine","sfff", module_id, freq, amp, phase);
  free(lo_addr_send);

  return 0;
} /* osc_add_module_sine_handler */


int
osc_edit_module_sine_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  char *module_path = NULL;
  char *module_id = NULL;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float freq;
  float amp;
  float phase;
  int count;

  printf("path: <%s>\n", path);

  module_path = argv[0];
  freq=argv[1]->f;
  amp=argv[2]->f;
  phase=argv[3]->f;
  
  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);

  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  
  dsp_edit_sine(target_module, freq, amp, phase);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/sine","sfff", module_id, freq, amp, phase);
  free(lo_addr_send);

  return 0;
} /* osc_edit_module_sine_handler */

int osc_add_module_square_handler(const char *path, const char *types, lo_arg ** argv,
                                  int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float freq;
  float amp;
  
  printf("path: <%s>\n", path);

  bus_path=argv[0];
  freq=argv[1]->f;
  amp=argv[2]->f;

  target_bus = dsp_parse_bus_path(bus_path);
  dsp_create_square(target_bus, freq, amp);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);
  
  printf("creating square wave at freq %f and amp %f, module_id %s..\n",freq, amp, module_id);

  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/square","sff", module_id, freq, amp);
  free(lo_addr_send);
  
  return 0;
} /* osc_create_module_square_handler */

int osc_edit_module_square_handler(const char *path, const char *types, lo_arg ** argv,
                                   int argc, void *data, void *user_data)
{
    char *module_path = NULL;
  char *module_id = NULL;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float freq;
  float amp;

  int count;

  printf("path: <%s>\n", path);

  module_path = argv[0];
  freq=argv[1]->f;
  amp=argv[2]->f;
  
  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);

  module_id = malloc(sizeof(char) * 37);
  strncpy(module_id, module_path + strlen(module_path) - 36, 37);
  
  target_bus = dsp_parse_bus_path(bus_path);
  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  
  dsp_edit_square(target_module, freq, amp);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/square","sff", module_id, freq, amp);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_module_square_handler */


int osc_add_module_envelope_follower_handler(const char *path, const char *types, lo_arg ** argv,
				 int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float attack;
  float decay;
  float scale;
  
  printf("path: <%s>\n", path);

  bus_path = argv[0];
  attack=argv[1]->f;
  decay=argv[2]->f;
  scale=argv[3]->f;

  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_envelope_follower(target_bus, attack, decay, scale);
  
  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("add_module_envelope_follower_handler, module_id: %s\n", module_id);
    lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/envelope_follower","sfff", module_id, attack, decay, scale);
  free(lo_addr_send);
  
  return 0;
} /* osc_add_module_envelope_follower_handler */


int
osc_edit_module_envelope_follower_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  float attack;
  float decay;
  float scale;
  int count;
  printf("path: <%s>\n", path);

  module_path = argv[0];
  attack=argv[1]->f;
  decay=argv[2]->f;
  scale=argv[3]->f;

  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  for(count=0; count < (strlen(module_path) - 38); count++)
    bus_path[count] = module_path[count];

  module_path = malloc(sizeof(char) * 37);
  for(count=strlen(module_path) - 37; count < strlen(module_path) + 1; count++) {
    module_id[count - strlen(module_path) - 37] = module_path[count];
  }
  
  target_bus = dsp_parse_bus_path(bus_path);
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);

  dsp_edit_envelope_follower(target_module, attack, decay, scale);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/envelope_follower","sfff", module_id, attack, decay, scale);
  free(lo_addr_send);
  return 0;
} /* osc_edit_module_envelope_follower_handler */

int osc_add_module_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
                                                      int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float freq;
  float q;
  float amp;
  
  printf("path: <%s>\n", path);

  bus_path = argv[0];
  amp=argv[1]->f;
  freq=argv[2]->f;
  q=argv[3]->f;

  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_lowpass(target_bus, amp, freq, q);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);
  
  printf("creating butterworth biquad lowpass filter at freq cutoff %f and qonance %f..\n", freq, q);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/lowpass","sfff", module_id, amp, freq, q);
  free(lo_addr_send);

  return 0;
} /* osc_create_module_lowpass_handler */

int osc_edit_module_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
                                                       int argc, void *data, void *user_data)
{
  char *module_path, *module_id = NULL;
  char *bus_path = NULL;
  struct dsp_bus *target_bus = NULL;;
  struct dsp_module *target_module = NULL;
  float amt = 0.0;
  float freq = 0.0;
  float q = 0.0;
  int count = 0;

  printf("path: <%s>\n", path);
  
  module_path = argv[0];
  amt=argv[1]->f;
  freq=argv[2]->f;
  q=argv[3]->f;

  printf("module_path: %s\n", module_path);
  
  /* split up path */

  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);
  
  module_id = malloc(sizeof(char) * 37);  
  strncpy(module_id, module_path + strlen(module_path) - 36, 37); 
 
  target_bus = dsp_parse_bus_path(bus_path);
  if(target_bus == NULL) {
    target_bus = dsp_parse_bus_path(bus_path);
    printf("\n\n\nwhy the fuck is the target_bus null??\n\n\n");
    printf("dsp_global_bus_head->id: %s\n", dsp_global_bus_head->id);
    printf("bus_path: %s\n", bus_path);
    printf("strlen(bus_path): %d\n", strlen(bus_path));
    printf("\n");
    lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
    lo_send(lo_addr_send,"/cyperus/edit/module/lowpass","sfff", module_id, amt, freq, q);
    free(lo_addr_send);
    exit(1);
    return 1;
  }  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  
  dsp_edit_lowpass(target_module, amt, freq, q);
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/lowpass","sfff", module_id, amt, freq, q);
  free(lo_addr_send);

  free(bus_path);
  free(module_id);
 
  return 0;
} /* osc_edit_module_lowpass_handler */

int osc_add_module_highpass_handler(const char *path, const char *types, lo_arg ** argv,
                                                      int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float freq;
  float amp;
  
  printf("path: <%s>\n", path);

  bus_path = argv[0];
  amp=argv[1]->f;
  freq=argv[2]->f;

  target_bus = dsp_parse_bus_path(bus_path);  
  dsp_create_highpass(target_bus, amp, freq);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/highpass","sff", module_id, amp, freq);
  free(lo_addr_send);

  free(module_id);
  
  return 0;
} /* osc_create_module_highpass_handler */

int osc_edit_module_highpass_handler(const char *path, const char *types, lo_arg ** argv,
                                                       int argc, void *data, void *user_data)
{
  char *module_path, *module_id = NULL;
  char *bus_path = NULL;
  struct dsp_bus *target_bus = NULL;;
  struct dsp_module *target_module = NULL;
  float amt = 0.0;
  float freq = 0.0;
  int count = 0;

  printf("path: <%s>\n", path);
  
  module_path = argv[0];
  amt=argv[1]->f;
  freq=argv[2]->f;

  printf("module_path: %s\n", module_path);
  
  /* split up path */

  
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);
  
  module_id = malloc(sizeof(char) * 37);  
  strncpy(module_id, module_path + strlen(module_path) - 36, 37); 
 
  target_bus = dsp_parse_bus_path(bus_path);
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
  
  dsp_edit_highpass(target_module, amt, freq);
  
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/edit/module/highpass","sff", module_id, amt, freq);
  free(lo_addr_send);

  
  free(module_id);
  free(bus_path);
 
  return 0;
} /* osc_edit_module_highpass_handler */


int osc_add_module_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
				int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float amt;
  float shift;
  float mix;
  
  printf("path: <%s>\n", path);
  
  bus_path = argv[0];
  amt=argv[1]->f;
  shift=argv[2]->f;
  mix=argv[3]->f;
  
  printf("creating pitch shift with amount %f, shift %f, and mix %f..\n",amt,shift,mix);
  
  target_bus = dsp_parse_bus_path(bus_path); 
  dsp_create_pitch_shift(target_bus, amt, shift, mix);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("add_module_pitch_shift_handler, module_id: %s\n", module_id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/pitch_shift","sfff", module_id, amt, shift, mix);
  free(lo_addr_send);
  
  return 0;
} /* osc_add_module_pitch_shift_handler */

int
osc_edit_module_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  float amt;
  float shift;
  float mix;

  int count;
  
  printf("path: <%s>\n", path);
  
  module_path = argv[0];
  amt=argv[1]->f;
  shift=argv[2]->f;
  mix=argv[3]->f;

  printf("module_path: %s\n", module_path);
  
  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);

  module_id = malloc(sizeof(char) * 37);  
  strncpy(module_id, module_path + strlen(module_path) - 36, 37); 

  target_bus = dsp_parse_bus_path(bus_path);  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
    
  printf("module_id %s, editing pitch_shift of amount %f, shift %f of 1octave, mix %f..\n",module_id,amt,shift,mix);

  dsp_edit_pitch_shift(target_module, amt, shift, mix);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/pitch_shift","sfff", module_id, amt, shift, mix);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_pitch_shift_handler */



int osc_add_module_karlsen_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
				int argc, void *data, void *user_data)
{
  char *bus_path, *module_id = NULL;
  struct dsp_bus *target_bus = NULL;
  struct dsp_module *temp_module, *target_module = NULL;

  float amt;
  float freq;
  float res;
  
  printf("path: <%s>\n", path);
  
  bus_path = argv[0];
  amt=argv[1]->f;
  freq=argv[2]->f;
  res=argv[3]->f;
  
  printf("creating pitch freq with amount %f, freq %f, and res %f..\n",amt,freq,res);
  
  target_bus = dsp_parse_bus_path(bus_path); 
  dsp_create_karlsen_lowpass(target_bus, amt, freq, res);

  temp_module = target_bus->dsp_module_head;
  while(temp_module != NULL) {
    target_module = temp_module;
    temp_module = temp_module->next;
  }
  module_id = malloc(sizeof(char) * 37);
  strcpy(module_id, target_module->id);

  printf("add_module_karlsen_lowpass_handler, module_id: %s\n", module_id);
  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/karlsen_lowpass","sfff", module_id, amt, freq, res);
  free(lo_addr_send);
  
  return 0;
} /* osc_add_module_karlsen_lowpass_handler */

int
osc_edit_module_karlsen_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  char *module_path, *module_id;
  char *bus_path;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;

  float amt;
  float freq;
  float res;

  int count;
  
  printf("path: <%s>\n", path);
  
  module_path = argv[0];
  amt=argv[1]->f;
  freq=argv[2]->f;
  res=argv[3]->f;

  printf("module_path: %s\n", module_path);
  
  /* split up path */
  bus_path = malloc(sizeof(char) * (strlen(module_path) - 36));
  strncpy(bus_path, module_path, strlen(module_path) - 37);

  module_id = malloc(sizeof(char) * 37);  
  strncpy(module_id, module_path + strlen(module_path) - 36, 37); 

  target_bus = dsp_parse_bus_path(bus_path);  
  target_module = dsp_find_module(target_bus->dsp_module_head, module_id);
    
  printf("module_id %s, editing karlsen_lowpass of amount %f, freq %f of 1octave, res %f..\n",module_id,amt,freq,res);

  dsp_edit_karlsen_lowpass(target_module, amt, freq, res);

  lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out);
  lo_send(lo_addr_send,"/cyperus/add/module/karlsen_lowpass","sfff", module_id, amt, freq, res);
  free(lo_addr_send);
  
  return 0;
} /* osc_edit_karlsen_lowpass_handler */


/* ================= FUNCTIONS BELOW NEED TO BE CONVERTED TO USE dsp_* OBJECTS ==================== */

int osc_add_pinknoise_handler(const char *path, const char *types, lo_arg ** argv,
		     int argc, void *data, void *user_data)
{
  
  printf("path: <%s>\n", path);

  printf("creating pink noise..\n");
  
  /* add pinknoise() function from libcyperus onto correct voice/signal chain */
  dsp_create_pinknoise();
  
  return 0;
} /* osc_add_pinknoise_handler */

