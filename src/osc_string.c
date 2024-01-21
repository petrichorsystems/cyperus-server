/* osc_string.c
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

#include "osc_string.h"

char *osc_string_int_to_str(int x) {
	char *buffer = malloc(sizeof(char) * 13);
	if(buffer)
		sprintf(buffer, "%d", x);
	return buffer;
} /* osc_string_int_to_str */

char **osc_string_build_osc_str(int *osc_str_len, char *str) {
	/* split string into >=768 byte chunks, OSC on top of UDP will truncate
	 * payloads that are too big. i'm not sure what this limit is, but
	 * 768 bytes feels good to me */
	char *tmp, **osc_str, **osc_str_tmp = NULL;
	int idx, osc_str_idx = 0;
	
	if(strlen(str) > OSC_MAX_STR_LEN) {
		for (idx=0; idx<strlen(str); idx+=OSC_MAX_STR_LEN) {
			if ((strlen(str) - idx) < OSC_MAX_STR_LEN) {
				tmp = malloc(sizeof(char) * (strlen(str) - idx + 2));
				memcpy(tmp, str+idx, strlen(str) - idx + 1);
			} else {
				tmp = malloc(sizeof(char) * OSC_MAX_STR_LEN+1);
				memcpy(tmp, str+idx, OSC_MAX_STR_LEN);
			}
			
			if(!osc_str_idx)
				osc_str = malloc(sizeof(char *));
			else {
				osc_str_tmp = realloc(osc_str, sizeof(char *) * (osc_str_idx + 1));
				if (osc_str_tmp == NULL) {
					printf("could not allocate memory!");
					exit(1);
				}
				osc_str = osc_str_tmp;
			}
			osc_str[osc_str_idx] = tmp;
			osc_str_idx += 1;
		}
		*osc_str_len = osc_str_idx;
	} else {
		osc_str = malloc(sizeof(char *));
		osc_str[0] = malloc(sizeof(char) * (strlen(str) + 1));
		*osc_str_len = 1;
		strcpy(osc_str[0], str);
	}
	return osc_str;
} /* osc_string_build_osc_str */

char *osc_string_build_bus_list(struct dsp_bus *head_bus,
				int root_level,
				const char *separator,
				int single,
				int descendants) {
	struct dsp_bus *temp_bus = head_bus;
	char *single_result_str, *result_str = NULL;
	size_t single_result_str_size, result_str_size = 0;
	struct dsp_bus_port *temp_bus_port = NULL;
	int count_bus_ports;;
	char *bus_ins_str, *bus_outs_str;
	
	if( temp_bus != NULL )
		if(!root_level)
			if(descendants)
				temp_bus = temp_bus->down;
			else
				while(temp_bus->prev != NULL)
					temp_bus = temp_bus->prev;

	/* else */
	/* 	temp_bus = temp_bus->next; */
  
	while(temp_bus != NULL) {
		/* parse inputs */
		count_bus_ports = 0;
		temp_bus_port = temp_bus->ins;
		while(temp_bus_port != NULL) {
			count_bus_ports += 1;
			temp_bus_port = temp_bus_port->next;
		}
		bus_ins_str = osc_string_int_to_str(count_bus_ports);
		
		/* parse_outputs */
		count_bus_ports = 0;
		temp_bus_port = temp_bus->outs;
		while(temp_bus_port != NULL) {
			count_bus_ports += 1;
			temp_bus_port = temp_bus_port->next;
		}
		bus_outs_str = osc_string_int_to_str(count_bus_ports);

		/* construct result string */
		single_result_str_size = strlen(temp_bus->id) + 1 + strlen(temp_bus->name) + 1 +
			strlen(bus_ins_str) + 1 + strlen(bus_outs_str) + 2;
		result_str_size += single_result_str_size;
		single_result_str = malloc(sizeof(char) * single_result_str_size);
		strcpy(single_result_str, temp_bus->id);
		strcat(single_result_str, separator);
		strcat(single_result_str, temp_bus->name);
		strcat(single_result_str, separator);
		strcat(single_result_str, bus_ins_str);
		strcat(single_result_str, separator);
		strcat(single_result_str, bus_outs_str);
		strcat(single_result_str, "\n");
		free(bus_ins_str);
		free(bus_outs_str);
		
		if(result_str == NULL) {
			result_str = malloc(sizeof(char) * single_result_str_size);
			strcpy(result_str, single_result_str);
		} else {
			result_str = realloc(result_str, sizeof(char) * (result_str_size + 1));
			strcat(result_str, single_result_str);
		}
		free(single_result_str);

		if(single)
			break;
		if(descendants)
			temp_bus = temp_bus->down;
		else
			temp_bus = temp_bus->next;
	}

	return result_str;
} /* osc_string_build_bus_list */
			 
