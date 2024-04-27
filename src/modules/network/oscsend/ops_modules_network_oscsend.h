/* ops_modules_network_oscsend.h
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

#ifndef OPS_MODULES_NETWORK_OSCSEND_H
#define OPS_MODULES_NETWORK_OSCSEND_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)

#include "../../../dsp.h"
#include "../../../dsp_ops.h"
#include "../../../dsp_types.h"
#include "../../../dsp_math.h"
#include "../../../osc.h"
#include "../../../osc_handlers.h"

#include "../../../jackcli.h"

int
dsp_create_network_oscsend(struct dsp_bus *target_bus,
			   char *hostname_ip,
			   int port,			    
			   char *osc_path,
			   float freq_div);
void
dsp_network_oscsend(struct dsp_operation *network_oscsend, int jack_samplerate);

void
dsp_edit_network_oscsend(struct dsp_module *network_oscsend,
			 char *hostname_ip,
			 int port,			    
			 char *osc_path,
			 float freq_div);
#endif
