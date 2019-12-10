/* silica_fpga_xilinx_dma_test.c
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

#include "libsilica_fpga_xilinx_dma.h"

unsigned char *gen_rdm_bytestream (size_t num_bytes)
{
  unsigned char *stream = malloc (num_bytes);
  size_t i;

  for (i = 0; i < num_bytes; i++)
  {
    stream[i] = rand ();
  }

  return stream;
}

int main() {
  int transfer_size = 4096;
  int transferred_bytes = 0;

  unsigned char *tx_data = NULL;
  unsigned char *rx_data = NULL;

  srand ((unsigned int) time (NULL));
  
  tx_data = gen_rdm_bytestream((size_t)transfer_size);

  transferred_bytes = libsilica_write_fpga_xilinx_dma(&tx_data, transfer_size);
  printf("write success\n");

  rx_data = malloc((size_t)transfer_size);
  tra nsferred_bytes = libsilica_read_fpga_xilinx_dma(&rx_data, transfer_size);
  printf("read success\n");
  
  for(int i=0; i<transfer_size; i++) {
    printf("i:%d, tx:%d, rx:%d\n", i, tx_data[i], rx_data[i]);
    if(tx_data[i] != rx_data[i]) {
      printf("data corruption\n");
      exit(1);
    }
  }

  printf("no data corruption\n");

  free(tx_data);
}
