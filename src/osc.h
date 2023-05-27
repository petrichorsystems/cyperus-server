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

/* typedef struct osc_handler_user_defined_type { */
/*   char *osc_path; */
/*   char *type_str; */
/*   int num_ports; */
/*   char **ports; */

/*   struct osc_handler_user_defined_type *next; */
/*   struct osc_handler_user_defined_type *prev;   */
/* } osc_handler_user_defined_t; */

/* extern osc_handler_user_defined_t *global_osc_handlers_user_defined; */
/* extern pthread_mutex_t global_osc_handlers_user_defined_lock; */

/* osc_handler_user_defined_t* osc_handler_user_defined_init(char *osc_path, char *type_str, int num_module_ports, char **module_ports); */

/* void osc_handler_user_defined_insert_tail(osc_handler_user_defined_t *head_handler, osc_handler_user_defined_t *new_handler); */

/* void osc_execute_handler_parameter_assignment(osc_handler_user_defined_t *handler, lo_arg** argv); */

int osc_setup(char *osc_port_in, char *osc_port_out, char *addr);
int osc_change_address(char *request_id, char *new_host_out, char *new_port_out);

void *osc_listener_thread(void *arg);
void *osc_dsp_load_thread(void *arg);

#include "modules/delay/simple/osc_modules_delay_simple.h"
#include "modules/analysis/transient_detector/osc_modules_analysis_transient_detector.h"
#include "modules/filter/bandpass/osc_modules_filter_bandpass.h"
#include "modules/filter/moogff/osc_modules_filter_moogff.h"
#include "modules/filter/varslope_lowpass/osc_modules_filter_varslope_lowpass.h"
#include "modules/oscillator/sine/osc_modules_oscillator_sine.h"
#include "modules/oscillator/sawtooth/osc_modules_oscillator_sawtooth.h"
#include "modules/oscillator/triangle/osc_modules_oscillator_triangle.h"
#include "modules/oscillator/pulse/osc_modules_oscillator_pulse.h"
#include "modules/oscillator/clock/osc_modules_oscillator_clock.h"
#include "modules/envelope/adsr/osc_modules_envelope_adsr.h"
#include "modules/envelope/follower/osc_modules_envelope_follower.h"

#include "modules/osc/osc_metronome/osc_modules_osc_osc_metronome.h"
#include "modules/osc/float/osc_modules_osc_float.h"
#include "modules/utils/counter/osc_modules_utils_counter.h"
#include "modules/utils/equals/osc_modules_utils_equals.h"
#include "modules/utils/spigot/osc_modules_utils_spigot.h"

#include "modules/envelope/segment/osc_modules_envelope_segment.h"

#endif

