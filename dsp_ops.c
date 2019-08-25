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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "jackcli.h"
#include "cyperus.h"
#include "dsp_math.h"
#include "dsp.h"
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
	  /* commented out data movement logic */
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
	    printf("temp_connection->id_in: '%s', contains output! aborting..\n");
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
									  temp_connection->id_out,
									  temp_connection->id_in,
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


          new_summand = dsp_operation_sample_init(temp_sample_out->dsp_id, 0.0, 0);
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
  block_processor_param.type = DSP_BLOCK_PROCESSOR_PARAMETER_ID;
  block_processor_param.pos = 0;
  block_processor_param.block_processor.name = "block_processor";
  block_processor_param.block_processor.cyperus_params = (struct cyperus_parameters *)malloc(sizeof(struct cyperus_parameters));
  
  block_processor_param.block_processor.cyperus_params->block_fifo = *rtqueue_init(96000);
  block_processor_param.block_processor.cyperus_params->signal_buffer = (float *)calloc(4096, sizeof(float));  
  block_processor_param.block_processor.cyperus_params->pos = 0;
  block_processor_param.block_processor.cyperus_params->avg = 0.0;

  ins = dsp_port_in_init("in", 512);
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
    temp_sample = dsp_operation_sample_init(temp_port_in->id, 0.0, 1);
    if(new_op->ins == NULL)
      new_op->ins = temp_sample;
    else
      dsp_operation_sample_insert_tail(new_op->ins, temp_sample);
    temp_port_in = temp_port_in->next;
  }

  temp_port_out = module->outs;
  while(temp_port_out != NULL) {
    temp_sample = dsp_operation_sample_init(temp_port_out->id, 0.0, 1);
    if(new_op->outs == NULL)
      new_op->outs = temp_sample;
    else
      dsp_operation_sample_insert_tail(new_op->outs, temp_sample);

    temp_port_out = temp_port_out->next;
  }

  new_op->module = module;
  
  return new_op;
} /* dsp_optimize_module */

