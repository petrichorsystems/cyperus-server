/* errord.h
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


#ifndef LIBCYPERUS_ERROR_H_
#define LIBCYPERUS_ERROR_H_

#include <errno.h>
#include <stdarg.h>
#include <syslog.h>

#define MAXLINE 4096

/*
 * Caller must define and set this: nonzero if
 * intercative, zero if daemon.
 */
static void log_doit(int, int, const char *, va_list ap);

extern int log_to_stderr;

void log_open(const char *ident, int option, int facility);
void log_ret(const char *fmt, ...);
void log_sys(const char *fmt, ...);
void log_msg(const char *fmt, ...);
void log_quit(const char *fmt, ...);
#endif

