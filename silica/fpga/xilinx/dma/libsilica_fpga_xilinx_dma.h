/* libsilica_fpga_xilinx_dma.h
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

#ifndef LIBSILICA_FPGA_XILINX_DMA_H
#define LIBSILICA_FPGA_XILINX_DMA_H

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
int write_fpga_xilinx_dma(char *devicename, u_int32_t addr, u_int32_t size, u_int32_t offset, u_int32_t count, void *tx_buffer);
int read_fpga_xilinx_dma(char *devicename, uint32_t addr, uint32_t size, uint32_t offset, uint32_t count, void *rx_buffer);
int libsilica_write_fpga_xilinx_dma(void *buffer, size_t size);
int libsilica_read_fpga_xilinx_dma(void *buffer, size_t size);
#endif
