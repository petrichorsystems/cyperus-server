/* cyperus.c
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

#include "main.h"

void print_usage() {
  printf("Usage: cyperus [options] [arg]\n\n");
  printf("Options:\n"
	 " -h,   --help          displays this menu\n"
	 " -i,   --input         input channels.  default: 8\n"
	 " -o,   --output        output channels. default: 8\n"
	 " -bd,  --bitdepth      bitdepth of capture to 8, 16, 24, 32, 64, or 128. default: 24\n"
	 " -rp,  --receive-port  osc interface receiving port. default: 97211\n"
	 " -sp,  --send-port     osc interface sending port. default: 97217\n"
         " -p,   --period        frames per period. default: 128\n"
	 " -fi,  --fifo-size     fifo buffer size for each channel. default: 2048\n\n"
	 "documentation available soon\n\n");
} /* print_usage */

void print_header(void) {
  printf("\n\n"
	 "welcome to the\n"	                             
	 ",-. . . ,-. ,-. ,-. . . ,-. \n"
	 "|   | | | | |-' |   | | `-. \n"
	 "`-' `-| |-' `-' '   `-' `-' \n"
	 "     /| |                   \n"
	 "    `-' '                   \n"
	 "\t\trealtime music system\n\n\n");
} /* print_header */

int main(int argc, char *argv[])
{
  dsp_global_bus_head = NULL;
  dsp_global_connection_graph = NULL;
  
  dsp_global_translation_graph = NULL;
  dsp_global_operation_head = NULL;

  dsp_optimized_main_ins = NULL;
  dsp_optimized_main_outs = NULL;

  dsp_global_translation_connection_graph_processing = NULL;
  dsp_global_operation_head_processing = NULL;
 
  int c = 0;
  int input = 8;
  int output = 8;
  int bitdepth;
  int fifo_size = 2048;
  char *osc_port_in = NULL;
  char *osc_port_out = NULL;
  int period = 128;

  char *store_flag=NULL;
  char *store_input=NULL;

  int exit_key;
  
  if( argc > 1 )
    if( !strcmp(argv[1], "-h") ||
	!strcmp(argv[1], "--help") ) {
      printf("welcome to the cyperus realtime music system\n\n");
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
	  if( !strcmp(store_flag,"-rp") ||
	      !strcmp(store_flag,"--receive-port")) {
	    store_input=argv[c+1];
	    osc_port_in=store_input;
	  }

	  if( !strcmp(store_flag,"-sp") ||
	      !strcmp(store_flag,"--send-port")) {
	    store_input=argv[c+1];
	    osc_port_out=store_input;
	  }
	  
	  if( !strcmp(store_flag,"-bd") ||
	      !strcmp(store_flag,"--bitdepth")) {
	    store_input = argv[c+1];
	    bitdepth=atoi(store_input);
	  }
	  
	  if( !strcmp(store_flag,"-i") ||
	      !strcmp(store_flag,"--input")) {
	    store_input = argv[c+1];
	    input=atoi(store_input);
	  }

	  if( !strcmp(store_flag,"-o") ||
	      !strcmp(store_flag,"--output")) {
	    store_input = argv[c+1];
	    output=atoi(store_input);
	  }

	  if( !strcmp(store_flag,"-p") ||
	      !strcmp(store_flag,"--period")) {
	    store_input = argv[c+1];
	    period=atoi(store_input);
	  }
          
	  if( !strcmp(store_flag,"-fi") ||
	      !strcmp(store_flag,"--fifo-size")) {
	    store_input = argv[c+1];
	    fifo_size=atoi(store_input);
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
  
  /* if it's not 8, 16, 24, 32, or 64 assign 24 bits as default */
  switch(bitdepth) {
  case 8:
    break;
  case 16:
    break;
  case 24:
    break;
  case 32:
    break;
  case 64:
    break;
  default:
    bitdepth=24;
  }

  /* dsp setup, begin */
  dsp_setup(period, input, output);
  dsp_graph_id_init();
  threadsync_init();

  jackcli_setup("cyperus", bitdepth, input, output, fifo_size);

  printf("channels in: %d\n", input);
  printf("channels out: %d\n", output);
  printf("buffer size: %d\n", jackcli_buffer_size);
  printf("bitdepth: %dbits\n", bitdepth);
  printf("fifo size: %d\n", fifo_size);
  printf("osc receive port: %s\n", osc_port_in);
  printf("osc send port: %s\n\n\n", osc_port_out);
  
  osc_setup(osc_port_in, osc_port_out, "127.0.0.1");

  /* pthread_t osc_listener_thread_id; */
  /* pthread_create(&osc_listener_thread_id, NULL, osc_listener_thread, NULL); */
  /* pthread_detach(osc_listener_thread_id); */

  pthread_t osc_dsp_load_thread_id;
  pthread_create(&osc_dsp_load_thread_id, NULL, osc_dsp_load_thread, NULL);
  pthread_detach(osc_dsp_load_thread_id);
  
  printf("press <ENTER> to quit\n\n");
  
  while(1) {
    usleep(500000);
  }
  
  jackcli_teardown();
  
  return 0;
}
