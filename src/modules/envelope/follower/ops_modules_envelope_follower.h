/* osp_modules_envelope_follower.h
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

#ifndef OPS_MODULES_ENVELOPE_FOLLOWER_H
#define OPS_MODULES_ENVELOPE_FOLLOWER_H

#include "../../../dsp_types.h"


int
dsp_create_envelope_follower(struct dsp_bus *target_bus,
                                    float attack,
                                    float decay,
                                    float scale);

int
dsp_destroy_envelope_follower(struct dsp_module *target_module);

void
dsp_envelope_follower(struct dsp_operation *envelope_follower, int jack_samplerate);

void
dsp_edit_envelope_follower(struct dsp_module *envelope_follower,
                                  float attack,
                                  float decay,
                                  float scale);

void
dsp_osc_listener_envelope_follower(struct dsp_operation *envelope_follower,
                                   int jack_samplerate);
#endif