int
dsp_create_delay(struct dsp_bus *target_bus, float amt, float time, float feedback) {
  dsp_parameter delay_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  delay_param.type = DSP_DELAY_PARAMETER_ID;
  delay_param.pos = 0;
  delay_param.delay.name = "delay";
  delay_param.delay.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  delay_param.delay.amt = amt;
  delay_param.delay.time = time * jackcli_samplerate;
  delay_param.delay.feedback = feedback;
  delay_param.delay.cyperus_params[0].signal_buffer = (float *)calloc(time * jackcli_samplerate * 30, sizeof(float));

  delay_param.delay.cyperus_params[0].pos = 0;
  delay_param.delay.cyperus_params[0].delay_pos = 0;
  
  ins = dsp_port_in_init("in", 512);
  ins->next = dsp_port_in_init("param_time", 512);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "delay",
		 dsp_delay,
		 dsp_optimize_module,
		 delay_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_delay */

void
dsp_delay(struct dsp_operation *delay, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = delay->module->dsp_param;

  /* sum audio */
  insample = dsp_sum_summands(delay->ins->summands);
  delay->module->dsp_param.delay.cyperus_params->in = insample;
  
  /* set initial delay amount */

  /* set delay time if we have incoming data for that input */
  if( delay->ins->next->summands != NULL )
    dsp_param.delay.time = dsp_sum_summands(delay->ins->next->summands) * jack_samplerate;

  delay->module->dsp_param.delay.cyperus_params->delay_amt = dsp_param.delay.amt;
  delay->module->dsp_param.delay.cyperus_params->delay_time = dsp_param.delay.time;
  delay->module->dsp_param.delay.cyperus_params->fb = dsp_param.delay.feedback;

  outsample = cyperus_delay(delay->module->dsp_param.delay.cyperus_params,
			    jack_samplerate, pos);

  /* drive audio outputs */
  delay->outs->sample->value = outsample;

  return;
} /* dsp_delay */


void dsp_edit_delay(struct dsp_module *delay, float amt, float time, float feedback) {
  int i = 0;

  dsp_parameter dsp_param = delay->dsp_param;
  
  printf("about to assign amt\n");
  delay->dsp_param.delay.amt = amt;
  printf("assigned delay->dsp_param.delay.amt: %f\n", delay->dsp_param.delay.amt);
  printf("about to assign time\n");
  delay->dsp_param.delay.time = time * jackcli_samplerate;
  printf("assigned delay->dsp_param.delay.time: %f\n", delay->dsp_param.delay.time);
  printf("about to assign feedback\n");
  delay->dsp_param.delay.feedback = feedback;
  printf("assigned delay->dsp_param.delay.feedback: %f\n", delay->dsp_param.delay.feedback);
  
  /*
    dsp_voice_parameters[module_no].delay.cyperus_params[0].pos = 0;
    dsp_voice_parameters[module_no].delay.cyperus_params[0].delay_pos = 0;
  */

  printf("returning\n");
  
} /* dsp_edit_delay */

int dsp_create_sine(struct dsp_bus *target_bus, float freq, float amp, float phase) {
  dsp_parameter sine_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  sine_param.type = DSP_SINE_PARAMETER_ID;
  sine_param.pos = 0;
  sine_param.sine.name = "sine";
  sine_param.sine.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  sine_param.sine.freq = freq;
  sine_param.sine.amp = amp;
  sine_param.sine.phase = phase;
  
  sine_param.sine.cyperus_params[0].block_fifo = *rtqueue_init(jackcli_samplerate*2);
  sine_param.sine.cyperus_params[0].last_freq = freq;
  sine_param.sine.cyperus_params[0].phase_delta = 0.0;

  ins = dsp_port_in_init("in", 512);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "sine",
		 dsp_sine,
		 dsp_optimize_module,
		 sine_param,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_sine */

void
dsp_edit_sine(struct dsp_module *sine, float freq, float amp, float phase) {
  sine->dsp_param.sine.freq = freq;
  sine->dsp_param.sine.amp = amp;
  sine->dsp_param.sine.phase = phase;
  
  return;
} /* dsp_edit_sine */

void
dsp_sine(struct dsp_operation *sine, int jack_samplerate, int pos) {
  float outsample = 0.0;
  dsp_parameter dsp_param = sine->module->dsp_param;

  sine->module->dsp_param.sine.cyperus_params->freq = sine->module->dsp_param.sine.freq;
  sine->module->dsp_param.sine.cyperus_params->amp = sine->module->dsp_param.sine.amp;
  sine->module->dsp_param.sine.cyperus_params->phase = sine->module->dsp_param.sine.phase;
  
  outsample = cyperus_sine(sine->module->dsp_param.sine.cyperus_params,
			   jack_samplerate, pos);
  
  /* drive audio outputs */
  sine->outs->sample->value = outsample;
  
  return;
} /* dsp_sine */

int
dsp_create_square(struct dsp_bus *target_bus, float freq, float amp) {
  dsp_parameter square_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  square_param.type = DSP_SQUARE_PARAMETER_ID;
  square_param.pos = 0;
  square_param.square.name = "square";
  square_param.square.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  square_param.square.freq = freq;
  square_param.square.amp = amp;
  
  ins = dsp_port_in_init("in", 512);
  outs = dsp_port_out_init("out", 1);

  dsp_add_module(target_bus,
                 "square",
                 dsp_square,
                 dsp_optimize_module,
                 square_param,
                 ins,
                 outs);
  
  return 0;
} /* dsp_create_square */

void
dsp_edit_square(struct dsp_module *square, float freq, float amp) {
  square->dsp_param.square.freq = freq;
  square->dsp_param.square.amp = amp;

  return;
} /* dsp_edit_square */

void
dsp_square(struct dsp_operation *square, int jack_samplerate, int pos) {
  float outsample = 0.0;
  dsp_parameter dsp_param = square->module->dsp_param;

  square->module->dsp_param.square.cyperus_params->freq = dsp_param.square.freq;
  square->module->dsp_param.square.cyperus_params->amp = dsp_param.square.amp;
  
  outsample = cyperus_square(square->module->dsp_param.square.cyperus_params,
                             jack_samplerate,pos);

  square->outs->sample->value = outsample;
  
  return;
} /* dsp_square */


int
dsp_create_envelope_follower(struct dsp_bus *target_bus, float attack, float decay, float scale) {
  dsp_parameter envelope_follower_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  envelope_follower_param.type = DSP_ENVELOPE_FOLLOWER_PARAMETER_ID;
  envelope_follower_param.pos = 0;
  envelope_follower_param.envelope_follower.name = "envelope_follower";
  envelope_follower_param.envelope_follower.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  envelope_follower_param.envelope_follower.attack = attack;
  envelope_follower_param.envelope_follower.decay = decay;
  envelope_follower_param.envelope_follower.scale = scale;
  envelope_follower_param.envelope_follower.cyperus_params[0].signal_buffer = (float *)calloc(1, sizeof(float));

  ins = dsp_port_in_init("in", 512);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
		 "envelope_follower",
		 dsp_envelope_follower,
		 dsp_optimize_module,
		 envelope_follower_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_envelope_follower */

void
dsp_envelope_follower(struct dsp_operation *envelope_follower, int jack_samplerate, int pos) {

  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = envelope_follower->module->dsp_param;

  
  /* sum audio inputs */
  insample = dsp_sum_summands(envelope_follower->ins->summands);
  envelope_follower->module->dsp_param.envelope_follower.cyperus_params->in = insample;
  
  envelope_follower->module->dsp_param.envelope_follower.cyperus_params->attack = dsp_param.envelope_follower.attack;
  envelope_follower->module->dsp_param.envelope_follower.cyperus_params->decay = dsp_param.envelope_follower.decay;
  envelope_follower->module->dsp_param.envelope_follower.cyperus_params->scale = dsp_param.envelope_follower.scale;

  outsample = cyperus_envelope_follower(envelope_follower->module->dsp_param.envelope_follower.cyperus_params,
			    jack_samplerate, pos);
  
  /* drive audio outputs */
  envelope_follower->outs->sample->value = outsample;

  return;
} /* dsp_envelope_follower */

void dsp_edit_envelope_follower(struct dsp_module *envelope_follower, float attack, float decay, float scale) {
  int i = 0;
  dsp_parameter dsp_param = envelope_follower->dsp_param;
  
  dsp_param.envelope_follower.attack = attack;
  dsp_param.envelope_follower.decay = decay;
  dsp_param.envelope_follower.scale = scale;
  
} /* dsp_edit_envelope_follower */


void dsp_lowpass(struct dsp_operation *lowpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = lowpass->module->dsp_param;

  insample = dsp_sum_summands(lowpass->ins->summands);

  lowpass->module->dsp_param.lowpass.cyperus_params->in = insample;
  lowpass->module->dsp_param.lowpass.cyperus_params->amt = dsp_param.lowpass.amt;
  lowpass->module->dsp_param.lowpass.cyperus_params->freq = dsp_param.lowpass.freq;

  outsample = cyperus_lowpass(lowpass->module->dsp_param.lowpass.cyperus_params, jack_samplerate, pos);

  lowpass->outs->sample->value = outsample;
  
  return;
} /* dsp_lowpass */


int dsp_create_lowpass(struct dsp_bus *target_bus, float amt, float freq) {
  dsp_parameter filter_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  
  filter_param.type = DSP_LOWPASS_PARAMETER_ID;
  filter_param.pos = 0;
  filter_param.lowpass.name = "lowpass filter";
  filter_param.lowpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filter_param.lowpass.amt = amt;
  filter_param.lowpass.freq = freq;

  cyperus_lowpass_init(filter_param.lowpass.cyperus_params, jackcli_samplerate);

  ins = dsp_port_in_init("in", 512);
  outs = dsp_port_out_init("out", 1);
  dsp_add_module(target_bus,
                 "lowpass filter",
                 dsp_lowpass,
                 dsp_optimize_module,
                 filter_param,
                 ins,
                 outs);

  return 0;
} /* dsp_create_lowpass */

void dsp_edit_lowpass(struct dsp_module *lowpass, float amt, float freq) {
  lowpass->dsp_param.lowpass.amt = amt;
  lowpass->dsp_param.lowpass.freq = freq;

  printf("returning\n");
  return;
} /* dsp_edit_lowpass */

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
  
  filter_param.type = DSP_HIGHPASS_PARAMETER_ID;
  filter_param.pos = 0;
  filter_param.highpass.name = "highpass filter";
  filter_param.highpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filter_param.highpass.amt = amt;
  filter_param.highpass.freq = freq;

  cyperus_highpass_init(filter_param.highpass.cyperus_params, jackcli_samplerate);

  ins = dsp_port_in_init("in", 512);
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


void dsp_bandpass(struct dsp_operation *bandpass, int jack_samplerate, int pos) {
  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = bandpass->module->dsp_param;

  insample = dsp_sum_summands(bandpass->ins->summands);

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
  
  filter_param.type = DSP_BANDPASS_PARAMETER_ID;
  filter_param.pos = 0;
  filter_param.bandpass.name = "bandpass filter";
  filter_param.bandpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filter_param.bandpass.amt = amt;
  filter_param.bandpass.freq = freq;
  filter_param.bandpass.q = q;

  cyperus_bandpass_init(filter_param.bandpass.cyperus_params, jackcli_samplerate);

  ins = dsp_port_in_init("in", 512);
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
  pitch_shift_param.type = DSP_PITCH_SHIFT_PARAMETER_ID;
  pitch_shift_param.pos = 0;
  pitch_shift_param.pitch_shift.name = "pitch shift";
  pitch_shift_param.pitch_shift.cyperus_params = malloc(sizeof(struct cyperus_parameters));

  pitch_shift_param.pitch_shift.cyperus_params[0].block_fifo = *rtqueue_init(jackcli_samplerate*4);
  pitch_shift_param.pitch_shift.cyperus_params[0].signal_buffer = (float *)calloc(4096, sizeof(float));  
  pitch_shift_param.pitch_shift.cyperus_params[0].pos = 0;

  pitch_shift_param.pitch_shift.amp = amp;
  pitch_shift_param.pitch_shift.shift = shift;
  pitch_shift_param.pitch_shift.mix = mix;

  ins = dsp_port_in_init("in", 512);
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

float
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

  return outsample;
} /* dsp_pitch_shift */


int
dsp_create_karlsen_lowpass(struct dsp_bus *target_bus, float amp, float freq, float res) {
  dsp_parameter karlsen_lowpass_param;
  struct dsp_port_in *ins;
  struct dsp_port_out *outs;
  karlsen_lowpass_param.type = DSP_KARLSEN_LOWPASS_PARAMETER_ID;
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

  
  ins = dsp_port_in_init("in", 512);
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

float
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
  
  return outsample;
} /* dsp_karlsen_lowpass */





/* ================= FUNCTIONS BELOW NEED TO BE CONVERTED TO USE dsp_* OBJECTS ==================== */

int
dsp_create_pinknoise(void) {
  dsp_parameter pinknoise_param;
  pinknoise_param.type = DSP_PINKNOISE_PARAMETER_ID;
  pinknoise_param.pos = 0;
  pinknoise_param.pinknoise.name = "pinknoise";
  pinknoise_param.pinknoise.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  //dsp_add_module(dsp_pinknoise,pinknoise_param);
  return 0;
} /* dsp_create_pinknoise */

float
dsp_pinknoise(dsp_parameter noise_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  
  outsample = cyperus_pinknoise(&(noise_param.pinknoise.cyperus_params[0]),jack_samplerate,pos);
  
  return outsample;
} /* dsp_pinknoise */
