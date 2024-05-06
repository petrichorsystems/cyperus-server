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

static bool dsp_build_new_optimized_graph = false;;

int dsp_global_new_operation_graph = 0;
unsigned short dsp_global_period = 0;
float dsp_global_load = 0.0f;

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
	    port_in = dsp_port_in_init("in", 512);
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
dsp_add_bus(char *bus_id, struct dsp_bus *new_bus, char *ins, char *outs) {
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
  if( !strcmp(bus_id, "00000000-0000-0000-0000-000000000000") || !strcmp(bus_id, "")) {
    if( dsp_global_bus_head != NULL )
      dsp_bus_insert_tail(dsp_global_bus_head, new_bus);
    else
      dsp_global_bus_head = new_bus;
  } else {
    target_bus = dsp_find_bus(bus_id);
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
	       void (*dsp_function) (struct dsp_operation*, int),
               void (*dsp_osc_listener_function) (struct dsp_operation*, int),
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
  struct dsp_bus_port *temp_bus_port = NULL;
  struct dsp_port_out *port_out = NULL;
  struct dsp_port_in *port_in = NULL;

  port_out = dsp_find_main_in_port_out(id_out);
  if( port_out == NULL ) {
    if( (temp_bus_port = dsp_find_bus_port_in(id_out)) != NULL) {
      port_out = temp_bus_port->out;
    } else if( (temp_bus_port = dsp_find_bus_port_out(id_out)) != NULL) {
      port_out = temp_bus_port->out;
    } else {
      port_out = dsp_find_port_out(id_out);
    }
  }

  port_in = dsp_find_main_out_port_in(id_in);
  if( port_in == NULL ) {
    if( (temp_bus_port = dsp_find_bus_port_out(id_in)) != NULL) {
      port_in  = temp_bus_port->in;
    } else if( (temp_bus_port = dsp_find_bus_port_in(id_in)) != NULL) {
      port_in  = temp_bus_port->in;
    } else {
      port_in = dsp_find_port_in(id_in);
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
  
  dsp_build_new_optimized_graph = true;
  
  return 0;
} /* dsp_add_connection */


int
dsp_remove_connection(char *id_out, char *id_in) {
  struct dsp_connection *temp_connection;
  struct dsp_connection *found_connection;
  struct dsp_port_out *port_out = NULL;
  struct dsp_port_in *port_in = NULL;

  port_out = dsp_find_port_out(id_out);
  port_in = dsp_find_port_in(id_in);
  
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
dsp_optimize_connections_input(struct dsp_connection *connection) {

	/* is the below ever actually the case? */
	
	/* do we need to account for whether dsp_global_translation_connection_raph_processing is populated
	   versus dsp_global_operation_head_processing is not populated? do we care? */

	struct dsp_port_out *temp_port_out = NULL;
	struct dsp_port_in *temp_port_in = NULL;

	struct dsp_operation *temp_op_out = NULL;
	struct dsp_operation *temp_op_in = NULL;

	struct dsp_operation *matched_op_out = NULL;
	struct dsp_operation *matched_op_in = NULL;

	struct dsp_operation *op_out = NULL;
	struct dsp_operation *op_in = NULL;

	struct dsp_operation_sample *found_sample_out = NULL;
	struct dsp_operation_sample *found_sample_in = NULL;
  
	struct dsp_operation_sample *sample_out = NULL;
	struct dsp_operation_sample *sample_in = NULL;
  
	struct dsp_operation_sample *temp_sample_out = NULL;
	struct dsp_operation_sample *temp_sample_in = NULL;
	
	struct dsp_operation_sample *new_summand = NULL;

	struct dsp_translation_connection *temp_translation_conn = NULL;

	struct dsp_bus *temp_bus;
	struct dsp_module *temp_module;

	int is_main_in_out = 0;
	int is_bus_port_out = 0;
	int is_module_out = 0;
	
	int is_main_out_in = 0;
	int is_bus_port_in = 0;
	int is_module_in = 0;

	int module_out_exists = 0;
	
	int error_not_found;
  
	/* OUTPUT PROCESSING */
	error_not_found = 0;

	if( dsp_find_main_in_port_out((char *)connection->id_out) != NULL ) {
		is_main_in_out = 1;
	} else if( dsp_find_module_port_out((char *)connection->id_out) != NULL ) {
		is_module_out = 1;
	} else if( dsp_find_bus_port_out((char *)connection->id_out) != NULL ) {
		is_bus_port_out = 1;
	} else if( dsp_find_bus_port_in((char *)connection->id_out) != NULL ) {
		is_bus_port_out = 1;
	} else {
		printf("dsp.c::dsp_optimize_connections_input(), unexpected connection output -- id: '%s', exiting..\n", connection->id_out);
		exit(1);
	}
	
	if( is_main_in_out ) {
		temp_op_out = dsp_optimized_main_ins;
		while( temp_op_out != NULL ) {
			if( strcmp(temp_op_out->dsp_id, connection->id_out) == 0 ) {
				matched_op_out = temp_op_out;
				found_sample_out = temp_op_out->outs;
				break;
			}
			temp_op_out = temp_op_out->next;
		}

		if( matched_op_out == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching operation out for main_in_out with dsp id: %s\n", connection->id_out);
			error_not_found = 1;
		}
		if( found_sample_out == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching sample out for main_in_out with dsp id: %s\n", connection->id_out);      
			error_not_found = 1;
		}
		
		if( error_not_found ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: unmatched connection->id_out of dsp id: %s\n", connection->id_out);
			exit(1);
		}

		op_out = matched_op_out;
		sample_out = found_sample_out;
	}

	if( is_module_out )  {
		temp_module = dsp_get_module_from_port((char *)connection->id_out);
		
		if( temp_module == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), parent module not found for module port id: %s\n", connection->id_out);      
			error_not_found = 1;
		}
		
		/* look for existing module operation */
		temp_op_out = dsp_global_operation_head_processing;
		while( temp_op_out != NULL ) {
			if( strcmp(temp_op_out->dsp_id, temp_module->id) == 0 ) {
				matched_op_out = temp_op_out;
				break;
			}
			temp_op_out = temp_op_out->next;
		}
		
		/* instantiate module operation and insert into operation list */
		if( matched_op_out == NULL ) {
			matched_op_out = temp_module->dsp_optimize((char *)temp_module->id, temp_module);
			if(dsp_global_operation_head_processing == NULL) {
				dsp_global_operation_head_processing = matched_op_out;
			} else {          
				dsp_operation_insert_tail(dsp_global_operation_head_processing,
							  matched_op_out);
			}      
		}
		
		/* retrieve sample off of module operation */
		temp_sample_out = matched_op_out->outs;
		while( temp_sample_out != NULL ) {
			if( strcmp(temp_sample_out->dsp_id, (char *)connection->id_out) == 0 ) {
				found_sample_out = temp_sample_out;
				break;
			}
			temp_sample_out = temp_sample_out->next;
		}
		
		if( found_sample_out == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching sample out for module_out with dsp id: %s\n", connection->id_out);      
			error_not_found = 1;
		}    
		
		if( error_not_found ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: unmatched connection->id_out of dsp id: %s\n", connection->id_out);
			exit(1);
		}
		
		op_out = matched_op_out;
		sample_out = found_sample_out;
		
	}
	
	if( is_bus_port_out ) {
		/* look for existing bus port out port */
		temp_op_out = dsp_global_operation_head_processing;
		while( temp_op_out != NULL ) {
			if( strcmp(temp_op_out->dsp_id, connection->id_out) == 0 ) {
				matched_op_out = temp_op_out;
				break;
			}
			temp_op_out = temp_op_out->next;
		}

		/* if not found, instantiate operation, input port sample, and output port sample */
		if( matched_op_out == NULL ) {
			matched_op_out = dsp_operation_init(connection->id_out);
			found_sample_out = dsp_operation_sample_init("<bus port port out>", dsp_global_period, 0.0f, 1);
			if(dsp_global_operation_head_processing == NULL) {
				dsp_global_operation_head_processing = matched_op_out;
			} else {          
				dsp_operation_insert_tail(dsp_global_operation_head_processing,
							  matched_op_out);
			}
			
			matched_op_out->outs = found_sample_out;
			matched_op_out->ins = dsp_operation_sample_init("<bus port port in>", dsp_global_period, 0.0f, 1);
		}
		
		found_sample_out = matched_op_out->outs;
		
		if( matched_op_out == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching operation out for bus_port_out with dsp id: %s\n", connection->id_out);
			error_not_found = 1;
		}
		if( found_sample_out == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching sample out for bus_port_out with dsp id: %s\n", connection->id_out);      
			error_not_found = 1;
		}
		
		if( error_not_found ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: unmatched connection->id_out of dsp id: %s\n", connection->id_out);
			exit(1);
		}
		
		op_out = matched_op_out;
		sample_out = found_sample_out;
	}
	
	/* INPUT PROCESSING */
	error_not_found = 0;
	
	if( dsp_find_main_out_port_in((char *)connection->id_in) != NULL ) {
		is_main_out_in = 1;
	} else if( dsp_find_module_port_in((char *)connection->id_in) != NULL ) {
		is_module_in = 1;
	} else if( dsp_find_bus_port_in((char *)connection->id_in) != NULL ) {
		is_bus_port_in = 1;
	} else if( dsp_find_bus_port_out((char *)connection->id_in) != NULL ) {
		is_bus_port_in = 1;    
	} else {
		printf("unexpected connection input -- id: '%s', exiting..\n", connection->id_in);
		exit(1);
	}
	
	if( is_main_out_in ) {
		temp_op_in = dsp_optimized_main_outs;
		while( temp_op_in != NULL ) {
			if( strcmp(temp_op_in->dsp_id, connection->id_in) == 0 ) {
				matched_op_in = temp_op_in;
				found_sample_in = temp_op_in->ins;
				break;
			}
			temp_op_in = temp_op_in->next;
		}
		
		if( matched_op_in == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching operation in for main_out_in with dsp id: %s\n", connection->id_in);
			error_not_found = 1;
		}
		if( found_sample_in == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching sample in for main_out_in with dsp id: %s\n", connection->id_in);      
			error_not_found = 1;
		}
		
		if( error_not_found ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: unmatched connection->id_in of dsp id: %s\n", connection->id_in);
			exit(1);
		}
		
		op_in = matched_op_in;
		sample_in = found_sample_in;
	}
	
	if( is_module_in )  {
		temp_module = dsp_get_module_from_port((char *)connection->id_in);
		
		if( temp_module == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), parent module not found for module port id: %s\n", connection->id_in);      
			error_not_found = 1;
		}
		
		/* look for existing module operation */
		temp_op_in = dsp_global_operation_head_processing;
		while( temp_op_in != NULL ) {
			if( strcmp(temp_op_in->dsp_id, temp_module->id) == 0 ) {
				matched_op_in = temp_op_in;
				break;
			}
			temp_op_in = temp_op_in->next;
		}
		
		/* instantiate module operation and insert into operation list */
		if( matched_op_in == NULL ) {
			
			matched_op_in = temp_module->dsp_optimize((char *)temp_module->id, temp_module);
			if(dsp_global_operation_head_processing == NULL) {
				dsp_global_operation_head_processing = matched_op_in;
			} else {          
				dsp_operation_insert_tail(dsp_global_operation_head_processing,
							  matched_op_in);
			}
		}
		
		/* retrieve sample off of module operation */
		temp_sample_in = matched_op_in->ins;
		while( temp_sample_in != NULL ) {
			if( strcmp(temp_sample_in->dsp_id, (char *)connection->id_in) == 0 ) {
				found_sample_in = temp_sample_in;
				break;
			}
			temp_sample_in = temp_sample_in->next;
		}
		
		if( found_sample_in == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching sample in for module_in with dsp id: %s\n", connection->id_in);      
			error_not_found = 1;
		}    
		
		if( error_not_found ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: unmatched connection->id_in of dsp id: %s\n", connection->id_in);
			exit(1);
		}
		
		op_in = matched_op_in;
		sample_in = found_sample_in;
	}
	
	if( is_bus_port_in ) {
		/* look for existing bus port in port */
		temp_op_in = dsp_global_operation_head_processing;
		while( temp_op_in != NULL ) {
			if( strcmp(temp_op_in->dsp_id, connection->id_in) == 0 ) {
				matched_op_in = temp_op_in;
				break;
			}
			temp_op_in = temp_op_in->next;
		}
		
		/* if not found, instantiate operation, input port sample, and output port sample */
		if( matched_op_in == NULL ) {
			matched_op_in = dsp_operation_init(connection->id_in);
			found_sample_in = dsp_operation_sample_init("<bus port port in>", dsp_global_period, 0.0f, 1);
			if(dsp_global_operation_head_processing == NULL) {
				dsp_global_operation_head_processing = matched_op_in;
			} else {          
				dsp_operation_insert_tail(dsp_global_operation_head_processing,
							  matched_op_in);
			}
			matched_op_in->ins = found_sample_in;
			matched_op_in->outs = dsp_operation_sample_init("<bus port port out>", dsp_global_period, 0.0f, 1);
		}
		
		found_sample_in = matched_op_in->ins;
		
		if( matched_op_in == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching operation in for bus_port_in with dsp id: %s\n", connection->id_in);
			error_not_found = 1;
		}
		if( found_sample_in == NULL ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: could not find matching sample in for bus_port_in with dsp id: %s\n", connection->id_in);      
			error_not_found = 1;
		}
		
		if( error_not_found ) {
			printf("dsp.c::dsp_optimize_connections_input(), error: unmatched connection->id_in of dsp id: %s\n", connection->id_in);
			exit(1);
		}
		
		op_in = matched_op_in;
		sample_in = found_sample_in;
	}
	
	new_summand = dsp_operation_sample_init((char *)sample_out->dsp_id, dsp_global_period, 0.0, 0);
	new_summand->sample = sample_out->sample;
	if(sample_in->summands == NULL)
		sample_in->summands = new_summand;
	else
		dsp_operation_sample_insert_tail(sample_in->summands, new_summand);
	
} /* dsp_optimize_connections_input */

void
dsp_optimize_connections_bus(struct dsp_bus_port *ports) {
  
  struct dsp_bus_port *temp_port = ports;
  struct dsp_connection *temp_connection;

  int temp_port_idx = 0;

  while(temp_port != NULL) {
    temp_connection = dsp_global_connection_graph;
    while(temp_connection != NULL) {      
      if(strcmp(temp_port->id, temp_connection->id_out) == 0) {        
	dsp_optimize_connections_input(temp_connection);
      }
      temp_connection = temp_connection->next;
    }
    temp_port = temp_port->next;
    temp_port_idx++;

  }
} /* dsp_optimize_connections_bus */

void
dsp_optimize_graph(struct dsp_bus *head_bus) {
  struct dsp_module *temp_module;
  struct dsp_bus *temp_bus = head_bus;

  while(temp_bus != NULL) {
    /* process bus inputs */
    dsp_optimize_connections_bus(temp_bus->ins);

    /* handle dsp modules */
    temp_module = temp_bus->dsp_module_head;
    while(temp_module != NULL) {
      dsp_optimize_connections_module(temp_module->outs);
      temp_module = temp_module->next;
    }
    
    /* process bus outputs */
    dsp_optimize_connections_bus(temp_bus->outs);
    dsp_optimize_graph(temp_bus->down);
    temp_bus = temp_bus->next;
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
    } else {
      temp_port_out->next = dsp_port_out_init("main_in", 1);
      temp_port_out = temp_port_out->next;
    }
    
    temp_op = dsp_operation_init(temp_port_out->id);
    temp_sample = dsp_operation_sample_init("<main port out>", dsp_global_period, (float)0.0, 1);

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
      temp_port_in = dsp_port_in_init("main_out", fifo_size);
      dsp_main_outs = temp_port_in;
    } else {
      temp_port_in->next = dsp_port_in_init("main_out", fifo_size);
      temp_port_in = temp_port_in->next;
    }
  }
  dsp_build_optimized_main_outs();
} /* dsp_build_mains */

