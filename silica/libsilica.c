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


int _float_to_bytes(int index, unsigned char outbox[], float member)
{
  unsigned long d = *(unsigned long *)&member;

  outbox[index] = d & 0x00FF;
  index++;

  outbox[index] = (d & 0xFF00) >> 8;
  index++;

  outbox[index] = (d & 0xFF0000) >> 16;
  index++;

  outbox[index] = (d & 0xFF000000) >> 24;
  index++;
  return index;
}


float _bytes_to_float(int index, unsigned char outbox[])
{
  unsigned long d;

  d =  (outbox[index+3] << 24) | (outbox[index+2] << 16)
    | (outbox[index+1] << 8) | (outbox[index]);
  float member = *(float *)&d;
  return member;
}

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

  return 0;
}

float silica_add_float2(float x0, float x1) {
  libsilica_dsp_add_float2_instruction_t instruction;

  unsigned char temp_value_bytes[8];
  float temp_value;

  libsilica_dsp_add_float2_result_t *result;
  dsp_opcode opcode = ADD_FLOAT2;
  int num_x = 2;
  
  instruction.opcode = opcode;
  instruction.x = malloc(sizeof(float) * num_x);
  instruction.x[0] = x0;
  instruction.x[1] = x1;
  instruction.num_x = num_x;
  
  /* silica_device->write((unsigned char**)&instruction, sizeof(instruction)); */

  _float_to_bytes(0, temp_value_bytes, x0);
  // _float_to_bytes(4, temp_value_bytes, x1);

  
  silica_device->write((void *)&temp_value_bytes, 4);
  silica_device->read(&temp_value_bytes, 4);

  temp_value = _bytes_to_float(0, temp_value_bytes);
  
  printf("silica_add_float2: received: %f\n", temp_value);
  
  free(instruction.x);

  return temp_value;
}
