 /* ops_modules_network_oscsend.c
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

Copyright 2021 murray foster */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "ops_modules_network_oscsend.h"

int
dsp_create_network_oscsend(struct dsp_bus *target_bus,
			    char *hostname_ip,
			    int port,			    
			    char *osc_path,
			    float freq_div) {

	printf("ops_modules_network_oscsend.c::dsp_create_network_oscsend()\n");
	
	dsp_parameter params;
	struct dsp_port_in *ins;
	struct dsp_port_out *outs;

	params.name = "network_oscsend";  

	/* signal input */
	params.in = malloc(sizeof(float) * dsp_global_period);
  
	params.parameters = malloc(sizeof(dsp_module_parameters_t));

	params.parameters->char_type = malloc(sizeof(char*) * 2);
	params.parameters->int32_type = malloc(sizeof(int) * 2);
	params.parameters->float32_type = malloc(sizeof(float) * 2);

	/* user-facing parameter allocation */
	params.parameters->char_type[0] = malloc(sizeof(char) * (strlen(hostname_ip) + 1));
	params.parameters->char_type[1] = malloc(sizeof(char) * (strlen(osc_path) + 1));

	/* internal parameter assignment */
	params.parameters->int32_type[1] = 1;  /* samplerate counter */
	params.parameters->float32_type[1] = 0.0f; /* last output sample */

	/* osc listener parameters */
	strcpy(params.parameters->char_type[0], hostname_ip);
	strcpy(params.parameters->char_type[1], osc_path);
	params.parameters->int32_type[0] = port;
	params.parameters->float32_type[0] = freq_div;
	
	ins = dsp_port_in_init("in", 512);
	outs = dsp_port_out_init("out", 1);

	
	dsp_add_module(target_bus,
		       "network_oscsend",
		       dsp_network_oscsend,
		       NULL,
		       dsp_optimize_module,
		       params,
		       ins,
		       outs);

	return 0;
} /* dsp_create_network_oscsend */

void
dsp_edit_network_oscsend(struct dsp_module *network_oscsend,
			 char *hostname_ip,
			 int port,			    
			 char *osc_path,
			 float freq_div) {
	printf("ops_modules_network_oscsend.c::dsp_edit_network_oscsend()\n");	
	network_oscsend->dsp_param.parameters->char_type[0] = realloc(network_oscsend->dsp_param.parameters->char_type[0], strlen(hostname_ip) + 1);
	strcpy(network_oscsend->dsp_param.parameters->char_type[0], hostname_ip);

	network_oscsend->dsp_param.parameters->int32_type[0] = port;
	network_oscsend->dsp_param.parameters->float32_type[0] = freq_div;
	
	network_oscsend->dsp_param.parameters->char_type[1] = realloc(network_oscsend->dsp_param.parameters->char_type[1], strlen(osc_path) + 1);
	strcpy(network_oscsend->dsp_param.parameters->char_type[1], osc_path);		
} /* dsp_edit_network_oscsend */

void
dsp_network_oscsend(struct dsp_operation *network_oscsend, int jack_samplerate) {
	printf("ops_modules_network_oscsend.c::dsp_network_oscsend()\n");	
	char *hostname_ip, *osc_path, *osc_port;
	lo_address lo_addr_send;
	int port, sample_count, i, port_len;
	float freq_div;

	hostname_ip = network_oscsend->module->dsp_param.parameters->char_type[0];
	port = network_oscsend->module->dsp_param.parameters->int32_type[0];
	osc_path = network_oscsend->module->dsp_param.parameters->char_type[1];
	freq_div = network_oscsend->module->dsp_param.parameters->float32_type[0];	

	sample_count = network_oscsend->module->dsp_param.parameters->int32_type[2];

	port_len = snprintf(NULL, 0,"%d",port);
	osc_port = malloc(sizeof(char) * port_len);
	snprintf(osc_port, port_len+1, "%d", port);
	lo_addr_send = lo_address_new(hostname_ip, (const char*)osc_port);

	if( network_oscsend->ins->summands != NULL ) {  
		dsp_sum_summands(network_oscsend->module->dsp_param.in, network_oscsend->ins->summands);
	} else {
		printf("is NULL\n");
	}

	i=0;
	while(i<dsp_global_period) {		
		if( ((int)(jack_samplerate / freq_div) < sample_count)) {
			printf("jack_samplerate / freq_div: %d\n", (int)(jack_samplerate / freq_div));
			printf("sample_count: %d\n", sample_count);
			printf("sending osc...\n");
			printf("osc_path: %s\n", osc_path);
			printf("port: %d\n", port);			
			printf("osc_port: %s\n", osc_port);
			lo_send(lo_addr_send,
				osc_path,
				"f",
				network_oscsend->module->dsp_param.in[i]);
		}
		if((int)(jack_samplerate / freq_div) < sample_count) {
			sample_count=1;
		} else {
			sample_count++;
		}
		i++;
	}
	network_oscsend->module->dsp_param.parameters->int32_type[2] = sample_count;

	free(osc_port);
	free(lo_addr_send);  
} /* dsp_network_oscsend */

