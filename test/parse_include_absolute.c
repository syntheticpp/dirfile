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
/* Test include */
#include "test.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(void)
{
#if defined HAVE_GETCWD || defined HAVE__GETCWD
#ifdef HAVE__GETCWD
#define getcwd _getcwd
#endif
  const char *filedir = "dirfile";
  const char *format = "dirfile/format";
  const char *format1 = "dirfile/format1";
  const char *format_data1 = "INCLUDE ";
  const char *format_data2 = "/dirfile/format1\n";
  const char *format1_data = "data RAW UINT8 11\n";
  int cwd_size = 2048;
  char *ptr, *cwd = NULL;
  int fd, r = 0;
  DIRFILE *D;
  unsigned int spf;

  rmdirfile();
  mkdir(filedir, 0777);

  do {
    ptr = (char*)realloc(cwd, cwd_size *= 2);
    if (ptr == NULL) {
      fprintf(stderr, "out of memory for cwd!\n");
      exit(1);
    }
  } while (!getcwd(cwd = ptr, cwd_size));

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data1, strlen(format_data1));
  gd_pathwrite(fd, cwd);
  write(fd, format_data2, strlen(format_data2));
  close(fd);

  fd = open(format1, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format1_data, strlen(format1_data));
  close(fd);

  D = gd_open(filedir, GD_RDONLY | GD_VERBOSE);
  spf = gd_spf(D, "data");
  gd_close(D);

  unlink(format1);
  unlink(format);
  rmdir(filedir);

  CHECKU(spf, 11);
  free(cwd);
  return r;
#else
  return 77;
#endif
}
