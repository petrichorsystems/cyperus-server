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
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

'cyperus' is a JACK client for learning about software synthesis

Copyright 2015 murray foster */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);

#include "cyperus.h"
#include "rtqueue.h"
#include "libcyperus.h"
#include "dsp.h"
#include "dsp_types.h"
#include "dsp_ops.h"
#include "jackcli.h"

#define MAX_PATH_ID_LENGTH 16384

int fifo_out_is_waiting = 0;
pthread_mutex_t fifo_out_is_waiting_mutex;
pthread_cond_t fifo_out_is_waiting_cond;

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
dsp_parse_path(char* result[], char *path) {
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
  parsed_path[++i] = '\0';
  for(i=0; i < id_length; i++)
    parsed_id[i] = temp_string[strlen(path) - id_length + i];
  parsed_id[++i] = '\0';
  
  if( strcmp(result[0], "/") )
    result[1] = parsed_path;
  else
    result[1] = path;
  result[2] = parsed_id;
  return;
} /* dsp_parse_path */

struct dsp_bus*
dsp_parse_bus_path(char *target_path) {
  char target_bus_path[256];
  strcpy(target_bus_path, target_path);
  char *output_token;
  char *path_elem;
  char *last_bus_id;
  struct dsp_bus *temp_bus, *target_bus, *temp_bus_head, *deep_bus;
  temp_bus = dsp_global_bus_head;
  char *p = target_path; 
  char *path_index[256];
  int bus_count = 0;
  int path_count = 0;

  p = target_path;
  while (*p)                      /* while not end of string */
    {
      char *sp = p;                  /* set a start pointer */
      while (*p && *p != '/') p++;   /* advance to space    */
      output_token = malloc (p - sp + 1); /* allocate */
      strncpy (output_token, sp, p - sp);         /* copy   */
      output_token[p - sp] = 0;   /* force null-termination */
      if( output_token != NULL ) {
	if( strcmp(output_token, "") != 0 ) {
	  path_index[bus_count] = output_token;
	  bus_count += 1;
	}
      }
      while (*p && *p == '/') p++;   /* find next non-space */
    }

  last_bus_id = path_index[bus_count - 1];
  p = target_path;
  while (*p)                      /* while not end of string */
    {
      char *sp = p;                  /* set a start pointer */
      while (*p && *p != '/') p++;   /* advance to space    */
      output_token = malloc (p - sp + 1); /* allocate */
      strncpy (output_token, sp, p - sp);         /* copy   */
      output_token[p - sp] = 0;   /* force null-termination */
      if( output_token != NULL ) {
	if( strcmp(output_token, "") != 0 ) {
	  while( temp_bus != NULL ) {
	    if( strcmp(path_index[path_count], temp_bus->id) == 0) {
	      path_count += 1;
	      target_bus = temp_bus;
	      if( (strcmp(temp_bus->id, last_bus_id) == 0) &&
		  (bus_count == path_count) ) {
		return temp_bus;
	      } else 
		if(temp_bus->down != NULL) {
		  temp_bus = temp_bus->down;
		  break;
		} else {
		  fprintf(stderr, "ain't no bus existin': %s!\n", temp_bus->id);
		  return NULL;
		}
	    } 
	    temp_bus = temp_bus->next;
	  }
	}
      }
      while (*p && *p == '/') p++;   /* find next non-space */
    }
  return NULL;
} /* dsp_parse_bus_path */


