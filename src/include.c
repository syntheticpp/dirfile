/* (C) 2008 D. V. Wiebe
 *
 ***************************************************************************
 *
 * This file is part of the GetData project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GetData is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with GetData; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include "internal.h"

#ifdef STDC_HEADERS
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#ifdef HAVE_LIBGEN_H
#include <libgen.h>
#endif

/* Include a format file fragment -- returns the include index, or 
 * -1 on error */
int _GD_Include(DIRFILE* D, const char* ename, const char* format_file,
    int linenum, char** ref_name, int me, int* standards, int flags)
{
  int i;
  int found = 0;
  char temp_buf1[FILENAME_MAX];
  char temp_buf2[FILENAME_MAX];
  void* ptr;
  FILE* new_fp = NULL;

  dtrace("%p, \"%s\", \"%s\", %p, %i, %i, %p, %x\n", D, ename, format_file,
      ref_name, linenum, me, standards, flags);

  /* create the format filename */
  if (D->fragment[me].sname)
    snprintf(temp_buf1, FILENAME_MAX, "%s/%s/%s", D->name,
        D->fragment[me].sname, ename);
  else
    snprintf(temp_buf1, FILENAME_MAX, "%s/%s", D->name, ename);

  /* Run through the include list to see if we've already included this
   * file */
  for (i = 0; i < D->n_fragment; ++i)
    if (strcmp(temp_buf1, D->fragment[i].cname) == 0) {
      found = 1;
      break;
    }

  /* If we found the file, we won't reopen it.  Continue parsing. */
  if (found) {
    dreturn("%i", i);
    return i;
  }

  /* Otherwise, try to open the file */
  if ((D->flags & GD_ACCMODE) == GD_RDWR) {
    i = open(temp_buf1, O_RDWR | ((flags & GD_CREAT) ? O_CREAT : 0) |
        ((flags & GD_TRUNC) ? O_TRUNC : 0) | ((flags & GD_EXCL) ? O_EXCL : 0),
        0666);
    if (i < 0) {
      _GD_SetError(D, GD_E_OPEN_INCLUDE, errno, format_file, linenum,
          temp_buf1);
      dreturn("%i", -1);
      return -1;
    }
    new_fp = fdopen(i, "r+");
  } else
    new_fp = fopen(temp_buf1, "r");

  /* If opening the file failed, set the error code and abort parsing. */
  if (new_fp == NULL) {
    _GD_SetError(D, GD_E_OPEN_INCLUDE, errno, format_file, linenum, temp_buf1);
    dreturn("%i", -1);
    return -1;
  }

  /* If we got here, we managed to open the included file; parse it */
  ptr = realloc(D->fragment, (++D->n_fragment) * sizeof(struct gd_fragment_t));
  if (ptr == NULL) {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%i", -1);
    return -1;
  }
  D->fragment = ptr;

  D->fragment[D->n_fragment - 1].cname = strdup(temp_buf1);
  D->fragment[D->n_fragment - 1].ename = strdup(ename);
  D->fragment[D->n_fragment - 1].modified = 0;
  D->fragment[D->n_fragment - 1].parent = me;
  D->fragment[D->n_fragment - 1].encoding = flags & GD_ENCODING;
  D->fragment[D->n_fragment - 1].byte_sex =
#ifdef WORDS_BIGENDIAN
    (flags & GD_LITTLE_ENDIAN) ? GD_LITTLE_ENDIAN : GD_BIG_ENDIAN
#else
    (flags & GD_BIG_ENDIAN) ? GD_BIG_ENDIAN : GD_LITTLE_ENDIAN
#endif
    ;
  D->fragment[D->n_fragment - 1].ref_name = NULL;
  D->fragment[D->n_fragment - 1].frame_offset = D->fragment[me].frame_offset;
  D->fragment[D->n_fragment - 1].protection = GD_PROTECT_NONE;

  if (D->fragment[D->n_fragment - 1].cname == NULL ||
      D->fragment[D->n_fragment - 1].ename == NULL)
  {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%i", -1);
    return -1;
  }

  /* extract the subdirectory name - dirname both returns a volatile string
   * and modifies its argument, ergo strcpy */
  strncpy(temp_buf1, ename, FILENAME_MAX);
  if (D->fragment[me].sname)
    snprintf(temp_buf2, FILENAME_MAX, "%s/%s", D->fragment[me].sname,
        dirname(temp_buf1));
  else
    strcpy(temp_buf2, dirname(temp_buf1));

  if (temp_buf2[0] == '.' && temp_buf2[1] == '\0')
    D->fragment[D->n_fragment - 1].sname = NULL;
  else {
    D->fragment[D->n_fragment - 1].sname = strdup(temp_buf2);

    if (D->fragment[D->n_fragment - 1].sname == NULL) {
      _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
      dreturn("%i", -1);
      return -1;
    }
  }

  *ref_name = _GD_ParseFragment(new_fp, D, D->n_fragment - 1, standards, flags);

  fclose(new_fp);

  dreturn("%i", D->n_fragment - 1);
  return D->n_fragment - 1;
}

