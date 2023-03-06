/* dsp.c
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
hit under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
xoYou should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2015 murray foster */

#include "dsp.h"

#define MAX_PATH_ID_LENGTH 16384

int dsp_global_new_operation_graph = 0;

struct dsp_bus_port
*dsp_find_bus_port(struct dsp_bus_port *target_bus_port, char *id) {
  struct dsp_bus_port *temp_bus_port;
  int match_found = 0;
  temp_bus_port = target_bus_port;
  if(id != NULL ) {
    if( strcmp(id, "") != 0 ) {
      while( temp_bus_port != NULL ) {
	if( strcmp(temp_bus_port->id, id) == 0) {
	  target_bus_port = temp_bus_port;
	  match_found = 1;
	  break;
	}
	temp_bus_port = temp_bus_port->next;
      }
      if( !match_found )
	return NULL;
    }
  }
  return target_bus_port;
} /* dsp_find_bus_port */

struct dsp_module
*dsp_find_module(struct dsp_module *head_module, char *id) {
  struct dsp_module *target_module;
  struct dsp_module *temp_module;
  int match_found = 0;

  temp_module = head_module;

  if(id != NULL ) {
    if( strcmp(id, "") != 0 ) {
      while( temp_module != NULL ) {
	if( strcmp(temp_module->id, id) == 0) {
	  target_module = temp_module;
	  match_found = 1;
	  break;
	}
	temp_module = temp_module->next;
      }
      if( !match_found )
	return NULL;
    }
  }
  return target_module;
} /* dsp_find_module */

struct dsp_port_out
*dsp_find_port_out(struct dsp_port_out *port_out_head, char *id) {
  struct dsp_port_out *target_port_out;
  struct dsp_port_out *temp_port_out;
  int match_found = 0;
  temp_port_out = port_out_head;
  if(id != NULL ) {
    if( strcmp(id, "") != 0 ) {
      while( temp_port_out != NULL ) {
	if( strcmp(temp_port_out->id, id) == 0) {
	  target_port_out = temp_port_out;
	  match_found = 1;
	  break;
	}
	temp_port_out = temp_port_out->next;
      }
      if( !match_found )
	return NULL;
    }
  }
  return target_port_out;
} /* dsp_find_port_out */


struct dsp_port_in
*dsp_find_port_in(struct dsp_port_in *port_in_head, char *id) {
  struct dsp_port_in *target_port_in;
  struct dsp_port_in *temp_port_in;
  int match_found = 0;
  temp_port_in = port_in_head;
  if(id != NULL ) {
    if( strcmp(id, "") != 0 ) {
      while( temp_port_in != NULL ) {
	if( strcmp(temp_port_in->id, id) == 0) {
	  target_port_in = temp_port_in;
	  match_found = 1;
	  break;
	}
	temp_port_in = temp_port_in->next;
      }
      if( !match_found )
	return NULL;
    }
  }
  return target_port_in;
} /* dsp_find_port_in */

void
dsp_parse_path(char* result[], const char *path) {
  char temp_string[MAX_PATH_ID_LENGTH];
  char *parsed_id, *parsed_path;
  int i, id_length, match_found;
  id_length = 0;
  match_found = 0;
  temp_string[strlen(path)] = '\0';
  for(i=strlen(path) - 1; i  >= 0; i--) {
    temp_string[i] = path[i];
    switch(path[i]) {
    case '/' :
      /* bus */
      result[0] = "/";
      match_found = 1;
      break;
    case ':':
      /* bus port */
      result[0] = ":";
      match_found = 1;
      break;
    case '?':
      /* module */
      result[0] = "?";
      match_found = 1;
      break;
    case '<':
      /* input port */
      result[0] = "<";
      match_found = 1;
      break;
    case '>':
      /* output port */
      result[0] = ">";
      match_found = 1;
      break;
    case '{':
      /* MAINS input port */
      result[0] = "{";
      match_found = 1;
      break;
    case '}':
      /* MAINS output port */
      result[0] = "}";
      match_found = 1;
      break;
    default:
      break;
    }
    if( match_found )
      break;
    id_length++;
  }
  parsed_id = (char *)malloc(sizeof(char) * id_length + 1 + 1);
  parsed_path = (char *)malloc(sizeof(char) * strlen(path) - id_length + 1 + 1);
  for(i=0; i < strlen(path) - id_length - 1; i++)
    parsed_path[i] = path[i];
  parsed_path[i++] = '\0';
  for(i=0; i < id_length; i++)
    parsed_id[i] = temp_string[strlen(path) - id_length + i];
  parsed_id[i++] = '\0';

  if( strcmp(result[0], "/") )
    result[1] = parsed_path;
  else
    result[1] = (char *)path;
  result[2] = parsed_id;
  return;
} /* dsp_parse_path */