struct dsp_bus_port*
dsp_build_bus_ports(struct dsp_bus_port *head_bus_port,
		    char *bus_ports, int out) {

  char target_bus_ports[256];
  strcpy(target_bus_ports, bus_ports);

  char *port_id;
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_bus_port *target_bus_port = NULL;
  struct dsp_port_in *port_in = NULL;
  struct dsp_port_out *port_out = NULL;
  int match_found = 0;
  char *p = target_bus_ports;
  temp_bus_port = head_bus_port;
  while (*p)                      /* while not end of string */
    {
      char *sp = p;                  /* set a start pointer */
      while (*p && *p != ',') p++;   /* advance to space    */
      char *output_token = malloc (p - sp + 1); /* allocate */
      strncpy (output_token, sp, p - sp);         /* copy   */
      output_token[p - sp] = 0;   /* force null-termination */
      match_found = 0;
      if( output_token != NULL ) {
	if( strcmp(output_token, "") != 0 ) {

	  while( temp_bus_port != NULL ) {
	    temp_bus_port = temp_bus_port->next;
	  }

	  if( !match_found ) {
	    temp_bus_port = dsp_bus_port_init(output_token, out);
	    port_in = dsp_port_in_init("in", 512);
	    port_out = dsp_port_out_init("out", 1);
	    temp_bus_port->in = port_in;
	    temp_bus_port->out = port_out;
	    
	    if(head_bus_port != NULL) {
	      dsp_bus_port_insert_tail(head_bus_port, temp_bus_port);
	    }
	    else {
	      head_bus_port = temp_bus_port;
	    }
	  }
	}
      }
      while (*p && *p == '/') p++;   /* find next non-space */
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
    return;
  }
  /* if it's not a head bus, parse down path and add */
  target_bus = dsp_parse_bus_path(target_bus_path);
  if(target_bus != NULL) {
    temp_bus = target_bus->down;
    if (temp_bus != NULL)
      dsp_bus_insert_tail(temp_bus, new_bus);
    else
      target_bus->down = new_bus;
  }
  else {
    target_bus = new_bus;
  }
  return;
} /* dsp_add_bus */

void
dsp_add_module(struct dsp_bus *target_bus,
	       char *name,
	       void (*dsp_function) (char*, struct dsp_module*, int, int),
	       dsp_parameter dsp_param,
	       struct dsp_port_in *ins,
	       struct dsp_port_out *outs) {
  struct dsp_module *new_module  = dsp_module_init(name,
						   dsp_function,
						   dsp_param,
						   ins,
						   outs);
  if( target_bus->dsp_module_head == NULL )
    target_bus->dsp_module_head = new_module;
  else
    dsp_module_insert_tail(target_bus->dsp_module_head, new_module);
  return;


} /* dsp_add_module */

void
dsp_remove_module(struct dsp_module *module, int remove) {
  module->remove = remove;
  return;
} /* dsp_remove_module */

void
dsp_bypass_module(struct dsp_module *module, int bypass) {
  module->bypass = bypass;
  return;
} /* dsp_bypass */

void
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
    port_out = target_bus_port->out;
  }

  /* parsing id_in (to connection input) */
  dsp_parse_path(temp_result, id_in);
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
    port_in = target_bus_port->in;
  }

  if( (port_out == NULL) ||
      (port_in == NULL) ) {
    fprintf(stderr, "failed to add connection!\n");
    return;
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
  return;
} /* dsp_add_connection */

char*
param_to_module_name(dsp_parameter module) {
  switch( module.type ) {
  case DSP_NULL_PARAMETER_ID:
    module.null.name = "null";
    return module.null.name;
  case DSP_SINE_PARAMETER_ID:
    return module.sine.name;
  case DSP_SQUARE_PARAMETER_ID:
    return module.square.name;
  case DSP_PINKNOISE_PARAMETER_ID:
    return module.pinknoise.name;
  case DSP_BUTTERWORTH_BIQUAD_LOWPASS_PARAMETER_ID:
    return module.butterworth_biquad_lowpass.name;
  case DSP_DELAY_PARAMETER_ID:
    return module.delay.name;
  case DSP_VOCODER_PARAMETER_ID:
    return module.vocoder.name;
  case DSP_BLOCK_PROCESSOR_PARAMETER_ID:
    return module.block_processor.name;
  case DSP_PITCH_SHIFT_PARAMETER_ID:
    return module.pitch_shift.name;
  default:
    module.null.name="null";
    return module.null.name;
  }
} /* param_to_module_name */

char*
dsp_list_modules() {
  
} /* dsp_list_modules */

