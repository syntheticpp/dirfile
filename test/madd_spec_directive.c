/* Add a RAW field */
#include "test.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(void)
{
  const char* filedir = __TEST__ "dirfile";
  const char* format = __TEST__ "dirfile/format";
  int r = 0;

  DIRFILE* D = gd_open(filedir, GD_RDWR | GD_CREAT);
  gd_madd_spec(D, "META INDEX RAW UINT8 2", "INDEX");
  int error = gd_error(D);

  /* check */
  int n = gd_nfields(D);
  int m = gd_nmfields(D, "INDEX");

  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(n, 1);
  CHECKI(m, 0);
  CHECKI(error, GD_E_FORMAT);

  return r;
}
