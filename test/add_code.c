/* Add a dirfile field with invalid characters */
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

  gd_entry_t E;
  E.field =  "ne/w";
  E.field_type = GD_RAW_ENTRY;
  E.fragment_index = 0;
  E.u.raw.spf = 2;
  E.u.raw.type = GD_UINT8;

  DIRFILE* D = gd_open(filedir, GD_RDWR | GD_CREAT);
  gd_add(D, &E);
  int error = gd_error(D);

  /* check */
  int n = gd_nfields(D);

  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(n, 1);
  CHECKI(error, GD_E_BAD_CODE);

  return r;
}
