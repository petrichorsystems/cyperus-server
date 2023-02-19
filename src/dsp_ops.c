/* dsp_ops.c
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

Copyright 2018 murray foster */

#include "dsp_ops.h"

dsp_parameter dsp_voice_parameters[0];

float
dsp_sum_summands(struct dsp_operation_sample *summands) {
  struct dsp_operation_sample *temp_summand = summands;
  float outsample = 0.0;

  /* TODO: Properly sum inputs? (be careful, what if not audio) */

  while(temp_summand != NULL) {
    outsample += temp_summand->sample->value;
    temp_summand = temp_summand->next;
  }

  return outsample;
} /* dsp_sum_input */


float
dsp_sum_input(struct dsp_port_in *in) {

  /* TODO: Properly sum inputs? (be careful, what if not audio) */

  float outsample = 0.0;
  while(rtqueue_isempty(in->values) == 0) {
    outsample += rtqueue_deq(in->values);
  }
  return outsample;
} /* dsp_sum_input */


void
dsp_feed_outputs(char *current_bus_path, char *module_id, struct dsp_port_out *outs) {
  struct dsp_port_out *temp_out;
  struct dsp_connection *temp_connection;
  float temp_outsample;
  temp_out = outs;
  char *current_path;

  if( dsp_global_connection_graph != NULL ) {
    temp_connection = dsp_global_connection_graph;
    while(temp_out != NULL) {
      temp_outsample = temp_out->value;
      while(temp_connection != NULL) {
	/* compare each connection 'out' with this one, enqueue each fifo with data
	   that matches the 'out' port path */
	current_path = (char *)malloc(strlen(current_bus_path) + strlen(module_id) + 1 + strlen(temp_out->id) + 1);
	if(current_path != NULL) {
	  current_path[0] = '\0';
	  strcpy(current_path, current_bus_path);
	  strcat(current_path, "?");
	  strcat(current_path, module_id);
	  strcat(current_path, ">");
	  strcat(current_path, temp_out->id);
	}	
	if(strcmp(current_path, temp_connection->id_out) == 0) {
	  rtqueue_enq(temp_connection->in_values, temp_outsample);

	  /* optimization logic */
	  
	}
	temp_connection = temp_connection->next;
	free(current_path);
      }
      temp_out = temp_out->next;
    }
  }
} /* dsp_feed_outputs */

void
dsp_optimize_connections_module(char *current_bus_path, char *module_id, struct dsp_port_out *outs) {
  struct dsp_port_out *temp_out;
  struct dsp_connection *temp_connection;
  float temp_outsample;
  temp_out = outs;
  char *current_path;

  if( dsp_global_connection_graph != NULL ) {
    temp_connection = dsp_global_connection_graph;
    while(temp_out != NULL) {
      temp_outsample = temp_out->value;
      while(temp_connection != NULL) {
	/* compare each connection 'out' with this one, enqueue each fifo with data
	   that matches the 'out' port path */
	current_path = (char *)malloc(strlen(current_bus_path) + strlen(module_id) + 1 + strlen(temp_out->id) + 1);
	if(current_path != NULL) {
	  current_path[0] = '\0';
	  strcpy(current_path, current_bus_path);
	  strcat(current_path, "?");
	  strcat(current_path, module_id);
	  strcat(current_path, ">");
	  strcat(current_path, temp_out->id);
	}
	if(strcmp(current_path, temp_connection->id_out) == 0) {
	  dsp_optimize_connections_input(current_path,
					 temp_connection);
	}
	temp_connection = temp_connection->next;
	free(current_path);
      }
      temp_out = temp_out->next;
    }
  }
} /* dsp_optimize_connections_module */