void
dsp_build_optimized_main_outs() {
  struct dsp_port_in *temp_port_in = NULL;

  struct dsp_operation *temp_op = NULL;
  struct dsp_operation_sample *temp_sample = NULL;

  int i;
  
  dsp_optimized_main_outs = NULL;
  temp_port_in = dsp_main_outs;
  while(temp_port_in != NULL) {
    temp_op = dsp_operation_init(temp_port_in->id);
    temp_sample = dsp_operation_sample_init("<main port in>", dsp_global_period, (float)0.0, 1);

    temp_op->ins = temp_sample;
    if( dsp_optimized_main_outs == NULL )
      dsp_optimized_main_outs = temp_op;
    else 
      dsp_operation_insert_tail(dsp_optimized_main_outs, temp_op);
    
    temp_port_in = temp_port_in->next;
  }
} /* dsp_build_optimized_main_outs */

void
*dsp_build_optimized_graph(void *arg) {
  int i;
  float outsample = 0.0;

  struct dsp_bus *temp_bus;
  struct dsp_module *temp_module;

  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_port_in *temp_port_in = NULL;

  dsp_global_operation_head_processing = NULL;

  dsp_build_optimized_main_outs();
  
  dsp_optimize_connections_main_inputs(dsp_main_ins);
  
  temp_bus = dsp_global_bus_head;
  while( temp_bus != NULL ) {
    dsp_optimize_graph(temp_bus);
    temp_bus = temp_bus->next;
  }

  dsp_global_new_operation_graph = 1;
} /* dsp_build_optimized_graph */