struct dsp_bus*
dsp_parse_bus_path(char *target_path) {
  printf("dsp.c::dsp_parse_bus_path()\n");
  printf("target_path: %s\n", target_path);
  struct dsp_bus *temp_bus, *target_bus, *temp_bus_head, *deep_bus;
  int bus_count = 0;
  int path_count = 0;
  char *temp_count_path = NULL;
  char *temp_copy_path = NULL;
  char **paths;
  char *token = NULL;
  int idx;
  
  temp_count_path = malloc(sizeof(char)*(strlen(target_path)+1));
  temp_copy_path = malloc(sizeof(char)*(strlen(target_path)+1));

  strncpy(temp_count_path, target_path, strlen(target_path));
  temp_count_path[strlen(target_path)] = '\0';
  strncpy(temp_copy_path, target_path, strlen(target_path));
  temp_copy_path[strlen(target_path)] = '\0';
  
  while ((token = strsep(&temp_count_path, "/"))) {
    if(strcmp(token, "") != 0) {
      path_count++;
    }
  }
  free(temp_count_path);
  
  idx = 0;
  paths = malloc(sizeof(char*) * path_count);
  while ((token = strsep(&temp_copy_path, "/"))) {
    if(strcmp(token, "") != 0) {
      paths[idx] = malloc(sizeof(char)*strlen(token)+1);
      strncpy(paths[idx], token, strlen(token));
      paths[idx][36] = '\0';
      idx++;
    }
  }
  free(temp_copy_path);
  
  bus_count = 0;
  temp_bus = dsp_global_bus_head;  
  for(idx=0; idx<path_count; idx++) {
    while(temp_bus != NULL) {
      if(strcmp(paths[idx], temp_bus->id) == 0) {
        bus_count++;
        break;
      }
      temp_bus = temp_bus->next;
    }

    if( bus_count < path_count ) {
      if(temp_bus->down) {
        printf("dsp.c::dsp_parse_bus_path()::DEBUG - descending to bus\n");
        temp_bus = temp_bus->down;
      }
    }
  }
  
  if(bus_count == path_count) {
    target_bus = temp_bus;    
    printf("dsp.c::dsp_parse_bus_path()::DEBUG - found bus '%s'\n", target_bus->id);
  }
  
  for(idx=0; idx<path_count; idx++) {
    free(paths[idx]);
  }
  free(paths);
  
  return target_bus;
} /* dsp_parse_bus_path */

struct dsp_bus_port*
dsp_build_bus_ports(struct dsp_bus_port *head_bus_port,
		    char *bus_ports, int out) {

  char *target_bus_ports = malloc(sizeof(char) * strlen(bus_ports) + 1);
  char *output_token;
  int i;
  char *port_id;
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_bus_port *target_bus_port = NULL;
  struct dsp_port_in *port_in = NULL;
  struct dsp_port_out *port_out = NULL;
  int match_found = 0;
  char *p;

  int multi_port = 0;

  temp_bus_port = head_bus_port;
  strcpy(target_bus_ports, bus_ports);

  for(i=0; i < strlen(target_bus_ports); i++)
    if(target_bus_ports[i] == ',') {
      multi_port = 1;
      break;
    }

  if(!multi_port) {
    temp_bus_port = dsp_bus_port_init(target_bus_ports, out);
    port_in = dsp_port_in_init("in", 512, NULL);
    port_out = dsp_port_out_init("out", 1);
    temp_bus_port->in = port_in;
    temp_bus_port->out = port_out;

    if(head_bus_port != NULL) {
      dsp_bus_port_insert_tail(head_bus_port, temp_bus_port);
    }
    else {
      head_bus_port = temp_bus_port;
    }
  } else {
    p = target_bus_ports;
    while (*p)                      /* while not end of string */
      {
	char *sp = p;                  /* set a start pointer */
	while (*p && *p != ',') p++;   /* advance to space    */
	output_token = malloc (p - sp + 1); /* allocate */
	strncpy (output_token, sp, p - sp);         /* copy   */
	output_token[p - sp] = 0;   /* force null-termination */
	if( output_token != NULL ) {
	  if( strcmp(output_token, "") != 0 ) {
	    temp_bus_port = dsp_bus_port_init(output_token, out);
	    port_in = dsp_port_in_init("in", 512, NULL);
	    port_out = dsp_port_out_init("out", 1);
	    temp_bus_port->in = port_in;
	    temp_bus_port->out = port_out;
	    if(head_bus_port != NULL)
	      dsp_bus_port_insert_tail(head_bus_port, temp_bus_port);
	    else
	      head_bus_port = temp_bus_port;
	    free(output_token);
	  }
	}
	while (*p && *p == ',') p++;   /* find next non-space */
      }
  }
  return head_bus_port;
} /* dsp_build_bus_ports */

void
dsp_add_bus(char *target_bus_path, struct dsp_bus *new_bus, char *ins, char *outs) {
  struct dsp_bus *temp_bus, *target_bus;
  struct dsp_bus_port *temp_bus_port;

  /* create bus */
  if(ins != NULL) {
    temp_bus_port = new_bus->ins;
    temp_bus_port = dsp_build_bus_ports(temp_bus_port, ins, 0);
    new_bus->ins = temp_bus_port;
  }

  if(outs != NULL) {
    temp_bus_port = new_bus->outs;
    temp_bus_port = dsp_build_bus_ports(temp_bus_port, outs, 1); /* output port */
    new_bus->outs = temp_bus_port;
  }

  /* insert head bus, if that's what we're doing */
  if( !strcmp(target_bus_path, "/") || !strcmp(target_bus_path, "")) {
    if( dsp_global_bus_head != NULL )
      dsp_bus_insert_tail(dsp_global_bus_head, new_bus);
    else
      dsp_global_bus_head = new_bus;
  } else {
    /* if it's not a head bus, parse down path and add */
    target_bus = dsp_parse_bus_path(target_bus_path);
    if(target_bus != NULL) {
      temp_bus = target_bus->down;
      if (temp_bus != NULL) {
        dsp_bus_insert_tail(temp_bus, new_bus);
      } else {
        target_bus->down = new_bus;
      }
    }
    else {
      target_bus = new_bus;
    }
  }

  /* graph changed, generate new graph id */
  dsp_graph_id_rebuild();
  
  return;
} /* dsp_add_bus */

