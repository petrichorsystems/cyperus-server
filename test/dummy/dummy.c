/* dummy.c
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
#include <string.h> //memset
#include <stdlib.h> //exit(0);
#include <jack/jack.h> 
#include <math.h>
#include <lo/lo.h>
#include <fftw3.h>
#include <signal.h>
#include <unistd.h>

#include "dummy.h"
#include "osc.h"
#include "osc_handlers.h"

void print_usage() {
  printf("Usage: cyperus [options] [arg]\n\n");
  printf("Options:\n"
	 " -h,  --help          displays this menu\n"
	 " -p,  --port          set osc interface receiving port. default: 97211\n"
	 " -sp, --send-port     set osc interface sending port. default: 97217\n\n"
	 "documentation available soon\n\n");
} /* print_usage */

void print_header(void) {
  printf("\n\n"
	 "welcome to the\n"
         "    DUMMY OSC SERVER        \n"
	 ",-. . . ,-. ,-. ,-. . . ,-. \n"
	 "|   | | | | |-' |   | | `-. \n"
	 "`-' `-| |-' `-' '   `-' `-' \n"
	 "     /| |                   \n"
	 "    `-' '                   \n"
	 "\t\trealtime music system\n\n\n");
} /* print_header */

int main(int argc, char *argv[])
{
  int c = 0;
  char *osc_port_in = NULL;
  char *osc_port_out = NULL;

  char *store_flag=NULL;
  char *store_input=NULL;

  int exit_key;
  
  if( argc > 1 )
    if( !strcmp(argv[1], "-h") ||
	!strcmp(argv[1], "--help") ) {
      printf("welcome to the [DUMMY OSC SERVER] cyperus realtime music system\n\n");
      print_usage();
      exit(0);
    }
  
  print_header();

  /* process command-line input */
  for(c=1; c<argc; c++)
    {
      store_flag = argv[c];
      if( store_flag != NULL )
	{
	  if( !strcmp(store_flag,"-p") ||
	      !strcmp(store_flag,"--port")) {
	    store_input=argv[c+1];
	    osc_port_in=store_input;
	  }

	  if( !strcmp(store_flag,"-sp") ||
	      !strcmp(store_flag,"--send-port")) {
	    store_input=argv[c+1];
	    osc_port_out=store_input;
	  }
	  
	  /* reset temporarily stored flag&input */
	  store_input=NULL;
	  store_flag=NULL;
	}
    }

  if( osc_port_in == NULL )
    osc_port_in="97211";
  
  if( osc_port_out == NULL )
    osc_port_out="97217";
  
  printf("osc receive port: %s\n", osc_port_in);
  printf("osc send port: %s\n", osc_port_out);
  printf("\n");
  
  osc_setup(osc_port_in, osc_port_out, "127.0.0.1");
  
  printf("press <CTRL+C> to quit\n\n");
  
  while(1) {
    usleep(500000);
  }

  return 0;
}