void
dsp_optimize_connections_main_inputs(struct dsp_port_out *outs) {
  struct dsp_port_out *temp_out;
  struct dsp_connection *temp_connection;
  float temp_outsample;
  char *current_path, *temp_op_in_path;

  struct dsp_operation *temp_op_out, *temp_op_in = NULL;
  struct dsp_operation_sample *temp_sample, *temp_sample_out, *temp_sample_in, *sample_in, *sample_out = NULL;
  struct dsp_operation_sample *new_summand = NULL;
  
  struct dsp_translation_connection *temp_translation_connection = NULL;

  char *temp_result[3];
  
  temp_out = outs;
  if( dsp_global_connection_graph != NULL ) {
    while(temp_out != NULL) {
      temp_outsample = temp_out->value;
      temp_connection = dsp_global_connection_graph;
      while(temp_connection != NULL) {
	/* compare each main connection 'out' with this one, enqueue each fifo with data
	   that matches the 'out' port path */
	current_path = (char *)malloc(44);
	if(current_path != NULL) {
	  current_path[0] = '\0';
	  strcpy(current_path, "/mains{");
	  strcat(current_path, temp_out->id);
	}
	if(strcmp(current_path, temp_connection->id_out) == 0) {
	  /* commented out data motion logic */
	  /* rtqueue_enq(temp_connection->in_values, temp_outsample); */

	  /* BEGIN OPTIMIZATION LOGIC */	  

	  /* are we assuming all connections are made to a bus in
	     the below logic? */

          temp_op_out = NULL;
          
	  /* find existing 'out' operation (main in) */
	  temp_op_out = dsp_optimized_main_ins;
	  while( temp_op_out != NULL ) {
            
	    if( strcmp(temp_op_out->dsp_id, temp_connection->id_out) == 0 ){

	      temp_sample_out = temp_op_out->outs;
	      break;
	    }
	    temp_op_out = temp_op_out->next;
	  }
          
	  /* let program know there's something majorly wrong,
	     exit */
	  if(temp_sample_out == NULL) {
	    printf("no operation describing main input!!: %s\n", temp_connection->id_out);
	    printf("BAILING\n"); 
	  }
          
	  /* look for 'in' operation */
	  temp_op_in = dsp_global_operation_head_processing;

	  int is_bus_port = 0;
	  
	  dsp_parse_path(temp_result, temp_connection->id_in);
	  if( strstr(":", temp_result[0]) ) {
	    temp_op_in_path = current_path;
	    is_bus_port = 1;
	  }
	  else if( strstr("<", temp_result[0]) )
	    temp_op_in_path = temp_result[1];
	  else if( strstr(">", temp_result[0]) ) {
	    printf("temp_connection->id_in: '%s', contains output! aborting..\n", (char *)temp_connection->id_in);
	    exit(1);
	  } else
	    temp_op_in_path = current_path;

          sample_in = NULL;
	  while( temp_op_in != NULL ) {
	    if( strcmp(temp_op_in->dsp_id, temp_op_in_path) == 0 ) {
                
	      temp_sample_in = temp_op_in->ins;
	      if( is_bus_port == 0) {
		while( temp_sample_in != NULL ) {
		  if( strcmp(temp_sample_in->dsp_id, temp_result[2]) == 0 ) {
		    sample_in = temp_sample_in;
		    break;
		  }
		  temp_sample_in->next = temp_sample_in;
		}
	      } else
		sample_in = temp_sample_in;
	      break;
	    }
	    temp_op_in = temp_op_in->next;
	  }

          
	  if( sample_in == NULL ) {
	    if( is_bus_port ) {
              sample_out = dsp_operation_sample_init("<bus port port out>", 0.0, 1);
	      sample_in = dsp_operation_sample_init("<bus port port in>", 0.0, 1);
              sample_out->sample = sample_in->sample;
	      temp_op_in = dsp_operation_init(temp_connection->id_in);
              temp_op_in->outs = sample_out;
              temp_op_in->ins = sample_in;
	    } else {
	      sample_in = dsp_operation_sample_init(temp_result[2], 0.0, 1);
	      temp_op_in = dsp_operation_init(temp_result[1]);

              printf("dsp_ops.c: dsp_optimize_connections_main_inputs(): ATTENTION: connection made from main input directly to module. do we allow this? exiting..\n");
              exit(1);
              
	    }

	    if(temp_op_in->ins == NULL)
	      temp_op_in->ins = sample_in;
	    else
	      dsp_operation_sample_insert_tail(temp_op_in->ins, sample_in);

            
	    if( dsp_global_operation_head_processing == NULL ) {
	      dsp_global_operation_head_processing = temp_op_in;
	    } else {
	      if(dsp_global_operation_head_processing == NULL)
		dsp_global_operation_head_processing = temp_op_in;
	      else
		dsp_operation_insert_tail(dsp_global_operation_head_processing,
					  temp_op_in);
	    }
	    temp_translation_connection = dsp_translation_connection_init(temp_connection,
									  (char *)temp_connection->id_out,
									  (char *)temp_connection->id_in,
									  temp_op_out,
									  temp_op_in,
									  temp_sample_out,
									  temp_op_in->ins);

	    if(dsp_global_translation_connection_graph_processing == NULL)
	      dsp_global_translation_connection_graph_processing = temp_translation_connection;
	    else
	      dsp_translation_connection_insert_tail(dsp_global_translation_connection_graph_processing,
						     temp_translation_connection);  /* is this last arg always the same? */
	  }


          new_summand = dsp_operation_sample_init((char *)temp_sample_out->dsp_id, 0.0, 0);
          new_summand->sample = temp_sample_out->sample;
          if( sample_in->summands == NULL )
            sample_in->summands = new_summand;
          else
            dsp_operation_sample_insert_tail(sample_in->summands, new_summand);
          

	  /* END OPTIMIZATION LOGIC */
	}
	temp_connection = temp_connection->next;
	free(current_path);
      }
      temp_out = temp_out->next;
    }
  }

} /* dsp_optimize_connections_main_inputs */

