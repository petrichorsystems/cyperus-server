/* osp_modules_envelope_segment.h
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

#ifndef OPS_MODULES_ENVELOPE_SEGMENT_H
#define OPS_MODULES_ENVELOPE_SEGMENT_H

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
#include "../../math_utils.h"

#include "math_modules_modules_envelope_segment.h"
#include "ops_modules_modules_envelope_segment.h"

int
dsp_create_modules_envelope_segment(struct dsp_bus *target_bus,
                                     float *levels,
                                     float *times,
                                     float *shape,
                                     float *curve,
                                     int release_node,
                                     int loop_node,
                                     int offset,
                                     float gate,
                                     float level_scale,
                                     float level_bias,
                                     float time_scale,
                                     int num_stages);


void
dsp_modules_envelope_segment(struct dsp_operation *envelope_segment,
                              int jack_samplerate,
                              int pos);
void
dsp_edit_modules_envelope_segment(struct dsp_module *envelope_segment,
                                   int release_node,
                                   int loop_noe,
                                   int offset,
                                   float gate,
                                   float level_scale,
                                   float level_bias,
                                   float time_scale);
#endif
