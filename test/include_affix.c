/* Copyright (C) 2011 D. V. Wiebe
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
#include "test.h"

int main(void)
{
  const char *filedir = "dirfile";
  const char *format = "dirfile/format";
  const char *format1 = "dirfile/format1";
  const char *format_data = "#\n";
  const char *format1_data = "data RAW UINT8 11\n";
  int fd, error, r = 0;
  unsigned int spf;
  DIRFILE *D;

  rmdirfile();
  mkdir(filedir, 0777);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  fd = open(format1, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format1_data, strlen(format1_data));
  close(fd);

  D = gd_open(filedir, GD_RDWR | GD_VERBOSE);
  gd_include_affix(D, "format1", 0, "A", "Z", 0);
  error = gd_error(D);
  spf = gd_spf(D, "AdataZ");
  gd_close(D);

  unlink(format1);
  unlink(format);
  rmdir(filedir);

  CHECKI(error, 0);
  CHECKU(spf, 11);

  return r;
}
