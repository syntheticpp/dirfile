/* Copyright (C) 2008-2011 D. V. Wiebe
 *
 ***************************************************************************
 *
 * This file is part of the GetData project.
 *
 * GetData is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * GetData is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with GetData; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/* Add a dirfile field to a read-only dirfile*/
#include "test.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(void)
{
  const char *filedir = "dirfile";
  const char *format = "dirfile/format";
  const char *format_data = "data RAW UINT8 8\n";
  int fd, error, n, r = 0;
  DIRFILE *D;

  gd_entry_t E;
  E.field =  "new";
  E.field_type = GD_RAW_ENTRY;
  E.fragment_index = 0;
  E.EN(raw,spf) = 2;
  E.EN(raw,data_type) = GD_UINT8;

  rmdirfile();
  mkdir(filedir, 0777);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY | O_BINARY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  D = gd_open(filedir, GD_RDONLY);
  gd_add(D, &E);
  error = gd_error(D);

  /* check */
  n = gd_nfields(D);

  gd_close(D);

  unlink(format);
  rmdir(filedir);

  CHECKI(n, 2);
  CHECKI(error, GD_E_ACCMODE);

  return r;
}
