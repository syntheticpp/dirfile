/* Attempt to write UINT8 past the beginning-of-field*/
#include "test.h"

#include <inttypes.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(void)
{
  const char* filedir = __TEST__ "dirfile";
  const char* format = __TEST__ "dirfile/format";
  const char* format_data = "data RAW UINT8 8\nFRAMEOFFSET 10000";
  uint8_t c[8];
  int fd, n, error, r = 0;
  DIRFILE *D;

  mkdir(filedir, 0777);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  D = gd_open(filedir, GD_RDWR | GD_UNENCODED);
  n = gd_putdata(D, "data", 5, 0, 1, 0, GD_UINT8, c);
  error = gd_error(D);

  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(n,0);
  CHECKI(error,GD_E_RANGE);
  return r;
}