struct dsp_operation
*dsp_optimize_module(char *bus_path, struct dsp_module *module) {
  dsp_parameter dsp_param = module->dsp_param;

  struct dsp_port_in *temp_port_in = NULL;
  struct dsp_port_out *temp_port_out = NULL;
  struct dsp_operation_sample *temp_sample = NULL;
  struct dsp_operation *new_op = NULL;
  
  char *full_module_path = malloc(sizeof(char) * (strlen(bus_path) + strlen(module->id) + 2));
  snprintf(full_module_path, strlen(bus_path)+strlen(module->id)+2, "%s?%s", bus_path, module->id);
  new_op = dsp_operation_init(full_module_path);

  temp_port_in = module->ins;
  while(temp_port_in != NULL) {
    temp_sample = dsp_operation_sample_init((char *)temp_port_in->id, 0.0, 1);
    if(new_op->ins == NULL)
      new_op->ins = temp_sample;
    else
      dsp_operation_sample_insert_tail(new_op->ins, temp_sample);
    temp_port_in = temp_port_in->next;
  }

  temp_port_out = module->outs;
  while(temp_port_out != NULL) {
    temp_sample = dsp_operation_sample_init((char *)temp_port_out->id, 0.0, 1);
    if(new_op->outs == NULL)
      new_op->outs = temp_sample;
    else
      dsp_operation_sample_insert_tail(new_op->outs, temp_sample);

    temp_port_out = temp_port_out->next;
  }

  new_op->module = module;
  
  return new_op;
} /* dsp_optimize_module */

void dsp_osc_transmit(struct dsp_operation *osc_transmit, int jack_samplerate, int pos) {
  float insample = 0.0;

  if( osc_transmit->ins->next->summands != NULL ) {
    insample = dsp_sum_summands(osc_transmit->ins->summands);
  }

  if ( osc_transmit->module->dsp_param.osc_transmit.count % osc_transmit->module->dsp_param.osc_transmit.samplerate_divisor == 0 ) {
    lo_address lo_addr_send = lo_address_new((const char*)osc_transmit->module->dsp_param.osc_transmit.host, (const char*)osc_transmit->module->dsp_param.osc_transmit.port);
    lo_send(lo_addr_send, (const char*)osc_transmit->module->dsp_param.osc_transmit.path, "f", insample);
    free(lo_addr_send);
  }
  osc_transmit->module->dsp_param.osc_transmit.count++;
  
  return;
} /* dsp_osc_transmit */

int dsp_create_osc_transmit(struct dsp_bus *target_bus, char *host, char *port, char *path, int samplerate_divisor) {
  printf("start dsp_create_osc_transmit()\n");
  dsp_parameter osc_transmitter_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  osc_transmitter_param.pos = 0;
  osc_transmitter_param.osc_transmit.name = "osc transmitter";

  osc_transmitter_param.osc_transmit.host = malloc(sizeof(char) * (strlen(host) + 1));
  strcpy(osc_transmitter_param.osc_transmit.host, host);
  
  osc_transmitter_param.osc_transmit.port = malloc(sizeof(char) * (strlen(port) + 1));
  strcpy(osc_transmitter_param.osc_transmit.port, port);
  
  osc_transmitter_param.osc_transmit.path = malloc(sizeof(char) * (strlen(path) + 1));
  strcpy(osc_transmitter_param.osc_transmit.path, path);

  osc_transmitter_param.osc_transmit.samplerate_divisor = samplerate_divisor;
  osc_transmitter_param.osc_transmit.count = 0;
  
  ins = dsp_port_in_init("in", 512, NULL);

  dsp_add_module(target_bus,
                 "osc transmitter",
                 dsp_osc_transmit,
                 NULL,
                 dsp_optimize_module,
                 osc_transmitter_param,
                 ins,
                 NULL);
  printf("finish dsp_create_osc_transmit()\n");
  return 0;
} /* dsp_create_osc_transmit */

void dsp_edit_osc_transmit(struct dsp_module *osc_transmit, char *host, char *port, char *path, int samplerate_divisor) {
  printf("start dsp_edit_osc_transmit()\n");
  free(osc_transmit->dsp_param.osc_transmit.host);
  free(osc_transmit->dsp_param.osc_transmit.port);
  free(osc_transmit->dsp_param.osc_transmit.path);

  printf("finish free()s\n");
  
  osc_transmit->dsp_param.osc_transmit.host = malloc(sizeof(char) * (strlen(host) + 1));
  strcpy(osc_transmit->dsp_param.osc_transmit.host, host);
  
  osc_transmit->dsp_param.osc_transmit.port = malloc(sizeof(char) * (strlen(port) + 1));
  strcpy(osc_transmit->dsp_param.osc_transmit.port, port);
  
  osc_transmit->dsp_param.osc_transmit.path = malloc(sizeof(char) * (strlen(path) + 1));
  strcpy(osc_transmit->dsp_param.osc_transmit.path, path);

  osc_transmit->dsp_param.osc_transmit.samplerate_divisor = samplerate_divisor;  

  printf("finish dsp_edit_osc_transmit()\n");
  return;
} /* dsp_edit_osc_transmit */

