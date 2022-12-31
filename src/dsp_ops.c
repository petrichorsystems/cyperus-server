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

void
dsp_create_block_processor(struct dsp_bus *target_bus) {
  dsp_parameter block_processor_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  block_processor_param.pos = 0;
  block_processor_param.block_processor.name = "block_processor";
  block_processor_param.block_processor.cyperus_params = (struct cyperus_parameters *)malloc(sizeof(struct cyperus_parameters));
  
  block_processor_param.block_processor.cyperus_params->block_fifo = *rtqueue_init(96000);
  block_processor_param.block_processor.cyperus_params->signal_buffer = (float *)calloc(4096, sizeof(float));  
  block_processor_param.block_processor.cyperus_params->pos = 0;
  block_processor_param.block_processor.cyperus_params->avg = 0.0;

  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "block_processor",
		 dsp_block_processor,
		 dsp_optimize_module,
		 block_processor_param,
		 ins,
		 outs);
  return;
} /* dsp_create_block_processor */

void
dsp_block_processor(struct dsp_operation *block_processor, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = block_processor->module->dsp_param;

  /* sum inputs */
  insample = dsp_sum_summands(block_processor->ins->summands);
  
  /* process */
  block_processor->module->dsp_param.block_processor.cyperus_params->in = insample;
  outsample = cyperus_block_processor(block_processor->module->dsp_param.block_processor.cyperus_params,
				      jack_samplerate, pos);
  
  /* drive outputs */
   block_processor->outs->sample->value = outsample;

   return;
} /* dsp_block_processor */

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


