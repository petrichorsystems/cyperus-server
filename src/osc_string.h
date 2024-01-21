/* osc_string.h
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

#ifndef OSC_STRING_H
#define OSC_STRING_H

/* #include <stdio.h> printf */
/* #include <string.h> memset */
/* #include <stdlib.h> exit(0); */

#include <dirent.h>
#include <limits.h>

#include "dsp_types.h"

#define OSC_MAX_STR_LEN 768

char *osc_string_int_to_str(int x);
char **osc_string_build_osc_str(int *osc_str_len, char *str);
char *osc_string_build_bus_list(struct dsp_bus *head_bus,
				int root_level,
				const char *separator,
				int single,
				int descendants);

#endif
