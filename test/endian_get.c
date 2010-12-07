/* Test endianness */
#include "../src/config.h"
#include "test.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

int main(void)
{
  const char* filedir = __TEST__ "dirfile";
  const char* format = __TEST__ "dirfile/format";
  const char* format1 = __TEST__ "dirfile/format1";
  const char* format2 = __TEST__ "dirfile/format2";
  const char* format3 = __TEST__ "dirfile/format3";
  const char* format_data = "ENDIAN little\nINCLUDE format1\n";
  const char* format_data1 = "ENDIAN big\nINCLUDE format2\n";
  const char* format_data2 = "ENDIAN big arm\nINCLUDE format3\n";
  const char* format_data3 = "ENDIAN little arm\n";
  int fd, error, r = 0;
  unsigned long n, m, l, k;
  DIRFILE *D;

  mkdir(filedir, 0777);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  fd = open(format1, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data1, strlen(format_data1));
  close(fd);

  fd = open(format2, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data2, strlen(format_data2));
  close(fd);

  fd = open(format3, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data3, strlen(format_data3));
  close(fd);

  D = gd_open(filedir, GD_RDONLY | GD_VERBOSE);
  n = gd_endianness(D, 0);
  m = gd_endianness(D, 1);
  l = gd_endianness(D, 2);
  k = gd_endianness(D, 3);
  error = gd_error(D);

  gd_close(D);

  unlink(format);
  unlink(format1);
  unlink(format2);
  unlink(format3);
  rmdir(filedir);

  CHECKI(error, 0);
  CHECKX(n, GD_LITTLE_ENDIAN | GD_NOT_ARM_ENDIAN);
  CHECKX(m, GD_BIG_ENDIAN | GD_NOT_ARM_ENDIAN);
  CHECKX(l, GD_BIG_ENDIAN | GD_ARM_ENDIAN);
  CHECKX(k, GD_LITTLE_ENDIAN | GD_ARM_ENDIAN);

  return r;
}
