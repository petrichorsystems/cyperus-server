/* dsp_graph_id.c
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

#include "dsp_graph_id.h"

char *dsp_global_graph_id;
pthread_mutex_t dsp_graph_id_mutex;

void
dsp_graph_id_init() {
  dsp_global_graph_id = dsp_generate_object_id();
} /* dsp_graph_id_init */

void
dsp_graph_id_rebuild() {
  char *temp_str = NULL;
  temp_str = dsp_generate_object_id();  
  pthread_mutex_lock(&dsp_graph_id_mutex);
  strcpy(dsp_global_graph_id, temp_str);
  pthread_mutex_unlock(&dsp_graph_id_mutex);
  free(temp_str);
} /* dsp_graph_id_rebuild */

char*
dsp_graph_id_get() {
  return dsp_global_graph_id;

} /* dsp_graph_id_get */
