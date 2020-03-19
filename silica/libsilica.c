/* libsilica.c
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

#include "fpga/xilinx/dma/libsilica_fpga_xilinx_dma.h"
#include "libsilica.h"
#include "libsilica_dsp_types.h"



silica_device_t *silica_device;

int silica_init(char *device_name) {
  printf("sizeof(libsilica_dsp_add_float2_instruction_t): %ld\n",
         sizeof(libsilica_dsp_add_float2_instruction_t));
    printf("sizeof(libsilica_dsp_add_float2_result_t): %ld\n",
         sizeof(libsilica_dsp_add_float2_result_t));

  silica_device = NULL;
  silica_device = malloc(sizeof(silica_device_t));
  if (strcmp(device_name, "fpga_xilinx_dma") == 0) {
    silica_device->name = "fpga_xilinx_dma";
    silica_device->write = &libsilica_write_fpga_xilinx_dma;
    silica_device->read = &libsilica_read_fpga_xilinx_dma;
  } else {
    printf("unknown device, init failed\n");
    return -1;
  }

  printf("finished initialization\n");
  return 0;
}

int silica_add_float2(int x0, int x1) {
  libsilica_dsp_add_float2_instruction_t instruction;

  int temp_value;
  int temp_values[2] = {0.0};
  int temp_values_in[2] = {0.0};

  libsilica_dsp_add_float2_result_t *result;
  dsp_opcode opcode = ADD_FLOAT2;
  int num_x = 2;
  
  instruction.opcode = opcode;
  instruction.x = malloc(sizeof(int) * num_x);
  instruction.x[0] = x0 * ;
  instruction.x[1] = x1;
  instruction.num_x = num_x;
  int y;

  temp_values[0] = x0;
  temp_values[1] = x1;
  
  printf("before write\n");
  silica_device->write(&temp_values, sizeof(temp_values));
  printf("after write\n");

  silica_device->read(&temp_values_in, sizeof(temp_values));
  
  printf("silica_add_float2: received: [0]: %d\n", temp_values_in[0]);
  printf("silica_add_float2: received: [1]: %d\n", temp_values_in[1]);
  
  free(instruction.x);

  return temp_values_in[0];
}

int silica_add_float3(int x0, int x1, int x2) {
  libsilica_dsp_add_float2_instruction_t instruction;

  int temp_value;
  int temp_values[3] = {0.0};
  int temp_values_in[3] = {0.0};

  libsilica_dsp_add_float2_result_t *result;
  dsp_opcode opcode = ADD_FLOAT3;
  int num_x = 3;
  
  instruction.opcode = opcode;
  instruction.x = malloc(sizeof(int) * num_x);
  instruction.x[0] = x0;
  instruction.x[1] = x1;
  instruction.x[2] = x2;
  instruction.num_x = num_x;
  int y;

  temp_values[0] = x0;
  temp_values[1] = x1;
  temp_values[2] = x2;
  
  printf("before write\n");
  silica_device->write(&temp_values, sizeof(temp_values));
  printf("after write\n");

  silica_device->read(&temp_values_in, sizeof(temp_values));
  
  printf("silica_add_float2: received: [0]: %d\n", temp_values_in[0]);
  printf("silica_add_float2: received: [1]: %d\n", temp_values_in[1]);
  printf("silica_add_float2: received: [2]: %d\n", temp_values_in[2]);
  
  free(instruction.x);

  return temp_values_in[0];
}
