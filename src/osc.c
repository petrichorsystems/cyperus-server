/* osc.c
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

#include <sys/time.h>

#include "osc.h"

struct osc_global_t osc_global;

/* see osc.h, called by macro defined by osc_send_broadcast() */
int _osc_send_broadcast(const char *path, const char *types, ...) {
	va_list ap;
	
	struct osc_client_addr_t *temp_client_addr = osc_global.client_addr; 
	while( temp_client_addr != NULL ) {
		lo_address lo_addr_send = lo_address_new(
			(const char*)(temp_client_addr->send_host_out),
			(const char*)(temp_client_addr->send_port_out)
			);
		lo_message lo_msg = lo_message_new();
		va_start(ap, types);		
		lo_message_add_varargs(lo_msg, types, ap);
		va_end(ap);
		lo_send_message(lo_addr_send, path, lo_msg);

		lo_message_free(lo_msg);
		lo_address_free(lo_addr_send);
		
		temp_client_addr = temp_client_addr->next;
	}
	
	return 0;
} /* osc_broadcast_msg */

int osc_change_address(char *request_id, char *new_host_out, char *new_port_out) {
	bool multipart;

	struct osc_client_addr_t *temp_client_addr = osc_global.client_addr; 	
	
	free(temp_client_addr->send_host_out);
	free(temp_client_addr->send_port_out);

	temp_client_addr->send_host_out = malloc(sizeof(char) * strlen(new_host_out) + 1);
	strcpy(temp_client_addr->send_host_out, new_host_out);

	temp_client_addr->send_port_out = malloc(sizeof(char) * strlen(new_port_out) + 1);
	strcpy(temp_client_addr->send_port_out, new_port_out);

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)new_host_out, (const char*)new_port_out);
	lo_send(lo_addr_send,"/cyperus/address", "siiss", request_id, 0, multipart, new_host_out, new_port_out);
	free(lo_addr_send);
	printf("changed osc server and port to: %s:%s\n", new_host_out, new_port_out);
	return 0;
} /* osc_change_address */

int osc_add_client(char *new_host_out, char *new_port_out, bool listener_enable) {
	struct osc_client_addr_t *last_client_addr = NULL, *new_client_addr = NULL;
	struct osc_client_addr_t *temp_client_addr = osc_global.client_addr;

	pthread_mutex_lock(&osc_global.client_addr_update_mutex);
	
	while( temp_client_addr != NULL ) {
		if( strcmp(new_host_out, temp_client_addr->send_host_out) == 0 &&
		    strcmp(new_port_out, temp_client_addr->send_port_out) == 0 ) {
			new_client_addr = NULL;
			return E_OSC_CLIENT_EXISTS;
		}
		if( temp_client_addr->next == NULL )
			last_client_addr = temp_client_addr;
		temp_client_addr = temp_client_addr->next;
	}
	
	new_client_addr = malloc(sizeof(struct osc_client_addr_t));
	new_client_addr->next = NULL;

	new_client_addr->send_host_out = malloc(sizeof(char) * strlen(new_host_out) + 1);
	strcpy(new_client_addr->send_host_out, new_host_out);
	new_client_addr->send_port_out = malloc(sizeof(char) * strlen(new_port_out) + 1);
	strcpy(new_client_addr->send_port_out, new_port_out);
	new_client_addr->listener_enable = listener_enable;
	last_client_addr->next = new_client_addr;

	pthread_mutex_unlock(&osc_global.client_addr_update_mutex);
	
	return 0;
} /* osc_add_client */

void
osc_callback_timer_callback(union sigval sv) {
	
	struct osc_client_addr_t *temp_client_addr = osc_global.client_addr;	
	while( temp_client_addr != NULL ) {
		if( temp_client_addr->listener_enable ) {
			lo_address lo_addr_send = lo_address_new(
				(const char*)(temp_client_addr->send_host_out),
				(const char*)(temp_client_addr->send_port_out)
				);
			struct dsp_operation *temp_op = NULL;  

			lo_send(lo_addr_send,"/cyperus/dsp/load", "f", dsp_global.cpu_load);

			pthread_mutex_lock(&dsp_global.graph_state_mutex);
			if( pthread_spin_trylock(&dsp_global.optimization_spinlock) == 0 ) {			
				temp_op = dsp_global.operation_head;
				while(temp_op != NULL) {
					/* execute appropriate listener function */
					if( temp_op->module != NULL )
						if( temp_op->module->dsp_osc_listener_function != NULL )
							temp_op->module->dsp_osc_listener_function(temp_op, jackcli_samplerate);
					temp_op = temp_op->next;
				}
				
				lo_address_free(lo_addr_send);
				pthread_spin_unlock(&dsp_global.optimization_spinlock);
			}
			pthread_mutex_unlock(&dsp_global.graph_state_mutex);
		}
		temp_client_addr = temp_client_addr->next;
	}	
} /* osc_callback_timer_callback */

void *
osc_callback_timer_thread() {
    struct sigevent sev;
    timer_t timer_id;
    struct itimerspec its;
    
    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_notify_function = osc_callback_timer_callback;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = &timer_id;
    
    timer_create(CLOCK_MONOTONIC, &sev, &timer_id);
    
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = (1000000000L / 60); /* 30fps */
    
    timer_settime(timer_id, 0, &its, NULL);
    
    while(1) { sleep(1); }
} /* osc_callback_timer_thread */

int
osc_callback_timer_setup() {
	pthread_t callback_timer_thread_id;
	pthread_create(&callback_timer_thread_id, NULL, osc_callback_timer_thread, NULL);
	pthread_detach(callback_timer_thread_id);
	return 0;
} /* osc_callback_timer_setup */


int osc_setup(char *osc_port_in, char *osc_port_out) {
	/* global_osc_handlers_user_defined = NULL; */
	
	/* if(pthread_mutex_init(&global_osc_handlers_user_defined_lock, NULL) != 0) { */
	/*     printf("\n mutex init failed\n"); */
	/*     return 1; */
	/* } */

	struct osc_client_addr_t *client_addr = malloc(sizeof(struct osc_client_addr_t));
	client_addr->send_host_out = NULL;
	client_addr->send_port_out = NULL;
	client_addr->listener_enable = false;
	client_addr->next = NULL;

	pthread_mutex_init(&osc_global.client_addr_update_mutex, NULL);
	
	client_addr->send_host_out = malloc(sizeof(char) * 10);
	strcpy(client_addr->send_host_out, "127.0.0.1");

	client_addr->send_port_out = malloc(sizeof(char) * 6);
	strcpy(client_addr->send_port_out, osc_port_out);

	client_addr->listener_enable = true;

	osc_global.client_addr = client_addr;
	
	osc_global.lo_thread = lo_server_thread_new(osc_port_in, osc_error);
	lo_server_thread_add_method(osc_global.lo_thread, NULL, NULL, cyperus_osc_handler, NULL);

	lo_server_thread_start(osc_global.lo_thread);

	osc_callback_timer_setup();

	return 0;
} /* osc_setup */
