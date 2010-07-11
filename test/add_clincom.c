/* Add a complex LINCOM field */
#include "test.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
  const char* filedir = __TEST__ "dirfile";
  const char* format = __TEST__ "dirfile/format";

  DIRFILE* D = gd_open(filedir, GD_RDWR | GD_CREAT | GD_VERBOSE);
  const char* in_fields[2] = {"in1", "in2"};
  const double complex m[2] = {1 + _Complex_I * 3.3, 0.3 + _Complex_I * 18.3};
  const double complex b[2] = {2 + _Complex_I * 3.8, 2.1 + _Complex_I * 9.8};
  int r = 0;
  gd_entry_t e;

  gd_add_clincom(D, "new", 2, in_fields, m, b, 0);
  int error = gd_error(D);

  /* check */
  gd_entry(D, "new", &e);
  if (gd_error(D))
    r = 1;
  else {
    CHECKI(e.field_type, GD_LINCOM_ENTRY);
    CHECKI(e.fragment_index, 0);
    CHECKI(e.n_fields,2);
    CHECKS(e.in_fields[0], "in1");
    CHECKS(e.in_fields[1], "in2");
    CHECKC(e.cm[0], m[0]);
    CHECKC(e.cm[1], m[1]);
    CHECKC(e.cb[0], b[0]);
    CHECKC(e.cb[1], b[1]);
    CHECKI(e.comp_scal,1);
    gd_free_entry_strings(&e);
  }

  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(error, GD_E_OK);

  return r;
}
