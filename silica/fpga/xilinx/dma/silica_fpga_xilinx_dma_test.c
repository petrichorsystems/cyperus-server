#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "silica_fpga_xilinx_dma.h"

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

  transferred_bytes = silica_write_fpga_xilinx_dma(&tx_data, transfer_size);
  printf("write success\n");

  rx_data = malloc((size_t)transfer_size);
  transferred_bytes = silica_read_fpga_xilinx_dma(&rx_data, transfer_size);
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
