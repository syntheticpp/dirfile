/* Closing a dirfile should succeed cleanly */
#include "test.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(void)
{
  const char* filedir = __TEST__ "dirfile";
  const char* format = __TEST__ "dirfile/format";
  int error, r = 0;
  DIRFILE *D;

  mkdir(filedir, 0777);
  close(open(format, O_CREAT | O_EXCL | O_WRONLY, 0666));

  D = gd_open(filedir, GD_RDONLY | GD_VERBOSE);
  error = gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(error, 0);

  return r;
}