int dirfile_include(DIRFILE* D, const char* file, int fragment_index,
    unsigned long flags)
{
  int standards = DIRFILE_STANDARDS_VERSION;
  char* ref_name = NULL; 
  int i;

  dtrace("%p, \"%s\", %i, %lx", D, file, fragment_index, flags);

  if (D->flags & GD_INVALID) {/* don't crash */
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%zi", -1);
    return -1;
  }
  /* check access mode */
  if ((D->flags & GD_ACCMODE) == GD_RDONLY) {
    _GD_SetError(D, GD_E_ACCMODE, 0, NULL, 0, NULL);
    dreturn("%i", -1);
    return -1;
  }

  /* check for include index out of range */
  if (fragment_index < 0 || fragment_index >= D->n_fragment) {
    _GD_SetError(D, GD_E_BAD_INDEX, 0, NULL, fragment_index, NULL);
    dreturn("%i", -1);
    return -1;
  }

  /* check protection */
  if (D->fragment[fragment_index].protection & GD_PROTECT_FORMAT) {
    _GD_SetError(D, GD_E_PROTECTED, GD_E_PROTECTED_FORMAT, NULL, 0,
        D->fragment[fragment_index].cname);
    dreturn("%i", -1);
    return -1;
  }

  int new_fragment = _GD_Include(D, file, "dirfile_include()", 0, &ref_name,
      fragment_index, &standards, flags);

  if (!D->error)
    D->fragment[fragment_index].modified = 1;

  /* If ref_name is non-NULL, the included fragment contained a REFERENCE
   * directive.  If ref_name is NULL but D->fragment[new_fragment].ref_name is
   * non-NULL, no REFERENCE directive was present, but the parser found a RAW
   * field to serve as a reference field.
   */
  if (new_fragment >= 0 && D->fragment[new_fragment].ref_name != NULL)
    /* If the parser chose a reference field, propagate it upward if requried */
    for (i = fragment_index; i != -1; i = D->fragment[i].parent) {
      if (D->fragment[i].ref_name == NULL) {
        D->fragment[i].ref_name = strdup(D->fragment[new_fragment].ref_name);
        D->fragment[i].modified = 1;
      } else
        break;
    }

  /* Honour the reference directive, if not prohibited by the caller */
  if (ref_name != NULL && ~flags & GD_IGNORE_REFS) {
    gd_entry_t *E = _GD_FindField(D, ref_name, NULL);

    /* FIXME: These errors are problematic, since they'll cause the call to
     * fail, even though the new fragment has been integrated into the DIRFILE.
     */

    if (E == NULL)
      _GD_SetError(D, GD_E_BAD_REFERENCE, GD_E_REFERENCE_CODE, NULL, 0,
          ref_name);
    else if (E->field_type != GD_RAW_ENTRY)
      _GD_SetError(D, GD_E_BAD_REFERENCE, GD_E_REFERENCE_TYPE, NULL, 0,
          ref_name);
    else
      D->reference_field = E; 
    free(ref_name);
  }

  dreturn("%i", new_fragment);
  return new_fragment;
}
