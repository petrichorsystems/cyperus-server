#include <assert.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Subtract timespec t2 from t1
 *
 * Both t1 and t2 must already be normalized
 * i.e. 0 <= nsec < 1000000000 */
static void timespec_sub(struct timespec *t1, const struct timespec *t2)
{
  assert(t1->tv_nsec >= 0);
  assert(t1->tv_nsec < 1000000000);
  assert(t2->tv_nsec >= 0);
  assert(t2->tv_nsec < 1000000000);
  t1->tv_sec -= t2->tv_sec;
  t1->tv_nsec -= t2->tv_nsec;
  if (t1->tv_nsec >= 1000000000)
  {
    t1->tv_sec++;
    t1->tv_nsec -= 1000000000;
  }
  else if (t1->tv_nsec < 0)
  {
    t1->tv_sec--;
    t1->tv_nsec += 1000000000;
  }
}

int write_fpga_xilinx_dma(char *devicename, u_int32_t addr, u_int32_t size, u_int32_t offset, u_int32_t count, unsigned char **tx_buffer)
{
  int rc;
  int write_count = 0;
  char *buffer = NULL;
  char *allocated = NULL;
  struct timespec ts_start, ts_end;

  posix_memalign((void **)&allocated, 4096/*alignment*/, size + 4096);
  assert(allocated);
  buffer = allocated + offset;
  printf("host memory buffer = %p\n", buffer);

  int file_fd = -1;
  int fpga_fd = open(devicename, O_RDWR);
  assert(fpga_fd >= 0);

  /* select AXI MM address */
  off_t off = lseek(fpga_fd, addr, SEEK_SET);
  while (count--) {

    rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);
    /* write buffer to AXI MM address using SGDMA */
    memcpy(buffer, tx_buffer, size);
    rc = write(fpga_fd, buffer, size);
    rc += write_count;
    assert(rc == size);
    rc = clock_gettime(CLOCK_MONOTONIC, &ts_end);
  }
  /* subtract the start time from the end time */
  timespec_sub(&ts_end, &ts_start);
  /* display passed time, a bit less accurate but side-effects are accounted for */
  printf("CLOCK_MONOTONIC reports %ld.%09ld seconds (total) for last transfer of %d bytes\n",
    ts_end.tv_sec, ts_end.tv_nsec, size);

  close(fpga_fd);
  if (file_fd >= 0) {
    close(file_fd);
  }
  free(allocated);

  return write_count;
}

int read_fpga_xilinx_dma(char *devicename, uint32_t addr, uint32_t size, uint32_t offset, uint32_t count, unsigned char **rx_buffer)
{
  int rc;
  int read_count = 0;
  int time;
  char *buffer = NULL;
  char *allocated = NULL;
  struct timespec ts_start, ts_end;

  posix_memalign((void **)&allocated, 4096/*alignment*/, size + 4096);
  assert(allocated);
  buffer = allocated + offset;
  printf("host memory buffer = %p\n", buffer);

  int file_fd = -1;
  int fpga_fd = open(devicename, O_RDWR | O_NONBLOCK);
  assert(fpga_fd >= 0);

  while (count--) {
    memset(buffer, 0x00, size);
    /* select AXI MM address */
    off_t off = lseek(fpga_fd, addr, SEEK_SET);
    rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);
    /* read data from AXI MM into buffer using SGDMA */
    rc = read(fpga_fd, buffer, size);
    read_count += rc;
    if ((rc > 0) && (rc < size)) {
      printf("Short read of %d bytes into a %d bytes buffer, could be a packet read?\n", rc, size);
    }
    rc = clock_gettime(CLOCK_MONOTONIC, &ts_end);
    memcpy(rx_buffer, buffer, size);
  }
  /* subtract the start time from the end time */
  timespec_sub(&ts_end, &ts_start);
  /* display passed time, a bit less accurate but side-effects are accounted for */
  printf("CLOCK_MONOTONIC reports %ld.%09ld seconds (total) for last transfer of %d bytes\n", ts_end.tv_sec, ts_end.tv_nsec, size);

  close(fpga_fd);
  if (file_fd >=0) {
    close(file_fd);
  }
  free(allocated);

  return read_count;
}

int silica_write_fpga_xilinx_dma(unsigned char **buffer, size_t size) {
  int write_count = write_fpga_xilinx_dma("/dev/xdma0_h2c_0", 0, size, 0, 1, buffer);
  if ( write_count < 0) {
    return -1;
  }
  return write_count;
}

int silica_read_fpga_xilinx_dma(unsigned char **buffer, size_t size) {
  int read_count = read_fpga_xilinx_dma("/dev/xdma0_c2h_0", 0, size, 0, 1, buffer);
  if ( read_count != size) {
    return -1;
  }
  return read_count;
}