int dsp_create_sawtooth(struct dsp_bus *target_bus, float freq, float amp) {
  dsp_parameter sawtooth_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  sawtooth_param.pos = 0;
  sawtooth_param.sawtooth.name = "sawtooth";
  sawtooth_param.sawtooth.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  sawtooth_param.sawtooth.freq = freq;
  sawtooth_param.sawtooth.amp = amp;

  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "sawtooth",
		 dsp_sawtooth,
		 dsp_optimize_module,
		 sawtooth_param,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_sawtooth */

void
dsp_edit_sawtooth(struct dsp_module *sawtooth, float freq, float amp) {
  sawtooth->dsp_param.sawtooth.freq = freq;
  sawtooth->dsp_param.sawtooth.amp = amp;
  
  return;
} /* dsp_edit_sawtooth */

void
dsp_sawtooth(struct dsp_operation *sawtooth, int jack_samplerate, int pos) {
  float outsample = 0.0;
  dsp_parameter dsp_param = sawtooth->module->dsp_param;

  sawtooth->module->dsp_param.sawtooth.cyperus_params->freq = sawtooth->module->dsp_param.sawtooth.freq;
  sawtooth->module->dsp_param.sawtooth.cyperus_params->amp = sawtooth->module->dsp_param.sawtooth.amp;
  
  outsample = cyperus_sawtooth(sawtooth->module->dsp_param.sawtooth.cyperus_params,
			   jack_samplerate, pos);
  
  /* drive audio outputs */
  sawtooth->outs->sample->value = outsample;
  
  return;
} /* dsp_sawtooth */

int dsp_create_triangle(struct dsp_bus *target_bus, float freq, float amp) {
  dsp_parameter triangle_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  triangle_param.pos = 0;
  triangle_param.triangle.name = "triangle";
  triangle_param.triangle.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  triangle_param.triangle.freq = freq;
  triangle_param.triangle.amp = amp;

  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "triangle",
		 dsp_triangle,
		 dsp_optimize_module,
		 triangle_param,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_triangle */

void
dsp_edit_triangle(struct dsp_module *triangle, float freq, float amp) {
  triangle->dsp_param.triangle.freq = freq;
  triangle->dsp_param.triangle.amp = amp;
  
  return;
} /* dsp_edit_triangle */

void
dsp_triangle(struct dsp_operation *triangle, int jack_samplerate, int pos) {
  float outsample = 0.0;
  dsp_parameter dsp_param = triangle->module->dsp_param;

  triangle->module->dsp_param.triangle.cyperus_params->freq = triangle->module->dsp_param.triangle.freq;
  triangle->module->dsp_param.triangle.cyperus_params->amp = triangle->module->dsp_param.triangle.amp;
  
  outsample = cyperus_triangle(triangle->module->dsp_param.triangle.cyperus_params,
			   jack_samplerate, pos);
  
  /* drive audio outputs */
  triangle->outs->sample->value = outsample;
  
  return;
} /* dsp_triangle */

void dsp_highpass(struct dsp_operation *highpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = highpass->module->dsp_param;

  insample = dsp_sum_summands(highpass->ins->summands);

  highpass->module->dsp_param.highpass.cyperus_params->in = insample;
  highpass->module->dsp_param.highpass.cyperus_params->amt = dsp_param.highpass.amt;
  highpass->module->dsp_param.highpass.cyperus_params->freq = dsp_param.highpass.freq;
  
  outsample = cyperus_highpass(highpass->module->dsp_param.highpass.cyperus_params, jack_samplerate, pos);

  highpass->outs->sample->value = outsample;
  
  return;
} /* dsp_highpass */


int dsp_create_highpass(struct dsp_bus *target_bus, float amt, float freq) {
  dsp_parameter filter_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  filter_param.pos = 0;
  filter_param.highpass.name = "highpass filter";
  filter_param.highpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filter_param.highpass.amt = amt;
   
  cyperus_highpass_init(filter_param.highpass.cyperus_params, jackcli_samplerate);

  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  
  dsp_add_module(target_bus,
                 "highpass filter",
                 dsp_highpass,
                 dsp_optimize_module,
                 filter_param,
                 ins,
                 outs);

  return 0;
} /* dsp_create_highpass */

void dsp_edit_highpass(struct dsp_module *highpass, float amt, float freq) {
  highpass->dsp_param.highpass.amt = amt;
  highpass->dsp_param.highpass.freq = freq;

  return;
} /* dsp_edit_highpass */

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


void dsp_bandpass(struct dsp_operation *bandpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = bandpass->module->dsp_param;

  insample = dsp_sum_summands(bandpass->ins->summands);


  /* set bandpass cutoff frequency if we have incoming data for that input */
  if( bandpass->ins->next->summands != NULL )
    dsp_param.bandpass.freq = dsp_sum_summands(bandpass->ins->next->summands) * jack_samplerate;

  
  bandpass->module->dsp_param.bandpass.cyperus_params->in = insample;
  bandpass->module->dsp_param.bandpass.cyperus_params->amt = dsp_param.bandpass.amt;
  bandpass->module->dsp_param.bandpass.cyperus_params->freq = dsp_param.bandpass.freq;
  bandpass->module->dsp_param.bandpass.cyperus_params->q = dsp_param.bandpass.q;

  
  outsample = cyperus_bandpass(bandpass->module->dsp_param.bandpass.cyperus_params, jack_samplerate, pos);

  bandpass->outs->sample->value = outsample;
  
  return;
} /* dsp_bandpass */


int dsp_create_bandpass(struct dsp_bus *target_bus, float amt, float freq, float q) {
  dsp_parameter filter_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  filter_param.pos = 0;
  filter_param.bandpass.name = "bandpass filter";
  filter_param.bandpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filter_param.bandpass.amt = amt;
  filter_param.bandpass.freq = freq;
  filter_param.bandpass.q = q;

  cyperus_bandpass_init(filter_param.bandpass.cyperus_params, jackcli_samplerate);

  ins = dsp_port_in_init("in", 512, NULL);
  ins->next = dsp_port_in_init("param_cutoff_freq", 512, &(filter_param.bandpass.freq));
    
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
                 "bandpass filter",
                 dsp_bandpass,
                 dsp_optimize_module,
                 filter_param,
                 ins,
                 outs);

  return 0;
} /* dsp_create_bandpass */

void dsp_edit_bandpass(struct dsp_module *bandpass, float amt, float freq, float q) {
  bandpass->dsp_param.bandpass.amt = amt;
  bandpass->dsp_param.bandpass.freq = freq;
  bandpass->dsp_param.bandpass.q = q;

  return;
} /* dsp_edit_bandpass */


int
dsp_create_pitch_shift(struct dsp_bus *target_bus, float amp, float shift, float mix) {
  dsp_parameter pitch_shift_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;

  pitch_shift_param.pos = 0;
  pitch_shift_param.pitch_shift.name = "pitch shift";
  pitch_shift_param.pitch_shift.cyperus_params = malloc(sizeof(struct cyperus_parameters));

  pitch_shift_param.pitch_shift.cyperus_params[0].block_fifo = *rtqueue_init(jackcli_samplerate*4);
  pitch_shift_param.pitch_shift.cyperus_params[0].signal_buffer = (float *)calloc(4096, sizeof(float));  
  pitch_shift_param.pitch_shift.cyperus_params[0].pos = 0;

  pitch_shift_param.pitch_shift.amp = amp;
  pitch_shift_param.pitch_shift.shift = shift;
  pitch_shift_param.pitch_shift.mix = mix;

  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
                 "pitch shift",
                 dsp_pitch_shift,
                 dsp_optimize_module,
                 pitch_shift_param,
                 ins,
                 outs);
  return 0;
} /* dsp_create_pitch_shift */

int
dsp_edit_pitch_shift(struct dsp_module *pitch_shift, float amp, float shift, float mix) {
  int i = 0;

  dsp_parameter dsp_param = pitch_shift->dsp_param;
  
  pitch_shift->dsp_param.pitch_shift.amp = amp;
  pitch_shift->dsp_param.pitch_shift.shift = shift;
  pitch_shift->dsp_param.pitch_shift.mix = mix;
  
  return 0;
} /* dsp_edit_pitch_shift */

