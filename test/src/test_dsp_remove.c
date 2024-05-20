/* test_dsp.c
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

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);

#include "../../src/dsp.h"
#include "../../src/dsp_types.h"
#include "../../src/dsp_ops.h"

char*
strconcat(char *str_prefix, char *str_suffix) {
	char *full_str;
	full_str = (char*)malloc(sizeof(char)*(strlen(str_suffix)+strlen(str_prefix)+1));
	snprintf(full_str, strlen(str_suffix)+strlen(str_prefix)+1, "%s%s", str_prefix, str_suffix);
	return full_str;
}


int
main(void) {

	dsp_setup(256, 4, 4);
	dsp_graph_id_init();
	

	exit(0);
}

