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
#include "libcyperus.h"
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
  struct dsp_operation_sample *temp_sample, *temp_sample_out, *temp_sample_in, *sample_in = NULL;
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
	      sample_in = dsp_operation_sample_init("<bus port in>", 0.0, 1);
	      temp_op_in = dsp_operation_init(temp_connection->id_in);
	    } else {
	      sample_in = dsp_operation_sample_init(temp_result[2], 0.0, 1);
	      temp_op_in = dsp_operation_init(temp_result[1]);
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
      dsp_operation_sample_insert_head(new_op->ins, temp_sample);
    temp_port_in = temp_port_in->next;
  }

  temp_port_out = module->outs;
  while(temp_port_out != NULL) {
    temp_sample = dsp_operation_sample_init(temp_port_out->id, 0.0, 1);
    if(new_op->outs == NULL)
      new_op->outs = temp_sample;
    else
      dsp_operation_sample_insert_head(new_op->outs, temp_sample);

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
  //if( delay->ins->next->summands != NULL )
  //  dsp_param.delay.time = dsp_sum_summands(delay->ins->next->summands) * jack_samplerate;

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

  printf("about to assign delay->dsp_param\n");
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
		 dsp_optimize_sine,
		 sine_param,
		 ins,
		 outs);
  
  return 0;
} /* dsp_create_sine */

void
dsp_edit_sine(struct dsp_module *sine, float freq, float amp, float phase) {
  dsp_parameter dsp_param = sine->dsp_param;

  dsp_param.sine.freq = freq;
  dsp_param.sine.amp = amp;
  dsp_param.sine.phase = phase;
  return;
} /* dsp_edit_sine */

void
dsp_sine(char *bus_path, struct dsp_module *sine, dsp_parameter sine_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  dsp_parameter dsp_param = sine->dsp_param;

  sine->dsp_param.sine.cyperus_params->freq = dsp_param.sine.freq;
  sine->dsp_param.sine.cyperus_params->amp = dsp_param.sine.amp;
  sine->dsp_param.sine.cyperus_params->phase = dsp_param.sine.phase;
  
  outsample = cyperus_sine(sine->dsp_param.sine.cyperus_params,
			   jack_samplerate, pos);

  /* drive audio outputs */
  sine->outs->value = outsample;
  dsp_feed_outputs(bus_path, sine->id, sine->outs);
  
  return;
} /* dsp_sine */

void
dsp_optimize_sine(char *bus_path, struct dsp_module *sine) {

} /* dsp_optimize_sine */


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
		 dsp_optimize_envelope_follower,
		 envelope_follower_param,
		 ins,
		 outs);
  return 0;
} /* dsp_create_envelope_follower */

void
dsp_envelope_follower(char *bus_path, struct dsp_module *envelope_follower, int jack_samplerate, int pos) {

  float insample = 0.0;
  float outsample = 0.0;
  dsp_parameter dsp_param = envelope_follower->dsp_param;

  
  /* sum audio inputs */
  insample = dsp_sum_input(envelope_follower->ins);
  
  envelope_follower->dsp_param.envelope_follower.cyperus_params->in = insample;
  envelope_follower->dsp_param.envelope_follower.cyperus_params->attack = dsp_param.envelope_follower.attack;
  envelope_follower->dsp_param.envelope_follower.cyperus_params->decay = dsp_param.envelope_follower.decay;
  envelope_follower->dsp_param.envelope_follower.cyperus_params->scale = dsp_param.envelope_follower.scale;

  outsample = cyperus_envelope_follower(envelope_follower->dsp_param.envelope_follower.cyperus_params,
			    jack_samplerate, pos);

  
  /* drive audio outputs */
  envelope_follower->outs->value = outsample;
  dsp_feed_outputs(bus_path, envelope_follower->id, envelope_follower->outs);

  return;
} /* dsp_envelope_follower */

void
dsp_optimize_envelope_follower(char *bus_path, struct dsp_module *envelope_follower) {

} /* dsp_optimize_envelope_follower */

void dsp_edit_envelope_follower(struct dsp_module *envelope_follower, float attack, float decay, float scale) {
  int i = 0;
  dsp_parameter dsp_param = envelope_follower->dsp_param;
  
  dsp_param.envelope_follower.attack = attack;
  dsp_param.envelope_follower.decay = decay;
  dsp_param.envelope_follower.scale = scale;
  
} /* dsp_edit_envelope_follower */


