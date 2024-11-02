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

#include "osc.h"

struct osc_global_t osc_global;

/* osc_handler_user_defined_t *global_osc_handlers_user_defined; */
/* pthread_mutex_t global_osc_handlers_user_defined_lock; */

/* osc_handler_user_defined_t* osc_handler_user_defined_init(char *osc_path, char *type_str, int num_ports, char **ports) { */
/*   int idx; */
/*   osc_handler_user_defined_t *new_handler = (osc_handler_user_defined_t *)malloc(sizeof(osc_handler_user_defined_t)); */

/*   new_handler->osc_path = malloc(sizeof(char) * (strlen(osc_path) + 1)); */
/*   snprintf(new_handler->osc_path, strlen(osc_path)+1, "%s", osc_path); */
  
/*   new_handler->type_str = malloc(sizeof(char) * (strlen(type_str) + 1)); */
/*   snprintf(new_handler->type_str, strlen(type_str)+1, "%s", type_str); */

/*   new_handler->ports = malloc(sizeof(char*) * num_ports); */
/*   /\* populate ports *\/ */
/*   for(idx=0; idx<num_ports; idx++) { */
/*     new_handler->ports[idx] = malloc(sizeof(char) * (strlen(ports[idx]) + 1)); */
/*     snprintf(new_handler->ports[idx], strlen(ports[idx])+1, "%s", ports[idx]); */
/*   } */
/*   return new_handler; */
/* } /\* osc_handler_user_defined_init *\/ */

/* void osc_handler_user_defined_insert_tail(osc_handler_user_defined_t *head_handler, osc_handler_user_defined_t *new_handler) { */
/*   osc_handler_user_defined_t *temp_handler = head_handler; */
/*   if(temp_handler == NULL) { */
/*     head_handler = new_handler; */
/*     return; */
/*   } */
/*   while(temp_handler->next != NULL) { */
/*     temp_handler = temp_handler->next; */
/*   } */
/*   temp_handler->next = new_handler; */
/*   new_handler->prev = temp_handler; */
/* } /\* osc_handler_user_defined_insert_tail *\/ */

/* void osc_execute_handler_parameter_assignment(osc_handler_user_defined_t *handler, lo_arg** argv) { */
/*   int idx, error_code; */
/*   char *request_id, *temp_port_path; */

/*   struct dsp_bus *target_bus = NULL; */
/*   struct dsp_module *temp_module, *target_module = NULL; */
/*   struct dsp_port_in *temp_port_in, *target_port_in = NULL; */
  
/*   request_id = (char *)argv[0]; */

/*   for(idx=0; idx<handler->num_ports; idx++) { */
/*     temp_port_path = handler->ports[idx]; */

/*     /\* get temp_port_in or temp_port_path *\/ */
/*     if(strstr(temp_port_path, ":") != NULL) { */
/*       /\* print message, error out *\/       */
/*     } else if((strstr(temp_port_path, "}") != NULL) || */
/*               (strstr(temp_port_path, "{") != NULL)) { */
/*       /\* print message, error out *\/ */
/*     } else if(strstr(temp_port_path, ">") != NULL) { */
/*       /\* print message, error out *\/ */
/*     } else if(strstr(temp_port_path, "<") != NULL) { */
/*       target_port_in = parse_module_port_in(temp_port_path); */
/*     } else { */
/*       /\* throw message and error out, */
/*          we shouldn't get here *\/ */
/*     } */
/*     /\* assign parameter value from osc message *\/ */
/*     switch(handler->type_str[idx]) { */
/*     case 'f': */
/*       *(temp_port_in->parameter_value_ptr) = argv[1 + idx]->f; */
/*       break; */
/*     case 'i': */
/*       *(temp_port_in->parameter_value_ptr) = (float)argv[1 + idx]->i;  */
/*       break; */
/*     default: */
/*       printf("osc.c::osc_execute_handler_parameter_assignment():ERROR, we should never get here. exiting..\n"); */
/*       exit(1); */
/*       break; */
/*     } */
/*   } */

/*   /\* build and send response *\/ */
/*   lo_message msg = lo_message_new(); */
/*   lo_message_add_string(msg, request_id); */
/*   lo_message_add_int32(msg, 0); */
/*   for(int idx=0; idx<handler->num_ports; idx++) { */
/*     /\* assign parameter value from osc message *\/ */
/*     switch(handler->type_str[idx]) { */
/*     case 'f': */
/*       lo_message_add_float(msg, argv[1 + idx]->f); */
/*       break; */
/*     case 'i': */
/*       lo_message_add_int32(msg, argv[1 + idx]->i); */
/*       break; */
/*     default: */
/*       printf("osc.c::osc_execute_handler_parameter_assignment():ERROR, we should never get here. exiting..\n"); */
/*       exit(1); */
/*       break; */
/*     } */
/*   } */
/*   lo_address lo_addr_send = lo_address_new((const char*)send_host_out, (const char*)send_port_out); */
/*   lo_send_message(lo_addr_send, "/cyperus/add/module/osc/parameter_assignment", msg); */
/*   lo_message_free(msg); */
/*   free(lo_addr_send); */
/* } /\* osc_execute_handler_parameter_assignment *\/ */


