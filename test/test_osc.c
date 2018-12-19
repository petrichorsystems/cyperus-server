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
#include <stdlib.h> //exit(0);
#include <string.h>
#include <unistd.h>
#include <lo/lo.h>

lo_address t;

char *incoming_message;

/* catch any incoming messages and display them. returning 1 means that the
 * message has not been fully handled and the server should try other methods */
int generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data) {
    int i;

    printf("path: <%s>\n", path);
    for (i = 0; i < argc; i++) {
        printf("arg %d '%c' ", i, types[i]);
        lo_arg_pp((lo_type)types[i], argv[i]);
        printf("\n");
    }
    printf("\n");
    fflush(stdout);

    return 1;
}

int osc_list_main_handler(const char *path, const char *types, lo_arg **argv,
			   int argc, void *data, void *user_data)
{
  char *mains_str = argv[0];
  incoming_message = malloc(sizeof(char) * (strlen(mains_str) + 1));
  strcpy(incoming_message, mains_str);

  return 0;
} /* osc_list_main_handler */

void error(int num, const char *msg, const char *path) {
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

void test_passthru_single_channel() {
  char *mains_str;
  char *main_in_0, *main_out_0;
  int count;
  
  lo_send(t, "/cyperus/list/main", NULL);
  usleep(100);
  mains_str = malloc(sizeof(char) * (strlen(incoming_message) + 1));
  strcpy(mains_str, incoming_message);
  free(incoming_message);

  int out_pos;
  char *subptr = malloc(sizeof(char) * (strlen(mains_str) + 1));
  main_in_0 = malloc(sizeof(char) * 44);
  for(count=4; count<47; count++) {
    main_in_0[count - 4] = mains_str[count];
  }
  free(subptr);
  printf("main_in_0: %s\n", main_in_0);

  main_out_0 = malloc(sizeof(char) * 44);
  subptr = strstr(mains_str, "out:");
  out_pos = subptr - mains_str;
  for(count=out_pos + 5; count<out_pos + 44 + 4; count++) {
    main_out_0[count - 5 - out_pos] = mains_str[count];
  }
  printf("main_out_0: %s\n", main_out_0);

  lo_send(t, "/cyperus/add/connection", "ss", main_in_0, main_out_0);

  free(main_in_0);
  free(main_out_0);
  free(mains_str);
  
} /* test_passthru_single_channel */

int main(void) {
  lo_server_thread st = lo_server_thread_new("97217", error);
  lo_server_thread_add_method(st, "/cyperus/list/main", "s", osc_list_main_handler, NULL);
  lo_server_thread_start(st);

  t = lo_address_new("127.0.0.1", "97211");
  
  test_passthru_single_channel();
  //lo_send(t, "/cyperus/add/connection", "ss", "/mains{ffae0fee-eed5-46b3-8c84-cd99871dbc7b", "/mains}df1b2555-97ba-4635-a071-f7860155a71b"); 
  //lo_send(t, "/cyperus/add/connection", "ss", "/mains{ffae0fee-eed5-46b3-8c84-cd99871dbc7b", "/mains}df1b2555-97ba-4635-a071-f7860155a71b");

  while(1) {
  usleep(1000);
  }
  
  exit(0);
}

