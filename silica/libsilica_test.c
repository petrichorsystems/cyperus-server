/* silica_test.c
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

Copyright 2019 murray foster */

#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "libsilica.h"

int main() {
  char *device_name = "fpga_xilinx_dma";
  int add_float2_output;
  int add_float3_output;
  
  silica_init(device_name);
  add_float2_output = silica_add_float2(312, 48);

  printf("add_float2_output: %d\n", add_float2_output);
  if(add_float2_output == 360) {
    printf("silica_add_float2_output success\n");
  } else {
    printf("silica_add_float2_output failed\n");
  }

  add_float3_output = silica_add_float3(312, 48, 10);
  
  printf("add_float3_output: %d\n", add_float3_output);
  if(add_float3_output == 370) {
    printf("silica_add_float3_output success\n");
  } else {
    printf("silica_add_float3_output failed\n");
  }
}