struct dsp_module*
dsp_add_module(struct dsp_bus *target_bus,
	       char *name,
	       void (*dsp_function) (struct dsp_operation*, int, int),
               void (*dsp_osc_listener_function) (struct dsp_operation*, int, int),
	       struct dsp_operation *(*dsp_optimize) (char*, struct dsp_module*),
	       dsp_parameter dsp_param,
	       struct dsp_port_in *ins,
	       struct dsp_port_out *outs) {
  struct dsp_module *new_module  = dsp_module_init(name,
						   dsp_function,
                                                   dsp_osc_listener_function,
						   dsp_optimize,
						   dsp_param,
						   ins,
						   outs);
  if( target_bus->dsp_module_head == NULL )
    target_bus->dsp_module_head = new_module;
  else
    dsp_module_insert_tail(target_bus->dsp_module_head, new_module);

  /* graph changed, generate new graph id */
  dsp_graph_id_rebuild();
  
  return new_module;
} /* dsp_add_module */

void
dsp_remove_module(struct dsp_module *module, int remove) {
  module->remove = remove;

  /* graph changed, generate new graph id */
  dsp_graph_id_rebuild();
  
  return;
} /* dsp_remove_module */

void
dsp_bypass_module(struct dsp_module *module, int bypass) {
  module->bypass = bypass;

  /* graph changed, generate new graph id */
  dsp_graph_id_rebuild();
  
  return;
} /* dsp_bypass */

int
dsp_add_connection(char *id_out, char *id_in) {
  struct dsp_connection *new_connection;
  struct dsp_port_out *port_out = NULL;
  struct dsp_port_in *port_in = NULL;
  struct dsp_bus_port *bus_port = NULL;
  char *port_id;
  char *temp_result[3];
  char *module_path, *port_out_id, *port_in_id, *bus_path,
    *module_id, *bus_port_path, *bus_port_id, *bus_id;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  struct dsp_bus_port *target_bus_port;

  /* parsing id_out (connection output) */
  dsp_parse_path(temp_result, id_out);
  if( strcmp(temp_result[0], "{") == 0) {
    port_out = dsp_find_port_out(dsp_main_ins, temp_result[2]);
  }

  if( strcmp(temp_result[0], ">") == 0 ) {
    module_path = temp_result[1];
    port_out_id = temp_result[2];

    dsp_parse_path(temp_result, module_path);
    bus_path = temp_result[1];
    module_id = temp_result[2];

    target_bus = dsp_parse_bus_path(bus_path);

    target_module = dsp_find_module(target_bus->dsp_module_head,
				    module_id);
    port_out = dsp_find_port_out(target_module->outs, port_out_id);
  }

  if( strcmp(temp_result[0], ":") == 0) {
    bus_port_path = temp_result[1];
    bus_port_id = temp_result[2];

    dsp_parse_path(temp_result, bus_port_path);
    bus_path = temp_result[1];

    target_bus = dsp_parse_bus_path(bus_path);
    target_bus_port = dsp_find_bus_port(target_bus->outs,
					bus_port_id);
    if( target_bus_port == NULL ) {
      target_bus_port = dsp_find_bus_port(target_bus->ins,
					  bus_port_id);
      if( target_bus_port )
	port_out = target_bus_port->out;
    } else {
      port_out = target_bus_port->out;
    }
  }

  /* parsing id_in (to connection input) */
  dsp_parse_path(temp_result, id_in);
  if( strcmp(temp_result[0], "}") == 0) {
    port_in = dsp_find_port_in(dsp_main_outs, temp_result[2]);
  }

  if( strcmp(temp_result[0], "<") == 0 ) {
    module_path = temp_result[1];
    port_in_id = temp_result[2];
    dsp_parse_path(temp_result, module_path);
    bus_path = temp_result[1];
    module_id = temp_result[2];
    target_bus = dsp_parse_bus_path(bus_path);
    target_module = dsp_find_module(target_bus->dsp_module_head,
				    module_id);
    port_in = dsp_find_port_in(target_module->ins, port_in_id);
  }

  if( strcmp(temp_result[0], ":") == 0) {

    bus_port_path = temp_result[1];
    bus_port_id = temp_result[2];

    dsp_parse_path(temp_result, bus_port_path);
    bus_path = temp_result[1];

    target_bus = dsp_parse_bus_path(bus_path);
    target_bus_port = dsp_find_bus_port(target_bus->ins,
					bus_port_id);
    if( target_bus_port == NULL ) {
      target_bus_port = dsp_find_bus_port(target_bus->outs,
					  bus_port_id);
      if( target_bus_port )
	port_in = target_bus_port->in;
    } else {
      port_in = target_bus_port->in;
    }
  }

  if( (port_out == NULL) ||
      (port_in == NULL) ) {
    printf("failed to add connection!\n");
    return 1;
  }

  /* instantiate and add to global connection graph */
  new_connection = dsp_connection_init(id_out,
				       id_in,
				       port_out,
				       port_in);
  if(dsp_global_connection_graph == NULL)
    dsp_global_connection_graph = new_connection;
  else
    dsp_connection_insert_tail(dsp_global_connection_graph,
			       new_connection);

  /* TODO: check that the current processing graph isn't the same as this new one,
  */
  
  dsp_build_optimized_graph(NULL);

  /* graph changed, generate new graph id */
  dsp_graph_id_rebuild();
  
  return 0;
} /* dsp_add_connection */


