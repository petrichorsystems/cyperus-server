/* dsp_graph_id.h
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

#ifndef DSP_GRAPH_ID_H

#include <stdio.h> //printf
#include <pthread.h>

#include "dsp_types.h"

extern char *dsp_global_graph_id;
extern pthread_mutex_t dsp_graph_id_mutex;

void dsp_graph_id_init();
void dsp_graph_id_rebuild();
char *dsp_graph_id_get();

#endif
