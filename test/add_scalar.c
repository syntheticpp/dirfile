/* Add a dirfile field */
#include "test.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <math.h>

int main(void)
{
  const char* filedir = __TEST__ "dirfile";
  const char* format = __TEST__ "dirfile/format";
  int error, r = 0;
  DIRFILE *D;

  gd_entry_t E, e;
  E.field = "data";
  E.field_type = GD_LINCOM_ENTRY;
  E.fragment_index = 0;
  E.EN(lincom,n_fields) = 1;
  E.comp_scal = 0;
  E.in_fields[0] = "INDEX";
  E.EN(lincom,m)[0] = 1.;
  E.scalar[0] = NULL;
  E.scalar[0 + GD_MAX_LINCOM] = "c";

  D = gd_open(filedir, GD_RDWR | GD_CREAT | GD_VERBOSE);
  gd_add_spec(D, "c CONST INT64 4", 0);
  gd_add(D, &E);
  error = gd_error(D);

  /* check */
  gd_entry(D, "data", &e);
  if (gd_error(D))
    r = 1;
  else {
    CHECKI(e.field_type, GD_LINCOM_ENTRY);
    CHECKI(e.fragment_index, 0);
    CHECKI(e.EN(lincom,n_fields), 1);
    CHECKF(e.EN(lincom,m)[0], 1);
    CHECKF(e.EN(lincom,b)[0], 4);
    CHECKP(e.scalar[0]);
    CHECKS(e.scalar[0 + GD_MAX_LINCOM], "c");
    gd_free_entry_strings(&e);
  }

  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(error, GD_E_OK);

  return r;
}