int
dsp_remove_connection(char *id_out, char *id_in) {
  struct dsp_connection *temp_connection;
  struct dsp_connection *found_connection;
  struct dsp_port_out *port_out = NULL;
  struct dsp_port_in *port_in = NULL;
  struct dsp_bus_port *bus_port = NULL;
  char *port_id;
  char *temp_result[3];
  char *module_path, *port_out_id, *port_in_id, *bus_path,
    *module_id, *bus_port_path, *bus_port_id, *bus_id;
  struct dsp_bus *target_bus;
  struct dsp_module *target_module;
  struct dsp_bus_port *target_bus_port;

  /* parsing id_out (connection output) */
  dsp_parse_path(temp_result, id_out);
  if( strcmp(temp_result[0], "{") == 0) {
    port_out = dsp_find_port_out(dsp_main_ins, temp_result[2]);
  }

  if( strcmp(temp_result[0], ">") == 0 ) {
    module_path = temp_result[1];
    port_out_id = temp_result[2];

    dsp_parse_path(temp_result, module_path);
    bus_path = temp_result[1];
    module_id = temp_result[2];

    target_bus = dsp_parse_bus_path(bus_path);

    target_module = dsp_find_module(target_bus->dsp_module_head,
				    module_id);
    port_out = dsp_find_port_out(target_module->outs, port_out_id);
  }
  if( strcmp(temp_result[0], ":") == 0) {
    bus_port_path = temp_result[1];
    bus_port_id = temp_result[2];

    dsp_parse_path(temp_result, bus_port_path);
    bus_path = temp_result[1];

    target_bus = dsp_parse_bus_path(bus_path);
    target_bus_port = dsp_find_bus_port(target_bus->outs,
					bus_port_id);
    if( target_bus_port == NULL ) {
      target_bus_port = dsp_find_bus_port(target_bus->ins,
					  bus_port_id);
      if( target_bus_port )
	port_out = target_bus_port->out;
    } else {
      port_out = target_bus_port->out;
    }
  }

  /* parsing id_in (to connection input) */
  dsp_parse_path(temp_result, id_in);
  if( strcmp(temp_result[0], "}") == 0) {
    port_in = dsp_find_port_in(dsp_main_outs, temp_result[2]);
  }

  if( strcmp(temp_result[0], "<") == 0 ) {
    module_path = temp_result[1];
    port_in_id = temp_result[2];
    dsp_parse_path(temp_result, module_path);
    bus_path = temp_result[1];
    module_id = temp_result[2];
    target_bus = dsp_parse_bus_path(bus_path);
    target_module = dsp_find_module(target_bus->dsp_module_head,
				    module_id);
    port_in = dsp_find_port_in(target_module->ins, port_in_id);
  }

  if( strcmp(temp_result[0], ":") == 0) {

    bus_port_path = temp_result[1];
    bus_port_id = temp_result[2];

    dsp_parse_path(temp_result, bus_port_path);
    bus_path = temp_result[1];

    target_bus = dsp_parse_bus_path(bus_path);
    target_bus_port = dsp_find_bus_port(target_bus->ins,
					bus_port_id);
    if( target_bus_port == NULL ) {
      target_bus_port = dsp_find_bus_port(target_bus->outs,
					  bus_port_id);
      if( target_bus_port )
	port_in = target_bus_port->in;
    } else {
      port_in = target_bus_port->in;
    }
  }

  if( (port_out == NULL) ||
      (port_in == NULL) ) {
    printf("failed to add connection!\n");
    return 1;
  }

  if(dsp_global_connection_graph) {
    temp_connection = dsp_global_connection_graph;
    while(temp_connection != NULL) {
      if( (!strcmp(temp_connection->id_out, id_out)) &&
	  (!strcmp(temp_connection->id_in, id_in)) ) {
	printf("found target connection\n");
	found_connection = temp_connection;
	temp_connection = temp_connection->prev;
 
	/* check if previous connection is root/NULL */
	if( temp_connection ) {
	  temp_connection->next = found_connection->next;
	} else {
	  dsp_global_connection_graph = found_connection->next;
	}
	  
	dsp_connection_terminate(found_connection);
	printf("free()'d target connection\n");
	
	dsp_build_optimized_graph(NULL);
	return 0;
      }
      temp_connection = temp_connection->next;
    }
  }

  /* TODO: check that the current processing graph isn't the same as this new one,
           replace the actual processing graph if it's not (on the last sample cycle),
  */

  /* graph changed, generate new graph id */
  dsp_graph_id_rebuild();

  /* error return */
  return 1;
} /* dsp_remove_connection */

