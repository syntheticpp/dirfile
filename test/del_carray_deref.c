/* Attempt to delete a field */
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
  const char* format_data = "data CARRAY UINT8 13 14 15 16 17\n"
    "raw RAW UINT8 data<2>\n";
  int fd, ret, error, r = 0;
  DIRFILE *D;
  gd_spf_t spf;

  mkdir(filedir, 0777);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  D = gd_open(filedir, GD_RDWR);
  ret = gd_delete(D, "data", GD_DEL_DEREF);
  error = gd_error(D);
  spf = gd_spf(D, "raw");
  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(error, GD_E_OK);
  CHECKU(spf, 15);
  CHECKI(ret, 0);

  return r;
}