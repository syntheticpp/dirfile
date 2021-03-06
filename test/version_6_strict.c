/* Copyright (C) 2010-2011 D. V. Wiebe
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
/* Check Standards Version 6 strictness */
#include "test.h"

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int cb(gd_parser_data_t *pdata, void *ll)
{
  ((int*)ll)[pdata->linenum - 1] = 1;
  return GD_SYNTAX_IGNORE;
}

int main(void)
{
  const char *filedir = "dirfile";
  const char *format = "dirfile/format";
  const char *data = "dirfile/ar";
  const char *format_data =
    "/VERSION 6\n"
    "X<r RAW UINT8 8\n"
    "X#r RAW COMPLEX128 8\n"
    "X.r RAW UINT8 8\n"
    "Xr POLYNOM INDEX 8 3 1 2\n"
    "/ENCODING none\n"
    "/REFERENCE ar\n"
    "/PROTECT none\n"
    "c CONST UINT8 3\n"
    "ar RAW UINT8 8\n";
  uint16_t c[8];
  int ll[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint16_t d[8];
  unsigned char data_data[256];
  int fd, n, error, m, error2, i, r = 0;
  DIRFILE *D;

  memset(c, 0, 8);
  rmdirfile();
  mkdir(filedir, 0777);

  for (fd = 0; fd < 256; ++fd)
    data_data[fd] = (unsigned char)fd;

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  fd = open(data, O_CREAT | O_EXCL | O_WRONLY | O_BINARY, 0666);
  write(fd, data_data, 256);
  close(fd);

  D = gd_cbopen(filedir, GD_RDONLY | GD_PEDANTIC, cb, ll);
  n = gd_getdata(D, "ar", 5, 0, 1, 0, GD_UINT16, c);
  error = gd_error(D);

  m = gd_getdata(D, "FILEFRAM", 5, 0, 8, 0, GD_UINT16, d);
  error2 = gd_error(D);

  gd_close(D);

  unlink(data);
  unlink(format);
  rmdir(filedir);

  CHECKI(error,0);
  CHECKI(error2, GD_E_BAD_CODE);

  for (i = 0; i < 10; ++i) {
    if (i == 1 || i == 2 || i == 3 || i == 4) {
      CHECKIi(i,!ll[i],0);
    } else if (i < 1 || i > 4) {
      CHECKIi(i,ll[i],0);
    }
  }

  CHECKI(n,8);

  for (i = 0; i < 8; ++i)
    CHECKIi(i,c[i],40 + i);

  CHECKI(m,0);

  return r;
}