void
dsp_optimize_connections_input(char *current_path, struct dsp_connection *connection) {
  /* is the below ever actually the case? */

  /* do we need to account for whether dsp_global_translation_connection_raph_processing is populated
     versus dsp_global_operation_head_processing is not populated? do we care? */

  struct dsp_operation *temp_op = NULL;
  struct dsp_operation *temp_op_out = NULL;
  struct dsp_operation *temp_op_in = NULL;

  struct dsp_operation *matched_op_out = NULL;
  struct dsp_operation *matched_op_in = NULL;

  struct dsp_operation *temp_trranslation_op = NULL;

  struct dsp_operation_sample *temp_sample_out = NULL;
  struct dsp_operation_sample *temp_sample_in = NULL;
  struct dsp_operation_sample *found_sample_out = NULL;
  struct dsp_operation_sample *sample_in = NULL;
  struct dsp_operation_sample *sample_out = NULL;
  
  struct dsp_operation_sample *new_summand = NULL;

  struct dsp_translation_connection *temp_translation_conn = NULL;

  char *temp_result[3];
  char *temp_result_module[3];
  char *temp_op_out_path, *temp_op_in_path = NULL;

  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;

  int is_bus_port_out = 0;
  int is_module_out = 0;

  int is_bus_port_in = 0;
  int is_module_in = 0;
  int is_main_out_in = 0;

  int is_main_in_out = 0;
  
  dsp_parse_path(temp_result, (char *)connection->id_out);
  if( strstr(":", temp_result[0]) ) {
    temp_op_out_path = current_path;
    is_bus_port_out = 1;
  }

  else if( strstr("<", temp_result[0]) )
    temp_op_out_path = temp_result[1];
  else if( strstr(">", temp_result[0]) )
    temp_op_out_path = temp_result[1];
  else
    temp_op_out_path = current_path;

    
  /* OUTPUT PROCESSING */    
    
    /* grab 'out' op and sample address */
    temp_op_out = dsp_global_operation_head_processing;

    while(temp_op_out != NULL) {
      if( strcmp(temp_op_out->dsp_id, temp_op_out_path) == 0 ) {
	matched_op_out = temp_op_out;
	temp_sample_out = temp_op_out->outs;
	if( is_bus_port_out == 0 ) {
	  while(temp_sample_out != NULL) {
	    if( strcmp(temp_sample_out->dsp_id, temp_result[2]) == 0 ) {
	      found_sample_out = temp_sample_out;
	      break;
	    }
	    temp_sample_out = temp_sample_out->next;
	  }
	} else {
	  found_sample_out = temp_sample_out;
	}
	break;
      }
      temp_op_out = temp_op_out->next;
    }

  if( matched_op_out ) {
    if( found_sample_out == NULL ) {
      if( is_bus_port_out ) {
	found_sample_out = dsp_operation_sample_init("<bus port port out>", 0.0, 1);
      } else {
	found_sample_out = dsp_operation_sample_init(temp_result[2], 0.0, 1);
      }

      if( matched_op_out->outs == NULL ) {
	matched_op_out->outs = found_sample_out;
      } else {
	dsp_operation_sample_insert_tail(matched_op_out->outs, found_sample_out);
      }
    }
  }

  
  dsp_parse_path(temp_result, connection->id_out);
  if( strstr(":", temp_result[0]) ) {
    temp_op_in_path = (char *)connection->id_out;
    is_bus_port_in = 1;
  } else if( strstr("<", temp_result[0]) ) {
    printf("found connection input connected to connection input! BAD. and bailing\n");
    exit(1);
  } else if( strstr("}", temp_result[0]) ) {
    temp_op_in_path = (char *)connection->id_out;
    is_main_in_out = 1;
  } else if( strstr(">", temp_result[0]) ) {
    temp_op_out_path = temp_result[1];
    dsp_parse_path(temp_result_module, temp_result[1]);
    if( strcmp(temp_result_module[0], "?") == 0 ) {
      is_module_out = 1;
    }
  } else {
    printf("unexpected connection output -- id: '%s', exiting..\n", connection->id_out);
    exit(1);
  }
  
  if( found_sample_out == NULL ) {
        /* grab 'in' op and sample address */
    if( is_main_in_out )
      temp_op_out = dsp_optimized_main_ins;
    else
      temp_op_out = dsp_global_operation_head_processing;

    while(temp_op_out != NULL) {
      if( strcmp(temp_op_out->dsp_id, temp_op_out_path) == 0 ) {
	temp_sample_out = temp_op_out->outs;
	if( is_bus_port_out == 0 && is_module_out) {
	  while(temp_sample_out != NULL) {
	    if( strcmp(temp_sample_out->dsp_id, temp_result[2]) == 0 ) {
	      sample_out = temp_sample_out;
	      break;
	    }
	    temp_sample_out = temp_sample_out->next;
	  }
	} else {
	  sample_out = temp_sample_out;
        }
	break;
      }
      temp_op_out = temp_op_out->next;
    }

    if( sample_out == NULL ) {
      if( is_bus_port_out ) {
	sample_out = dsp_operation_sample_init("<bus port port in>", 0.0, 1);
      }
      else if( is_module_out ) {
	/* sample_out = dsp_operation_sample_init(temp_result[2], 0.0, 1); */
      } else if( is_main_out_in ) {

      } else {
	printf("found unknown dsp object type!! (?) exiting..\n");
	exit(1);
      }
    
      if( temp_op_out == NULL ) {
	if( is_module_out ) {
	  temp_bus = dsp_parse_bus_path(temp_result_module[1]);
	  temp_module = dsp_find_module(temp_bus->dsp_module_head, temp_result_module[2]);
	  temp_op = temp_module->dsp_optimize(temp_result_module[1], temp_module);

          temp_sample_out = temp_op->outs;
          while(temp_sample_out != NULL) {
            if( strcmp(temp_sample_out->dsp_id, temp_result[2]) == 0 ) {
              sample_out = temp_sample_out;
              break;
            }
            temp_sample_out = temp_sample_out->next;
          }
          if( sample_out == NULL ) {
            printf("CALLING dsp_optimize() ON '%s' FAILED! exiting..\n", temp_result[1]);
            exit(1);
          }
	} else {
	  temp_op = dsp_operation_init(connection->id_out);
	  }
	if(dsp_global_operation_head_processing == NULL)
	  dsp_global_operation_head_processing = temp_op;
	else {
	  dsp_operation_insert_tail(dsp_global_operation_head_processing,
				    temp_op);
        }
      } else {
	temp_op = temp_op_out;
      }

      if(temp_op->outs == NULL)
	temp_op->outs = sample_out;
      else
	dsp_operation_sample_insert_tail(temp_op->outs, sample_out);

    }

    matched_op_out = temp_op;
    temp_op = NULL;
    temp_op_out = NULL;
    
  } else {
    sample_out = found_sample_out;
  }

  
  /* INPUT PROCESSING */

  
  dsp_parse_path(temp_result, (char *)connection->id_in);
  if( strstr(":", temp_result[0]) ) {
    temp_op_in_path = (char *)connection->id_in;
    is_bus_port_in = 1;
  } else if( strstr("<", temp_result[0]) ) {
    temp_op_in_path = temp_result[1];
    dsp_parse_path(temp_result_module, temp_result[1]);
    if( strcmp(temp_result_module[0], "?") == 0 ) {
      is_module_in = 1;
    }
  } else if( strstr("}", temp_result[0]) ) {
    temp_op_in_path = (char *)connection->id_in;
    is_main_out_in = 1;
  } else if( strstr(">", temp_result[0]) ) {
    printf("found connection output connected to connection output! BAD. and bailing\n");
    exit(1);
  } else {
    printf("unexpected connection input -- id: '%s', exiting..\n", connection->id_in);
    exit(1);
  }

    /* grab 'in' op and sample address */
    if( is_main_out_in )
      temp_op_in = dsp_optimized_main_outs;
    else
      temp_op_in = dsp_global_operation_head_processing;

    while(temp_op_in != NULL) {
      if( strcmp(temp_op_in->dsp_id, temp_op_in_path) == 0 ) {
	temp_sample_in = temp_op_in->ins;
	if( is_bus_port_in == 0 && is_module_in) {
	  while(temp_sample_in != NULL) {
	    if( strcmp(temp_sample_in->dsp_id, temp_result[2]) == 0 ) {
	      sample_in = temp_sample_in;
	      break;
	    }
	    temp_sample_in = temp_sample_in->next;
	  }
	} else {
	  sample_in = temp_sample_in;
        }
	break;
      }
      temp_op_in = temp_op_in->next;
    }

    int created_op = 0;
    if( sample_in == NULL ) {
      if( is_bus_port_in ) {
	sample_in = dsp_operation_sample_init("<bus port port in>", 0.0, 1);
      }
      else if( is_module_in ) {
	/* sample_in = dsp_operation_sample_init(temp_result[2], 0.0, 1); */
      } else if( is_main_out_in ) {

      } else {
	printf("found unknown dsp object type!! (?) exiting..\n");
	exit(1);
      }
      
      if( temp_op_in == NULL ) {
	if( is_module_in ) {
	  temp_bus = dsp_parse_bus_path(temp_result_module[1]);
	  temp_module = dsp_find_module(temp_bus->dsp_module_head, temp_result_module[2]);
	  temp_op = temp_module->dsp_optimize(temp_result_module[1], temp_module);

          temp_sample_in = temp_op->ins;
          while(temp_sample_in != NULL) {
            if( strcmp(temp_sample_in->dsp_id, temp_result[2]) == 0 ) {
              sample_in = temp_sample_in;
              break;
            }
            temp_sample_in = temp_sample_in->next;
          }
          if( sample_in == NULL ) {
            printf("CALLING dsp_optimize() ON '%s' FAILED! exiting..\n", temp_result[1]);
            exit(1);
          }
	} else {
	  temp_op = dsp_operation_init(connection->id_in);
        }

	if(dsp_global_operation_head_processing == NULL)
	  dsp_global_operation_head_processing = temp_op;
	else {
	  if( is_module_in ) {
	    dsp_operation_insert_ahead(matched_op_out, temp_op);
	  } else {
	    dsp_operation_insert_tail(dsp_global_operation_head_processing,
				      temp_op);
	  }
	}
      } else {
	temp_op = temp_op_in;
      }

      if(temp_op->ins == NULL)
	temp_op->ins = sample_in;
      else
	dsp_operation_sample_insert_tail(temp_op->ins, sample_in);
    }
    
    new_summand = dsp_operation_sample_init((char *)sample_out->dsp_id, 0.0, 0);
    new_summand->sample = sample_out->sample;
    if(sample_in->summands == NULL)
      sample_in->summands = new_summand;
    else
      dsp_operation_sample_insert_tail(sample_in->summands, new_summand);
    
} /* dsp_optimize_connections_input */

