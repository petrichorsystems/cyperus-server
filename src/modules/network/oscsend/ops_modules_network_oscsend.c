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

#include <lo/lo.h>
#include <pthread.h>

#include "../../../dsp.h"
#include "../../../osc.h"

#include "params_modules_network_oscsend.h"
#include "ops_modules_network_oscsend.h"

lo_address
_build_new_lo_address(char *hostname_ip, int port) {
	char *osc_port;
	int port_len;
	lo_address lo_addr = malloc(sizeof(lo_address));

	port_len = snprintf(NULL, 0,"%d",port);
	osc_port = malloc(sizeof(char) * port_len);
	snprintf(osc_port, port_len+1, "%d", port);
	lo_addr = lo_address_new(hostname_ip, (const char*)osc_port);

	free(osc_port);
	
	return lo_addr;
} /* _build_new_lo_address */

int
dsp_create_network_oscsend(struct dsp_bus *target_bus,
			    char *hostname_ip,
			    int port,			    
			    char *osc_path,
			    float freq_div) {	
	dsp_parameter params;
	struct dsp_port_in *ins;
	
	params.name = "network_oscsend";  

	/* signal input */
	params.in = malloc(sizeof(float) * dsp_global_period);
  
	params.parameters = malloc(sizeof(dsp_module_parameters_t));

	params.parameters->char_type = malloc(sizeof(char*) * 2);
	params.parameters->int32_type = malloc(sizeof(int) * 2);
	params.parameters->float32_type = malloc(sizeof(float) * 1);

	params.parameters->lo_address_type = malloc(sizeof(lo_address) * 3);
	params.parameters->atomic_flag_type = malloc(sizeof(atomic_flag) * 2);
	
	/* user-facing parameter allocation */
	params.parameters->char_type[PARAM_USER_HOSTNAME_IP] = malloc(sizeof(char) * (strlen(hostname_ip) + 1));
	strcpy(params.parameters->char_type[PARAM_USER_HOSTNAME_IP], hostname_ip);	
	params.parameters->char_type[PARAM_USER_OSC_PATH] = malloc(sizeof(char) * (strlen(osc_path) + 1));
	strcpy(params.parameters->char_type[PARAM_USER_OSC_PATH], osc_path);	
	params.parameters->int32_type[PARAM_USER_PORT] = port;
	params.parameters->float32_type[PARAM_USER_FREQ_DIV] = freq_div;	
	
	/* internal parameter assignment */
	params.parameters->int32_type[PARAM_INTERNAL_SAMPLERATE_COUNTER] = 1;  /* samplerate counter */
	params.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND] = _build_new_lo_address(hostname_ip, port);
	params.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED] = NULL;
	params.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_EXPIRED] = NULL;
	atomic_flag_clear(params.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED_STATE]);
	atomic_flag_clear(params.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_GARBAGE_STATE]);	
	
	ins = dsp_port_in_init("in");
	
	dsp_add_module(target_bus,
		       "network_oscsend",
		       dsp_network_oscsend,
		       dsp_destroy_network_oscsend,
		       NULL,
		       dsp_optimize_module,
		       params,
		       ins,
		       NULL);

	return 0;
} /* dsp_create_network_oscsend */

int
dsp_destroy_network_oscsend(struct dsp_module *target_module) {
	free(target_module->dsp_param.parameters->char_type[PARAM_USER_HOSTNAME_IP]);
	free(target_module->dsp_param.parameters->char_type[PARAM_USER_OSC_PATH]);
	free(target_module->dsp_param.parameters->char_type);	

	free(target_module->dsp_param.parameters->int32_type);	
	free(target_module->dsp_param.parameters->float32_type);
	
	lo_address_free(target_module->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND]);	
	free(target_module->dsp_param.parameters->lo_address_type);

	free(target_module->dsp_param.parameters);
	free(target_module->dsp_param.out);
	free(target_module->dsp_param.in);

	return 0;	
} /* dsp_destroy_network_oscsend */

void
dsp_edit_network_oscsend(struct dsp_module *network_oscsend,
			 char *hostname_ip,
			 int port,			    
			 char *osc_path,
			 float freq_div) {
	network_oscsend->dsp_param.parameters->char_type[PARAM_USER_HOSTNAME_IP] = realloc(network_oscsend->dsp_param.parameters->char_type[PARAM_USER_HOSTNAME_IP], strlen(hostname_ip) + 1);
	strcpy(network_oscsend->dsp_param.parameters->char_type[PARAM_USER_HOSTNAME_IP], hostname_ip);

	network_oscsend->dsp_param.parameters->char_type[PARAM_USER_OSC_PATH] = realloc(network_oscsend->dsp_param.parameters->char_type[PARAM_USER_OSC_PATH], strlen(osc_path) + 1);
	strcpy(network_oscsend->dsp_param.parameters->char_type[PARAM_USER_OSC_PATH], osc_path);	
	
	network_oscsend->dsp_param.parameters->int32_type[PARAM_USER_PORT] = port;
	network_oscsend->dsp_param.parameters->float32_type[PARAM_USER_FREQ_DIV] = freq_div;

	network_oscsend->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED] = _build_new_lo_address(hostname_ip, port);
	atomic_flag_test_and_set(&network_oscsend->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED_STATE]);

	if( atomic_flag_test_and_set(&network_oscsend->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_GARBAGE_STATE]) &&
	    network_oscsend->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_EXPIRED] != NULL) {
		lo_address_free(network_oscsend->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_EXPIRED]);
		network_oscsend->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_EXPIRED] = NULL;
		atomic_flag_clear(&network_oscsend->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_GARBAGE_STATE]);
	} else {
		atomic_flag_clear(&network_oscsend->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_GARBAGE_STATE]);
	}
	
} /* dsp_edit_network_oscsend */

void
dsp_network_oscsend(struct dsp_operation *network_oscsend, int jack_samplerate) {
	char *osc_path;
	int sample_count, i;
	float freq_div;

	if( atomic_flag_test_and_set(&network_oscsend->module->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED_STATE]) ) {
		network_oscsend->module->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_EXPIRED] = network_oscsend->module->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND];		
		network_oscsend->module->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND] = network_oscsend->module->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED];
		atomic_flag_clear(&network_oscsend->module->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED_STATE]);
		atomic_flag_test_and_set(&network_oscsend->module->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_GARBAGE_STATE]);
	} else {
		atomic_flag_clear(&network_oscsend->module->dsp_param.parameters->atomic_flag_type[PARAM_INTERNAL_LO_ADDR_SEND_STAGED_STATE]);
	}
	
	osc_path = network_oscsend->module->dsp_param.parameters->char_type[PARAM_USER_OSC_PATH];	
	freq_div = network_oscsend->module->dsp_param.parameters->float32_type[PARAM_USER_FREQ_DIV];
	sample_count = network_oscsend->module->dsp_param.parameters->int32_type[PARAM_INTERNAL_SAMPLERATE_COUNTER];

	if( network_oscsend->ins->summands != NULL ) {  
		dsp_sum_summands(network_oscsend->module->dsp_param.in, network_oscsend->ins->summands);
	}

	i=0;
	while(i<dsp_global_period) {		
		if( ((int)(jack_samplerate / freq_div) < sample_count)) {
			lo_send(network_oscsend->module->dsp_param.parameters->lo_address_type[PARAM_INTERNAL_LO_ADDR_SEND],
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
	network_oscsend->module->dsp_param.parameters->int32_type[PARAM_INTERNAL_SAMPLERATE_COUNTER] = sample_count;

} /* dsp_network_oscsend */