void
dsp_pitch_shift(struct dsp_operation *pitch_shift, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = pitch_shift->module->dsp_param;

  /* sum audio */
  insample = dsp_sum_summands(pitch_shift->ins->summands);
  pitch_shift->module->dsp_param.pitch_shift.cyperus_params->in = insample;
  
  pitch_shift->module->dsp_param.pitch_shift.cyperus_params[0].amp = dsp_param.pitch_shift.amp;
  pitch_shift->module->dsp_param.pitch_shift.cyperus_params[0].shift = dsp_param.pitch_shift.shift;
  pitch_shift->module->dsp_param.pitch_shift.cyperus_params[0].mix = dsp_param.pitch_shift.mix;

  outsample = cyperus_pitch_shift(pitch_shift->module->dsp_param.pitch_shift.cyperus_params,
                                  jack_samplerate, pos);

  pitch_shift->outs->sample->value = outsample;

} /* dsp_pitch_shift */


int
dsp_create_karlsen_lowpass(struct dsp_bus *target_bus, float amp, float freq, float res) {
  dsp_parameter karlsen_lowpass_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  karlsen_lowpass_param.pos = 0;
  karlsen_lowpass_param.karlsen_lowpass.name = "apple biquad lowpass filter";
  karlsen_lowpass_param.karlsen_lowpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));

  karlsen_lowpass_param.karlsen_lowpass.amp = amp;
  karlsen_lowpass_param.karlsen_lowpass.freq = freq;
  karlsen_lowpass_param.karlsen_lowpass.res = res;

  
  karlsen_lowpass_param.karlsen_lowpass.cyperus_params[0].state0 = 0.0;
  karlsen_lowpass_param.karlsen_lowpass.cyperus_params[0].state1 = 0.0;
  karlsen_lowpass_param.karlsen_lowpass.cyperus_params[0].state2 = 0.0;
  karlsen_lowpass_param.karlsen_lowpass.cyperus_params[0].state3 = 0.0;

  
  ins = dsp_port_in_init("in", 512, NULL);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
                 "apple biquad lowpass filter",
                 dsp_karlsen_lowpass,
                 dsp_optimize_module,
                 karlsen_lowpass_param,
                 ins,
                 outs);
  return 0;
} /* dsp_create_karlsen_lowpass */

int
dsp_edit_karlsen_lowpass(struct dsp_module *karlsen_lowpass, float amp, float freq, float res) {
  int i = 0;

  dsp_parameter dsp_param = karlsen_lowpass->dsp_param;
  
  karlsen_lowpass->dsp_param.karlsen_lowpass.amp = amp;
  karlsen_lowpass->dsp_param.karlsen_lowpass.freq = freq;
  karlsen_lowpass->dsp_param.karlsen_lowpass.res= res;
  
  return 0;
} /* dsp_edit_karlsen_lowpass */

void
dsp_karlsen_lowpass(struct dsp_operation *karlsen_lowpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = karlsen_lowpass->module->dsp_param;

  /* sum audio */
  insample = dsp_sum_summands(karlsen_lowpass->ins->summands);
  karlsen_lowpass->module->dsp_param.karlsen_lowpass.cyperus_params->in = insample;
  
  karlsen_lowpass->module->dsp_param.karlsen_lowpass.cyperus_params[0].amp = dsp_param.karlsen_lowpass.amp;
  karlsen_lowpass->module->dsp_param.karlsen_lowpass.cyperus_params[0].freq = dsp_param.karlsen_lowpass.freq;
  karlsen_lowpass->module->dsp_param.karlsen_lowpass.cyperus_params[0].res = dsp_param.karlsen_lowpass.res;

  outsample = cyperus_karlsen_lowpass(karlsen_lowpass->module->dsp_param.karlsen_lowpass.cyperus_params,
                                  jack_samplerate, pos);

  karlsen_lowpass->outs->sample->value = outsample;
  
} /* dsp_karlsen_lowpass */





/* ================= FUNCTIONS BELOW NEED TO BE CONVERTED TO USE dsp_* OBJECTS ==================== */

int
dsp_create_pinknoise(void) {
  dsp_parameter pinknoise_param;
  pinknoise_param.pos = 0;
  pinknoise_param.pinknoise.name = "pinknoise";
  pinknoise_param.pinknoise.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  //dsp_add_module(dsp_pinknoise,pinknoise_param);
  return 0;
} /* dsp_create_pinknoise */

void
dsp_pinknoise(dsp_parameter noise_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  
  outsample = cyperus_pinknoise(&(noise_param.pinknoise.cyperus_params[0]),jack_samplerate,pos);
} /* dsp_pinknoise */