/* ================= FUNCTIONS BELOW NEED TO BE CONVERTED TO USE dsp_* OBJECTS ==================== */

int
dsp_create_square(float freq, float amp) {
  dsp_parameter square_param;
  square_param.type = DSP_SQUARE_PARAMETER_ID;
  square_param.pos = 0;
  square_param.square.name = "square";
  square_param.square.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  square_param.square.freq = freq;
  square_param.square.amp = amp;
  //dsp_add_module(dsp_square,square_param);
  return 0;
} /* dsp_create_square */

int
dsp_edit_square(int module_no, float freq, float amp) {
  dsp_voice_parameters[module_no].square.freq = freq;
  dsp_voice_parameters[module_no].square.amp = amp;
  return 0;
} /* dsp_edit_square */

float
dsp_square(dsp_parameter square_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  square_param.square.cyperus_params[0].freq = square_param.square.freq;
  square_param.square.cyperus_params[0].amp = square_param.square.amp;
  
  outsample = cyperus_square(&(square_param.square.cyperus_params[0]),jack_samplerate,pos);
  
  return outsample;
} /* dsp_square */


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


int
dsp_create_butterworth_biquad_lowpass(float cutoff, float res) {
  dsp_parameter filtr_param;
  filtr_param.type = DSP_BUTTERWORTH_BIQUAD_LOWPASS_PARAMETER_ID;
  filtr_param.pos = 0;
  filtr_param.butterworth_biquad_lowpass.name = "butterworth_biquad_lowpass";
  filtr_param.butterworth_biquad_lowpass.cyperus_params = malloc(sizeof(struct cyperus_parameters));
  filtr_param.butterworth_biquad_lowpass.cutoff = cutoff;
  filtr_param.butterworth_biquad_lowpass.res = res;
  //dsp_add_module(dsp_butterworth_biquad_lowpass,filtr_param);
  return 0;
} /* dsp_create_butterworth_biquad_lowpass */

int
dsp_edit_butterworth_biquad_lowpass(int module_no, float cutoff, float res) {
  dsp_voice_parameters[module_no].butterworth_biquad_lowpass.cutoff = cutoff;
  dsp_voice_parameters[module_no].butterworth_biquad_lowpass.res = res;
  return 0;
} /* dsp_edit_butterworth_biquad_lowpass */

float
dsp_butterworth_biquad_lowpass(dsp_parameter filter_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  
  filter_param.butterworth_biquad_lowpass.cyperus_params[0].in = filter_param.in;
  filter_param.butterworth_biquad_lowpass.cyperus_params[0].freq = filter_param.butterworth_biquad_lowpass.cutoff;
  filter_param.butterworth_biquad_lowpass.cyperus_params[0].res = filter_param.butterworth_biquad_lowpass.res;

  outsample = cyperus_butterworth_biquad_lowpass(&(filter_param.butterworth_biquad_lowpass.cyperus_params[0]),jack_samplerate,pos);
  
  return outsample;
} /* dsp_butterworth_biquad_lowpass */

