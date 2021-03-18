/* threadsync.c
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

#include <stdio.h> //printf
#include "threadsync.h"

void
threadsync_init() {
pthread_cond_init(&threadsync_is_waiting_cond, NULL);
} /* threadsync_init */

void
threadsync_sync() {
  pthread_cond_broadcast(&threadsync_is_waiting_cond);
} /* threadsync_sync */

void
threadsync_wait_for_sync() {
  pthread_mutex_lock(&threadsync_is_waiting_mutex);
  pthread_cond_wait(&threadsync_is_waiting_cond, &threadsync_is_waiting_mutex);
  pthread_mutex_unlock(&threadsync_is_waiting_mutex); 
} /* threadsync_wait_for_sync */
