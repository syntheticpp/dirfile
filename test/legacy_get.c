/* Attempt to read UINT8 via the legacy interface */
#include "../src/getdata.h"


#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(void)
{
#ifndef GETDATA_LEGACY_API
  return 77; /* skipped */
#else
  const char* filedir = __FILE__ "dirfile";
  const char* format = __FILE__ "dirfile/format";
  const char* data = __FILE__ "dirfile/data";
  const char* format_data = "data RAW UINT8 8\n";
  unsigned char c[8];
  unsigned char data_data[256];
  int fd, i;

  memset(c, 0, 8);
  mkdir(filedir, 0777);

  for (fd = 0; fd < 256; ++fd)
    data_data[fd] = fd;

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  fd = open(data, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, data_data, 256);
  close(fd);

  int error;
  int n = GetData(filedir, "data", 5, 0, 1, 0, 'c', c, &error);

  if (error)
    return 1;
  if (n != 8)
    return 1;
  for (i = 0; i < 8; ++i)
    if (c[i] != 40 + i)
      return 1;

  unlink(data);
  unlink(format);
  rmdir(filedir);

  return 0;
#endif
}