int
dsp_create_vocoder(float freq, float amp) {
  dsp_parameter vocoder_param;
  vocoder_param.type = DSP_VOCODER_PARAMETER_ID;
  vocoder_param.pos = 0;
  vocoder_param.vocoder.name = "phase vocoder";
  vocoder_param.vocoder.cyperus_params = malloc(sizeof(struct cyperus_parameters));

  vocoder_param.vocoder.freq = freq;
  vocoder_param.vocoder.amp = amp;
  vocoder_param.vocoder.cyperus_params[0].freq = freq;
  vocoder_param.vocoder.cyperus_params[0].amp = amp;

  vocoder_param.vocoder.cyperus_params[0].block_fifo = *rtqueue_init(96000);
  
  /* buffer size for fft should be 2x signal buffer size */
  vocoder_param.vocoder.cyperus_params[0].signal_buffer = (float *)calloc(VOCODERFRAMESIZE, sizeof(float));

  vocoder_param.vocoder.cyperus_params[0].signal_buffer_out = (float *)calloc(20*4096, sizeof(float));  

  printf("about to memset fft_buffer");
  
  // memset(vocoder_param.vocoder.cyperus_params[0].fft_buffer, 0, 2*VOCODERFRAMESIZE*sizeof(float));

  printf("about to memset last_phase");
  vocoder_param.vocoder.cyperus_params[0].last_phase = (float *)malloc(VOCODERFRAMESIZE*sizeof(float));
  memset(vocoder_param.vocoder.cyperus_params[0].last_phase, 0, (VOCODERFRAMESIZE)*sizeof(float));

  printf("about to memset sum_phase");
  vocoder_param.vocoder.cyperus_params[0].sum_phase = (float *)malloc(VOCODERFRAMESIZE*sizeof(float));
  memset(vocoder_param.vocoder.cyperus_params[0].sum_phase, 0, (VOCODERFRAMESIZE)*sizeof(float));

  printf("output accumulator");
  vocoder_param.vocoder.cyperus_params[0].output_accumulator = (float *)malloc(VOCODERFRAMESIZE*sizeof(float));
  memset(vocoder_param.vocoder.cyperus_params[0].output_accumulator, 0, VOCODERFRAMESIZE*sizeof(float));

  printf("about to memset signal_buffer_out");
  vocoder_param.vocoder.cyperus_params[0].signal_buffer_out = (float *)malloc(VOCODERFRAMESIZE*sizeof(float));
  memset(vocoder_param.vocoder.cyperus_params[0].signal_buffer_out,0, VOCODERFRAMESIZE*sizeof(float));
  
  vocoder_param.vocoder.cyperus_params[0].pos = 0;
  vocoder_param.vocoder.cyperus_params[0].vocoder_pos = 0;
  //dsp_add_module(dsp_vocoder,vocoder_param);
  
  printf("vocoder module added");
  return 0;
} /* dsp_create_vocoder */

int
dsp_edit_vocoder(int module_no, float freq, float amp) {
  int i = 0;

  /*
  dsp_voice_parameters[module_no].vocoder.cyperus_params[0].pos = 0;
  dsp_voice_parameters[module_no].vocoder.cyperus_params[0].vocoder_pos = 0;
  */

  dsp_voice_parameters[module_no].vocoder.freq = freq;
  dsp_voice_parameters[module_no].vocoder.amp = amp;
  dsp_voice_parameters[module_no].vocoder.cyperus_params[0].freq = freq;
  dsp_voice_parameters[module_no].vocoder.cyperus_params[0].amp = amp;
  
  return 0;
} /* dsp_edit_vocoder */

float
dsp_vocoder(dsp_parameter vocoder_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  
  vocoder_param.vocoder.cyperus_params[0].in = vocoder_param.in;
  vocoder_param.vocoder.cyperus_params[0].freq = vocoder_param.vocoder.freq;
  vocoder_param.vocoder.cyperus_params[0].amp = vocoder_param.vocoder.amp;
  
  outsample = cyperus_vocoder(&(vocoder_param.vocoder.cyperus_params[0]),jack_samplerate,pos);

  return outsample;
} /* dsp_vocoder */

int
dsp_create_pitch_shift(float amp, float shift, float mix) {
  dsp_parameter pitch_shift_param;
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

  //dsp_add_module(dsp_pitch_shift,pitch_shift_param);
  return 0;
} /* dsp_create_pitch_shift */

int
dsp_edit_pitch_shift(int module_no, float amp, float shift, float mix) {
  int i = 0;
  
  dsp_voice_parameters[module_no].pitch_shift.amp = amp;
  dsp_voice_parameters[module_no].pitch_shift.shift = shift;
  dsp_voice_parameters[module_no].pitch_shift.mix = mix;
  
  return 0;
} /* dsp_edit_pitch_shift */

float
dsp_pitch_shift(dsp_parameter pitch_shift_param, int jack_samplerate, int pos) {
  float outsample = 0.0;
  pitch_shift_param.pitch_shift.cyperus_params[0].in = pitch_shift_param.in;
  pitch_shift_param.pitch_shift.cyperus_params[0].amp = pitch_shift_param.pitch_shift.amp;
  pitch_shift_param.pitch_shift.cyperus_params[0].shift = pitch_shift_param.pitch_shift.shift;
  pitch_shift_param.pitch_shift.cyperus_params[0].mix = pitch_shift_param.pitch_shift.mix;
  
  outsample = cyperus_pitch_shift(&(pitch_shift_param.pitch_shift.cyperus_params[0]), jack_samplerate, pos);

  return outsample;
} /* dsp_pitch_shift */