void
dsp_optimize_connections_bus(char *current_bus_path, struct dsp_bus_port *ports) {
  struct dsp_bus_port *temp_port = ports;
  struct dsp_connection *temp_connection;
  char *current_path = NULL;

  int temp_port_idx = 0;

  while(temp_port != NULL) {
    /* handle bus input */
    //temp_sample_in = 0.0;

    /* while(rtqueue_isempty(temp_port->in->values) == 0) {
      temp_sample_in += rtqueue_deq(temp_port->in->values);
    }
    */

    // temp_port->out->value = temp_sample_in;
    /* enqueue out samples on in fifo's based on connection graph */
    temp_connection = dsp_global_connection_graph;

    current_path = (char *)malloc(strlen(current_bus_path) + strlen(temp_port->id) + 1);
    if(current_path != NULL) {
      current_path[0] = '\0';
      strcpy(current_path, current_bus_path);
      current_path[strlen(current_bus_path)] = '\0'; /* expecting a '/' on the end, remove it */
      strcat(current_path, ":");
      strcat(current_path, temp_port->id);
    }

    while(temp_connection != NULL) {
      /* compare each connection 'out' with this one, enqueue each fifo with data
	 that matches the 'out' port path */
      
      if(strcmp(current_path, temp_connection->id_out) == 0) {
	/* commented out data motion logic */
	/* rtqueue_enq(temp_connection->in_values, temp_sample_in); */
        
	dsp_optimize_connections_input(current_path,
				       temp_connection);
      }
      temp_connection = temp_connection->next;
    }
    if(current_path != NULL)
      free(current_path);
    temp_port = temp_port->next;
    temp_port_idx++;

  }
} /* dsp_optimize_connections_bus */

