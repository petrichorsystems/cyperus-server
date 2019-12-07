/* silica_fpga_xilinx_dma.h
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

#ifndef SILICA_FPGA_XILINX_DMA_H
#define SILICA_FPGA_XILINX_DMA_H

#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>


static void timespec_sub(struct timespec *t1, const struct timespec *t2);

typedef struct {
  short unsigned int opcode;
  float input0;
  float input1;
  float schedule_flag;
} silica_dsp_instruction_t;

typedef struct {
  short unsigned int opcode;
  float result;
} silica_dsp_result_t;

int write_fpga_xilinx_dma(char *devicename, u_int32_t addr, u_int32_t size, u_int32_t offset, u_int32_t count, unsigned char **tx_buffer);
int read_fpga_xilinx_dma(char *devicename, uint32_t addr, uint32_t size, uint32_t offset, uint32_t count, unsigned char **rx_buffer);
int silica_write_fpga_xilinx_dma(unsigned char **buffer, size_t size);
int silica_read_fpga_xilinx_dma(unsigned char **buffer, size_t size);
#endif
