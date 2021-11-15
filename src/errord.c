/* errord.c
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

Copyright 2021 murray foster 

all of the following functions are copied from the following textbook:

Advanced Programming in the UNIX Environment
Second Edition
  W. Richard Stevens
  Stephen A. Rago
ISBN: 0-201-43307-9

there may be changes to both function names and anything related
to improving compatibility with cyperus' programming architecture.

*/

#include "errord.h"

/*
 * Initialize syslog(), if running as daemon.
 */
void
log_open(const char *ident, int option, int facility) {
  if(log_to_stderr == 0)
    openlog(ident, option, facility);
} /* log_open */

/*
 * Nonfatal error related to a system call.
 * Print a message with the system's errno value and return.
 */
void
log_ret(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  log_doit(1, LOG_ERR, fmt, ap);
  va_end(ap);  
} /* log_ret */

/*
 * Fatal error related to a system call.
 * Print a message and terminate.
 */
void
log_sys(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  log_doit(1, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(2);
} /* log_sys */

/*
 * Nonfatal error unrelated to a system call.
 * Print a message and return.
 */
void
log_msg(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  log_doit(0, LOG_ERR, fmt, ap);
  va_end(ap);
} /* log_msg */


/*
 * Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void
log_quit(const char *fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  log_doit(0, LOG_ERR, fmt, ap);
  va_end(ap);
  exit(2);
} /* log_quit */

/*
 * Print a message and retrun to caller.
 * Caller specifies "errnoflag" and "priority".
 */
static void
log_doit(int errnoflag, int priority, const char *fmt, va_list ap) {
  int errno_save;
  char buf[MAXLINE];

  errno_save = errno;
  vsnprintf(buf, MAXLINE, fmt, ap);
  if(errnoflag)
    snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s", strerror(errno_save));
  strcat(buf, "\n");
  if(log_to_stderr) {
    fflush(stdout);
    fputs(buf, stderr);
    fflush(stderr);
  } else {
    syslog(priority, buf);
  }
} /* log_doit */