void
dsp_optimize_graph(struct dsp_bus *head_bus, char *parent_path) {
  struct dsp_module *temp_module;
  struct dsp_bus *temp_bus = head_bus;
  char *current_path;
  int parent_path_size = 0;

  while(temp_bus != NULL) {
    
    /* build current path */

    /* handle root path "/" (avoid adding extra path separator if it's root) */
    if( strcmp(parent_path, "/") == 0 )
      parent_path_size = 1;
    else
      parent_path_size = strlen(parent_path) + 1;

    current_path = (char *)malloc(sizeof(char) * (parent_path_size + strlen(temp_bus->id) + 1));
    strcpy(current_path, parent_path);
    if( parent_path_size > 1 )
      strcat(current_path, "/");
    strcat(current_path, temp_bus->id);

    /* process bus inputs */
    dsp_optimize_connections_bus(current_path, temp_bus->ins);

    /* handle dsp modules */
    temp_module = temp_bus->dsp_module_head;
    while(temp_module != NULL) {
      dsp_optimize_connections_module(current_path, (char *)temp_module->id, temp_module->outs);
      temp_module = temp_module->next;
    }
    
    /* process bus outputs */
    dsp_optimize_connections_bus(current_path, temp_bus->outs);

    dsp_optimize_graph(temp_bus->down, current_path);

    temp_bus = temp_bus->next;

    if(current_path)
      free(current_path);
  }
  return;
} /* dsp_optimize_graph */

