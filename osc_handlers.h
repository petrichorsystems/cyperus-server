/* osc_handlers.h
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

#include <lo/lo.h>

#include "dsp_types.h"
#include "libcyperus.h"

#ifndef OSC_HANDLERS_H
#define OSC_HANDLERS_H

int osc_add_sine_handler(const char *path, const char *types, lo_arg ** argv,
			 int argc, void *data, void *user_data);

int osc_edit_sine_handler(const char *path, const char *types, lo_arg ** argv,
			  int argc, void *data, void *user_data);

int osc_add_square_handler(const char *path, const char *types, lo_arg ** argv,
			   int argc, void *data, void *user_data);

int osc_edit_square_handler(const char *path, const char *types, lo_arg ** argv,
			    int argc, void *data, void *user_data);

int osc_add_pinknoise_handler(const char *path, const char *types, lo_arg ** argv,
			      int argc, void *data, void *user_data);

int osc_add_butterworth_biquad_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
					       int argc, void *data, void *user_data);

int osc_edit_butterworth_biquad_lowpass_handler(const char *path, const char *types, lo_arg ** argv,
						int argc, void *data, void *user_data);

int osc_add_delay_handler(const char *path, const char *types, lo_arg ** argv,
			  int argc, void *data, void *user_data);
int
osc_edit_delay_handler(const char *path, const char *types, lo_arg ** argv,
		       int argc, void *data, void *user_data);

int osc_add_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
				int argc, void *data, void *user_data);
int
osc_edit_pitch_shift_handler(const char *path, const char *types, lo_arg ** argv,
			     int argc, void *data, void *user_data);
int
osc_add_vocoder_handler(const char *path, const char *types, lo_arg ** argv,
			int argc, void *data, void *user_data);
int
osc_edit_vocoder_handler(const char *path, const char *types, lo_arg ** argv,
			 int argc, void *data, void *user_data);

#endif
