#include "tests/lib.h"

#define VOLUME "[disk]"
#define BUFFER_SIZE 1000

char buffer[BUFFER_SIZE];

/* Test that file I/O works.  Not an extraordinarily good test, but it shows
   that some things appear to work. */

int main() {
  int len;
  int file_science = syscall_open(VOLUME "science.txt");
  int file_sea_garden = syscall_open(VOLUME "sea-garden.txt");

  len = syscall_read(file_science, buffer, BUFFER_SIZE);
  putc('\n');
  buffer[MIN(BUFFER_SIZE, len) - 1] = '\0';
  puts(buffer);
  puts("\n=========================================\n");

  len = syscall_read(file_sea_garden, buffer, BUFFER_SIZE);
  putc('\n');
  buffer[MIN(BUFFER_SIZE, len) - 1] = '\0';
  puts(buffer);
  puts("\n=========================================\n");
  
  syscall_close(file_science);
  syscall_close(file_sea_garden);
  return 0;
}
