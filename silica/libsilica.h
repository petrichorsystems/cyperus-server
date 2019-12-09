/* libsilica.h
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

#ifndef LIBSILICA_H
#define LIBSILICA_H

#include "libsilica_dsp_types.h"
#include "fpga/xilinx/dma/libsilica_fpga_xilinx_dma.h"

typedef struct {
  char *name;
  int (*write)(void*, size_t);
  int (*read)(void*, size_t);
} silica_device_t;
#endif

int silica_init(char *device_name);
float *silica_add_float2(float x, float y);
