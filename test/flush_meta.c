/* Attempt to flush meta data */
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
  const char* data = __TEST__ "dirfile/new";
  struct stat buf;
  int r = 0;

  DIRFILE* D = dirfile_open(filedir, GD_RDWR | GD_CREAT | GD_TRUNC |
      GD_VERBOSE);
  dirfile_add_raw(D, "new", GD_UINT8, 2, 0);
  dirfile_metaflush(D);
  int error = get_error(D);

  dirfile_close(D);

  if (stat(format, &buf))
    r =  1;

  unlink(data);
  unlink(format);
  rmdir(filedir);

  if (error || r)
    return 1;

  return 0;
}