/* see osc.h, called by macro defined by osc_send_broadcast() */
int _osc_send_broadcast(const char *path, const char *types, ...) {
	uint16_t i = 0;
	va_list ap;
	const char *file = "";
	int line = 0;
	for(i=0; i<osc_global.client_count; i++) {
		lo_address lo_addr_send = lo_address_new(
			(const char*)(osc_global.client_addrs[i].send_host_out),
			(const char*)(osc_global.client_addrs[i].send_port_out)
			);
		lo_message lo_msg = lo_message_new();
		va_start(ap, types);		
		lo_message_add_varargs(lo_msg, types, ap);
		va_end(ap);
		lo_send_message(lo_addr_send, path, lo_msg);
	
		free(lo_addr_send);

	}
	return 0;
} /* osc_broadcast_msg */

int osc_change_address(char *request_id, char *new_host_out, char *new_port_out) {
	bool multipart;
	
	free(osc_global.client_addrs[0].send_host_out);
	free(osc_global.client_addrs[0].send_port_out);

	osc_global.client_addrs[0].send_host_out = malloc(sizeof(char) * strlen(new_host_out) + 1);
	strcpy(osc_global.client_addrs[0].send_host_out, new_host_out);

	osc_global.client_addrs[0].send_port_out = malloc(sizeof(char) * strlen(new_port_out) + 1);
	strcpy(osc_global.client_addrs[0].send_port_out, new_port_out);

	multipart = false;
	lo_address lo_addr_send = lo_address_new((const char*)new_host_out, (const char*)new_port_out);
	lo_send(lo_addr_send,"/cyperus/address", "siiss", request_id, 0, multipart, new_host_out, new_port_out);
	free(lo_addr_send);
	printf("changed osc server and port to: %s:%s\n", new_host_out, new_port_out);
	return 0;
} /* osc_change_address */

void
osc_callback_timer_callback(int signum) {
	lo_address lo_addr_send = lo_address_new(
		(const char*)(osc_global.client_addrs[0].send_host_out),
		(const char*)(osc_global.client_addrs[0].send_port_out)
		);
	struct dsp_operation *temp_op = NULL;  

	lo_send(lo_addr_send,"/cyperus/dsp/load", "f", dsp_global.cpu_load);
  
	temp_op = dsp_global.operation_head;
	while(temp_op != NULL) {
		/* execute appropriate listener function */
		if( temp_op->module != NULL )
			if( temp_op->module->dsp_osc_listener_function != NULL )
				temp_op->module->dsp_osc_listener_function(temp_op, jackcli_samplerate);
		temp_op = temp_op->next;
	}
} /* osc_callback_timer_callback */

void *
osc_callback_timer_thread(void *arg) {
	struct itimerval callback_timer;
	struct itimerval callback_timer_old;
	unsigned short fps = 30;
	long sample_duration_usec = (long)(1000 * 1000 / fps);

	callback_timer.it_value.tv_sec = 1;
	callback_timer.it_value.tv_usec = 0;
	callback_timer.it_interval.tv_sec = 0;
	callback_timer.it_interval.tv_usec = sample_duration_usec;
  
	setitimer(ITIMER_REAL, &callback_timer, &callback_timer_old);
	signal(SIGALRM, osc_callback_timer_callback);

	while(1) {
		sleep(1);
	}
} /* osc_callback_timer_thread */

int
osc_callback_timer_setup() {
	pthread_t callback_timer_thread_id;
	pthread_create(&callback_timer_thread_id, NULL, osc_callback_timer_thread, NULL);
	pthread_detach(callback_timer_thread_id);
	return 0;
} /* osc_callback_timer_setup */


int osc_setup(char *osc_port_in, char *osc_port_out, char *addr_out) {
	/* global_osc_handlers_user_defined = NULL; */
	
	/* if(pthread_mutex_init(&global_osc_handlers_user_defined_lock, NULL) != 0) { */
	/*     printf("\n mutex init failed\n"); */
	/*     return 1; */
	/* } */

	struct osc_client_addr_t client_addr;
	
	osc_global.client_addrs = malloc(sizeof(struct osc_client_addr_t));
	
	osc_global.client_addrs[0].send_host_out = malloc(sizeof(char) * 10);
	strcpy(osc_global.client_addrs[0].send_host_out, "127.0.0.1");

	osc_global.client_addrs[0].send_port_out = malloc(sizeof(char) * 6);
	strcpy(osc_global.client_addrs[0].send_port_out, osc_port_out);

	osc_global.client_addrs[0].listener_enable = true;
	
	osc_global.client_count = 1;
	
	osc_global.lo_thread = lo_server_thread_new(osc_port_in, osc_error);
	lo_server_thread_add_method(osc_global.lo_thread, NULL, NULL, cyperus_osc_handler, NULL);

	lo_server_thread_start(osc_global.lo_thread);

	osc_callback_timer_setup();
} /* osc_setup */
