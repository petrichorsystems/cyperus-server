/* osc_modules_control_osc_utils_spigot.h
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

#ifndef OSC_MODULES_UTILS_SPIGOT_H
#define OSC_MODULES_UTILS_SPIGOT_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);

/* #include "../../../dsp_math.h" */
#include "../../../dsp.h"
#include "../../../dsp_types.h"
/* #include "../../../dsp_ops.h" */
/* #include "../../../jackcli.h" */
/* #include "../../../osc.h" */

#include "ops_modules_utils_spigot.h"

int osc_add_modules_utils_spigot_handler(const char *path, const char *types, lo_arg ** argv,
                                             int argc, void *data, void *user_data);
int osc_edit_modules_utils_spigot_handler(const char *path, const char *types, lo_arg ** argv,
                                              int argc, void *data, void *user_data);

#endif
