/* cyperus.h
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

Copyright 2018 murray foster */

#ifndef CYPERUS_H
#define CYPERUS_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <unistd.h>

#include "dsp_types.h"
#include "rtqueue.h"
#include "jackcli.h"
#include "dsp.h"
#include "dsp_ops.h"
#include "osc_handlers.h"
#include "osc.h"

extern int jack_sr;
extern rtqueue_t *fifo_out;
extern rtqueue_t *fifo_in;
struct dsp_bus *dsp_global_bus_head;
#endif