void
dsp_build_mains(int channels_in, int channels_out) {
  char *formal_main_name = NULL;

  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_port_in *temp_port_in = NULL;

  struct dsp_operation *temp_op = NULL;
  struct dsp_operation_sample *temp_sample = NULL;

  int i;

  int fifo_size = 1024;

  for(i=0; i<channels_in; i++) {
    if( i == 0 ) {
      temp_port_out = dsp_port_out_init("main_in", 1);
      dsp_main_ins = temp_port_out;

      formal_main_name = (char *)malloc(44);
      formal_main_name[0] = '\0';
      strcpy(formal_main_name, "/mains{");
      strcat(formal_main_name, temp_port_out->id);
    } else {
      temp_port_out->next = dsp_port_out_init("main_in", 1);
      temp_port_out = temp_port_out->next;

      formal_main_name = (char *)malloc(44);
      if(formal_main_name != NULL) {
	formal_main_name[0] = '\0';
	strcpy(formal_main_name, "/mains{");
	strcat(formal_main_name, temp_port_out->id);
      }
    }
	
    temp_op = dsp_operation_init(formal_main_name);
    temp_sample = dsp_operation_sample_init("<main port in>", (float)0.0, 1);

    if(temp_op->outs == NULL)
      temp_op->outs = temp_sample;
    else
      dsp_operation_sample_insert_tail(temp_op->outs, temp_sample);
    if(dsp_optimized_main_ins == NULL)
      dsp_optimized_main_ins = temp_op;
    else
      dsp_operation_insert_tail(dsp_optimized_main_ins, temp_op);

    free(formal_main_name);
  }

  for(i=0; i<channels_out; i++) {
    if( i == 0 ) {
      temp_port_in = dsp_port_in_init("main_out", fifo_size, NULL);
      dsp_main_outs = temp_port_in;

      formal_main_name = (char *)malloc(44);
      if(formal_main_name != NULL) {
	formal_main_name[0] = '\0';
	strcpy(formal_main_name, "/mains}");
	strcat(formal_main_name, temp_port_in->id);
      }
    } else {
      temp_port_in->next = dsp_port_in_init("main_out", fifo_size, NULL);
      temp_port_in = temp_port_in->next;

      formal_main_name = (char *)malloc(44);
      if(formal_main_name != NULL) {
	formal_main_name[0] = '\0';
	strcpy(formal_main_name, "/mains}");
	strcat(formal_main_name, temp_port_in->id);
      }
    }
    temp_op = dsp_operation_init(formal_main_name);
    temp_sample = dsp_operation_sample_init("<main port out>", (float)0.0, 1);

    if( temp_op->ins == NULL )
      temp_op->ins = temp_sample;
    else
      dsp_operation_sample_insert_tail(temp_op->ins, temp_sample);

    if( dsp_optimized_main_outs == NULL )
      dsp_optimized_main_outs = temp_op;
    else 
      dsp_operation_insert_tail(dsp_optimized_main_outs, temp_op);
    
    free(formal_main_name);
  }
} /* dsp_build_mains */

void
*dsp_build_optimized_graph(void *arg) {
  int i;
  float outsample = 0.0;
  char current_path[2] = "/";

  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;

  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_port_in *temp_port_in = NULL;

  dsp_global_operation_head_processing = NULL;

  dsp_optimize_connections_main_inputs(dsp_main_ins);
  
  temp_bus = dsp_global_bus_head;
  while( temp_bus != NULL ) {
    dsp_optimize_graph(temp_bus, current_path);
    temp_bus = temp_bus->next;
  }

  printf("assigning flag\n");
  dsp_global_new_operation_graph = 1;
  printf("assigned flag\n");
} /* dsp_build_optimized_graph */

void
dsp_process(struct dsp_operation *head_op, int jack_sr, int pos) {
  struct dsp_operation *temp_op = NULL;
  temp_op = head_op;

  while(temp_op != NULL) {
    if( temp_op->module == NULL ) {
      if( temp_op->ins == NULL ) {
        temp_op->outs->sample->value = 0.0;
      } else {
	if( temp_op->outs != NULL )
	  temp_op->outs->sample->value = dsp_sum_summands(temp_op->ins->summands);
      }
    } else {
      temp_op->module->dsp_function(temp_op, jack_sr, pos);
    }
    temp_op = temp_op->next;
  }
  
  return;
} /* dsp_process */

void*
dsp_thread(void *arg) {
  int pos, i;

  char current_path[2] = "/";
  struct dsp_bus *temp_bus;

  struct dsp_operation *temp_op;
  
  struct dsp_operation *temp_main_in = NULL;
  struct dsp_operation *temp_main_out = NULL;

  /* dsp_mains_allocate(jackcli_channels_in, jackcli_channels_out, jackcli_fifo_size); */

  dsp_global_operation_head = NULL;
  
  while(1) {
    for(pos=0; pos<jackcli_samplerate; pos++) {
      /* process main inputs */
      temp_main_in = dsp_optimized_main_ins;
      i=0;
      while(temp_main_in != NULL) {
	temp_main_in->outs->sample->value = rtqueue_deq(jackcli_fifo_ins[i]);
	temp_main_in = temp_main_in->next;
	i += 1;
      }

      dsp_process(dsp_global_operation_head, jackcli_samplerate, pos);
      
      temp_main_out = dsp_optimized_main_outs;
      i=0;
      while(temp_main_out != NULL) {
	if(!rtqueue_isfull(jackcli_fifo_outs[i]))
	  rtqueue_enq(jackcli_fifo_outs[i], dsp_sum_summands(temp_main_out->ins->summands));
	temp_main_out = temp_main_out->next;
	i += 1;
      }
      
      if( dsp_global_new_operation_graph ) {
        printf("assigning new graph\n");
        dsp_global_operation_head = dsp_global_operation_head_processing;
        dsp_global_new_operation_graph = 0;
        dsp_global_operation_head_processing = NULL;
        printf("assigned new graph\n");
        printf("operations in new graph: \n");

        /* -- debug cruft */
	/* if(dsp_global_operation_head != NULL) { */
	/*   temp_op = dsp_global_operation_head; */
	/*   while(temp_op != NULL) { */
	/*     temp_op = temp_op->next; */
	/*   } */
	/* } */
        /* printf("done listing\n");         */
      }
    }
  }
  
  /* deallocate main inputs/outputs */
  /* NEED TO DO THIS BETTER (ie. iterate over outputs/inputs structs)*/
  free(dsp_optimized_main_ins);
  free(dsp_optimized_main_outs);

} /* dsp_thread */
