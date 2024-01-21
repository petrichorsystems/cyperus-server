/* osc_handlers.h
This file is a part of 'cyperus'
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
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

#ifndef OSC_HANDLERS_H
#define OSC_HANDLERS_H

/* #include <stdio.h> printf */
/* #include <string.h> memset */
/* #include <stdlib.h> exit(0); */

#include <dirent.h>
#include <limits.h>

#include "dsp.h"
#include "dsp_types.h"
#include "jackcli.h"
#include "osc.h"
#include "osc_string.h"

void osc_error(int num, const char *msg, const char *path);

int osc_address_handler(const char *path, const char *types, lo_arg ** argv,
			int argc, void *data, void *user_data);

int osc_list_main_handler(const char *path, const char *types, lo_arg ** argv,
			  int argc, void *data, void *user_data);

int osc_add_bus_handler(const char *path, const char *types, lo_arg **argv,
			int argc, void *data, void *user_data);
int osc_list_bus_handler(const char *path, const char *types, lo_arg **argv,
			int argc, void *data, void *user_data);

int osc_list_bus_port_handler(const char *path, const char *types, lo_arg **argv,
			       int argc, void *data, void *user_data);

int osc_add_connection_handler(const char *path, const char *types, lo_arg **argv,
			       int argc, void *data, void *user_data);

int osc_remove_connection_handler(const char *path, const char *types, lo_arg **argv,
			       int argc, void *data, void *user_data);

int osc_remove_module_handler(const char *path, const char *types, lo_arg ** argv,
			      int argc, void *data, void *user_data);

int osc_list_modules_handler(const char *path, const char *types, lo_arg ** argv,
			     int argc, void *data, void *user_data);


int osc_get_filesystem_cwd_handler(const char *path, const char *types, lo_arg ** argv,
				   int argc, void *data, void *user_data);

int osc_list_filesystem_path_handler(const char *path, const char *types, lo_arg ** argv,
				     int argc, void *data, void *user_data);

int osc_write_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				      int argc, void *data, void *user_data);

int osc_append_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				       int argc, void *data, void *user_data);

int osc_read_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				     int argc, void *data, void *user_data);

int osc_remove_filesystem_file_handler(const char *path, const char *types, lo_arg ** argv,
				       int argc, void *data, void *user_data);

int osc_list_module_port_handler(const char *path, const char *types, lo_arg ** argv,
				 int argc, void *data, void *user_data);

int osc_list_module_port_handler(const char *path, const char *types, lo_arg ** argv,
				 int argc, void *data, void *user_data);


int osc_add_module_block_processor_handler(const char *path, const char *types, lo_arg ** argv,
					   int argc, void *data, void *user_data);

int osc_add_module_motion_osc_parameter_assignment_handler(const char *path, const char *types, lo_arg ** argv, int argc, void *data, void *user_data);
int osc_edit_module_motion_osc_parameter_assignment_handler(const char *path, const char *types, lo_arg ** argv, int argc, void *data, void *user_data);

int osc_add_module_highpass_handler(const char *path, const char *types, lo_arg ** argv,
                                    int argc, void *data, void *user_data);

int osc_edit_module_highpass_handler(const char *path, const char *types, lo_arg ** argv,
                                     int argc, void *data, void *user_data);


int osc_add_module_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
                                       int argc, void *data, void *user_data);
int
osc_edit_module_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
                                    int argc, void *data, void *user_data);


int osc_add_module_karlsen_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
                                       int argc, void *data, void *user_data);
int
osc_edit_module_karlsen_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
                                    int argc, void *data, void *user_data);


int cyperus_osc_handler(const char *path, const char *types, lo_arg ** argv,
                        int argc, void *data, void *user_data);


/* ================= FUNCTIONS BELOW NEED TO BE CONVERTED TO USE dsp_* OBJECTS ==================== */

int osc_add_pinknoise_handler(const char *path, const char *types, lo_arg ** argv,
			      int argc, void *data, void *user_data);

#endif