void *
dsp_graph_optimization_thread(void *arg) {

	pthread_mutex_lock(&dsp_global_optimization_mutex);
	
	dsp_build_new_optimized_graph = false;
	dsp_build_optimized_graph(NULL);
	/* graph changed, generate new graph id */
	dsp_graph_id_rebuild();
	
	pthread_mutex_unlock(&dsp_global_optimization_mutex);

} /* dsp_graph_optimization_thread */

int
dsp_graph_optimization_thread_setup() {
	pthread_t graph_optimization_thread_id;
	pthread_create(&graph_optimization_thread_id, NULL, dsp_graph_optimization_thread, NULL);
	pthread_detach(graph_optimization_thread_id);
	return 0;	
} /* dsp_graph_optimization_thread_setup */

void
dsp_process(struct dsp_operation *head_op, int jack_sr, int pos) {  
  struct dsp_connection *temp_connection = dsp_global_connection_graph;
  int connection_idx = 0;
  
  struct dsp_operation *temp_op = NULL;
  temp_op = head_op;
  int p;
  while(temp_op != NULL) {
    if( temp_op->module == NULL ) {
      if( temp_op->ins == NULL ) {
	      temp_op->id;
        memset(temp_op->outs->sample->value, 0.0f, sizeof(float) * dsp_global_period);
      } else {
	if( temp_op->ins != NULL ) {
		temp_op->id;
		dsp_sum_summands(temp_op->outs->sample->value, temp_op->ins->summands);
        }
      }
    } else {
	    temp_op->module->name;
	    temp_op->module->dsp_function(temp_op, jack_sr);
    }
    temp_op = temp_op->next;
  }
  if (dsp_build_new_optimized_graph) {
	  dsp_graph_optimization_thread_setup();
  }
} /* dsp_process */

void dsp_setup(unsigned short period, unsigned short channels_in, unsigned short channels_out) {
	pthread_mutex_init(&dsp_global_optimization_mutex, NULL);
	dsp_global_operation_head = NULL;
	dsp_global_period = period;
	dsp_build_mains(channels_in, channels_out);
} /* dsp_setup */
