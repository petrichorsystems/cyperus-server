/* test_osc.c
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

#include "cyperus.h"
#include "rtqueue.h"
#include "jackcli.h"
#include "dsp.h"
#include "dsp_ops.h"
#include "osc_handlers.h"

int
print_usage() {
  fprintf(stdout,"Usage: cyperus [options] [arg]\n\n");
  fprintf(stdout,"Options:\n"
	  " -h,  --help          displays this menu\n"
	  " -i,  --input         input channels.  default: 8\n"
	  " -o,  --output        output channels. default: 8\n"
	  " -b,  --bitdepth      set bitdepth of capture to 8,16,24,32,64, or 128. default: 24\n"
	  " -p,  --port          set osc interface receiving port. default: 97211\n"
	  " -sp, --send-port     set osc interface sending port. default: 97217\n"
	  " -f,  --file          set path of session file to load preexisting sounds.\n\n"
          "documentation available soon\n\n");
  exit(0);

  return 0;
} /* print_usage */

void print_header(void) {
  printf("\n"
	 "welcome to the\n"	                             
	 ",-. . . ,-. ,-. ,-. . . ,-. \n"
	 "|   | | | | |-' |   | | `-. \n"
	 "`-' `-| |-' `-' '   `-' `-' \n"
	 "     /| |                   \n"
	 "    `-' '                   \n"
	 "\t\trealtime music system\n");
  printf("\n");
} /* print_header */
 
int main(int argc, char *argv[])
{
  int i=0;
  int input;
  int output;
  int bitdepth;
  char *osc_port_in;
  char *osc_port_out;

  if( argc > 1 )
    if( !strcmp(argv[1], "-h") ||
	!strcmp(argv[1], "--help") ) {
      printf("welcome to the cyperus realtime music system\n\n");
      print_usage();
      exit(0);
    }
  
  print_header();
  jackcli_setup("cyperus", 32, 8, 8);
  
  pthread_t dsp_thread_id;
  //pthread_create(&dsp_thread_id, NULL, dsp_thread, NULL);
  //pthread_detach(dsp_thread_id);
  
  while(1){sleep(1);};

  jackcli_teardown();
  
  return 0;
}
