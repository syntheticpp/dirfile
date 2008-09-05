/* Attempt to write PHASE */
#include "../src/getdata.h"

#include <inttypes.h>
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
  const char* format_data = "phase PHASE data -2\n";
  int8_t c[8];
  int fd, i;

  memset(c, 0, 8);
  mkdir(filedir, 0777);

  for (i = 0; i < 8; ++i)
    c[i] = (int8_t)(40 + i);

  fd = open(format, O_CREAT | O_EXCL | O_WRONLY, 0666);
  write(fd, format_data, strlen(format_data));
  close(fd);

  DIRFILE* D = dirfile_open(filedir, GD_RDWR);
  int n = putdata(D, "phase", 5, 0, 1, 0, GD_INT8, c);
  int error = D->error;

  dirfile_close(D);

  unlink(format);
  rmdir(filedir);

  if (n != 0)
    return 1;

  return (error != GD_E_BAD_CODE);
}
