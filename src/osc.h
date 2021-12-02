/* osc.h
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

#ifndef OSC_H
#define OSC_H

#include <lo/lo.h>

#include "dsp.h"
#include "osc_handlers.h"

extern char *send_host_out;
extern char *send_port_out;
extern lo_server_thread lo_thread;

typedef struct osc_handler_user_defined_type {
  char *osc_path;
  char *type_str;
  int num_ports;
  char **ports;

  struct osc_handler_user_defined_type *next;
  struct osc_handler_user_defined_type *prev;  
} osc_handler_user_defined_t;

extern osc_handler_user_defined_t *global_osc_handlers_user_defined;
extern pthread_mutex_t global_osc_handlers_user_defined_lock;

osc_handler_user_defined_t* osc_handler_user_defined_init(char *osc_path, char *type_str, int num_module_ports, char **module_ports);

void osc_handler_user_defined_insert_tail(osc_handler_user_defined_t *head_handler, osc_handler_user_defined_t *new_handler);

void osc_execute_handler_parameter_assignment(osc_handler_user_defined_t *handler, lo_arg** argv);

int osc_setup(char *osc_port_in, char *osc_port_out, char *addr);
int osc_change_address(char *request_id, char *new_host_out, char *new_port_out);

#include "modules/audio/analysis/transient_detector/osc_modules_audio_analysis_transient_detector.h"
#include "modules/audio/filter/moogff/osc_modules_audio_filter_moogff.h"
#include "modules/audio/filter/varslope_lowpass/osc_modules_audio_filter_varslope_lowpass.h"
#include "modules/audio/oscillator/pulse/osc_modules_audio_oscillator_pulse.h"
#include "modules/motion/envelope/adsr/osc_modules_motion_envelope_adsr.h"
#include "modules/motion/envelope/segment/osc_modules_motion_envelope_segment.h"
#include "modules/motion/osc/osc_metronome/osc_modules_motion_osc_osc_metronome.h"
#include "modules/motion/envelope/segment/osc_modules_motion_envelope_segment.h"
#endif

