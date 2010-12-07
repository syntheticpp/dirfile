/* The SPF of a polynom should equal the SPF of its input field */
#include "test.h"

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
  const char* format_data =
    "in RAW UINT8 11\n"
    "polynom POLYNOM in 1 2 1 3\n";
  int fd, r = 0;
  gd_spf_t spf;
  DIRFILE *D;

  mkdir(filedir, 0777);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  D = gd_open(filedir, GD_RDONLY | GD_VERBOSE);
  spf = gd_spf(D, "polynom");
  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(spf,11);
  return r;
}