void
dsp_feed_connections_bus(char *current_bus_path, struct dsp_bus_port *ports) {
  struct dsp_bus_port *temp_port = ports;
  struct dsp_connection *temp_connection;
  float temp_sample_in;
  char *current_path = NULL;

  int temp_port_idx = 0; 
  while(temp_port != NULL) {
    /* handle bus input */
    temp_sample_in = 0.0;
    while(rtqueue_isempty(temp_port->in->values) == 0) {
      temp_sample_in += rtqueue_deq(temp_port->in->values);
    }
    temp_port->out->value = temp_sample_in;
    /* enqueue out samples on in fifo's based on connection graph */
    temp_connection = dsp_global_connection_graph;

    while(temp_connection != NULL) {
      /* compare each connection 'out' with this one, enqueue each fifo with data
	 that matches the 'out' port path */
      current_path = (char *)malloc(strlen(current_bus_path) + strlen(temp_port->id) + 1);
      if(current_path != NULL) {
	current_path[0] = '\0';
	strcpy(current_path, current_bus_path);
	current_path[strlen(current_bus_path)] = '\0'; /* expecting a '/' on the end, remove it */
	strcat(current_path, ":");
	strcat(current_path, temp_port->id);
      }
      if(strcmp(current_path, temp_connection->id_out) == 0) {
	rtqueue_enq(temp_connection->in_values, temp_sample_in);
      }
      temp_connection = temp_connection->next;
    }
    temp_port = temp_port->next;
    temp_port_idx++;
  }
} /* dsp_feed_connections_bus */

void
recurse_dsp_graph(struct dsp_bus *head_bus, char *path, int jack_sr, int pos) {
  struct dsp_module *temp_module;
  struct dsp_bus *temp_bus = head_bus;
  char *current_path;  
  while(temp_bus != NULL) {
    /* build current path */
    if((*current_path = malloc(strlen(path) + strlen(temp_bus->id) + 1) != NULL)) {
      current_path[0] = '\0';
      strcat(current_path, path);
      strcat(current_path, temp_bus->id);
      strcat(current_path, "/");
    } else {
      printf("something went super wrong in recurse_dsp_graph()!!");
    }
    /* process bus inputs */
    dsp_feed_connections_bus(current_path, temp_bus->ins);
    /* handle dsp modules */
    temp_module = temp_bus->dsp_module_head;
    while(temp_module != NULL) {

      // construct module path?

      temp_module->dsp_function(current_path, temp_module, jack_sr, pos);
      temp_module = temp_module->next;
    }
    /* process bus outputs */
    dsp_feed_connections_bus(current_path, temp_bus->outs);
    recurse_dsp_graph(temp_bus->down, current_path, jack_sr, pos);
    temp_bus = temp_bus->next;
  }
  return;
} /* recurse_dsp_graph */

int jackcli_channels_in = 8;
int jackcli_channels_out = 8;
int jackcli_fifo_size = 2048;

void
dsp_mains_allocate(int channels_in, int channels_out, int fifo_size) {
  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  int i;
  
  for(i=0; i<channels_in; i++)
    if( i == 0 ) {
      temp_port_out = dsp_port_out_init("main_in", 1);
      dsp_main_ins = temp_port_out;
    } else {
      temp_port_out->next = dsp_port_out_init("main_in", 1);
      temp_port_out = temp_port_out->next;
    }

  for(i=0; i<channels_out; i++)
    if( i == 0 ) {
      temp_port_in = dsp_port_in_init("main_out", fifo_size);
      dsp_main_outs = temp_port_in;
    } else {
      temp_port_in->next = dsp_port_in_init("main_out", fifo_size);
      temp_port_in = temp_port_in->next;
    }
}

void
*dsp_thread(void *arg) {
  int pos, i;
  float outsample = 0.0;
  char current_path[2] = "/";
  
  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;

  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_port_in *temp_port_in = NULL;
  
  dsp_mains_allocate(jackcli_channels_in, jackcli_channels_out, jackcli_fifo_size);

  while(1) {
    for(pos=0; pos<jack_sr; pos++) {
      outsample = 0.0;

      /* process main inputs */
      temp_port_out = dsp_main_ins;
      i=0;
      while(temp_port_out != NULL) {
	temp_port_out->value = rtqueue_deq(jackcli_fifo_ins[i]);
	temp_port_out = temp_port_out->next;
	i += 1;
      }
      
      temp_bus = dsp_global_bus_head;
      while( temp_bus != NULL ) {
	recurse_dsp_graph(temp_bus, current_path, jack_sr, pos);
	temp_bus = temp_bus->next;
      }
      
      /* process main outputs */
      temp_port_in = dsp_main_outs;
      i=0;
      while(temp_port_in != NULL) {
	rtqueue_enq(jackcli_fifo_outs[i], dsp_sum_input(temp_port_in));
	temp_port_in = temp_port_in->next;
	i += 1;
      }
    }

    /* deallocate main inputs/outputs */
    free(dsp_main_ins);
    free(dsp_main_outs);

  }
} /* dsp_thread */

