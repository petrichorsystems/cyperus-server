/* threadsync.h
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

#ifndef THREADSYNC_H

#include <stdio.h> //printf
#include <pthread.h>

extern pthread_mutex_t threadsync_is_waiting_mutex;
extern pthread_cond_t threadsync_is_waiting_cond;

void threadsync_init();
void threadsync_sync();
void threadsync_wait_for_sync();

#endif
