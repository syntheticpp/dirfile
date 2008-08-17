/* (C) 2002-2005 C. Barth Netterfield
 * (C) 2005-2008 D. V. Wiebe
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
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */
#ifndef GETDATA_INTERNAL_H
#define GETDATA_INTERNAL_H

#include "getdata.h"

/* Type conventions:
 *
 *  - samples per frame is always unsigned int
 *  - variables holding offsets or file sizes should be of type off64_t (which
 *    may be simply off_t, depending on local LFS support)
 *  - variables holding object sizes or counts of items read or written should
 *    be of type size_t
 *  - public functions taking or returning types of off64_t should have both
 *    a off_t prototype and and off64_t type prototype.
 */

/* if we don't have off64_t, we probably don't have the rest of the transitional
 * LFS API
 */
#ifndef HAVE_OFF64_T
typedef off_t off64_t
# define lseek64 lseek
# define stat64 stat
#endif

#ifndef __attribute_malloc__
# define __attribute_malloc__
#endif

#ifndef __wur
# define __wur
#endif

#ifndef NDEBUG
#include <assert.h>
#else
#define assert(...)
#endif

/* maximum number of recursions */
#define GD_MAX_RECURSE_LEVEL  32

/* maximum length of a format file line */
#define MAX_LINE_LENGTH FILENAME_MAX

/* Suberror codes */
#define GD_E_OPEN_NOT_DIRFILE  0
#define GD_E_OPEN_NOT_EXIST    1
#define GD_E_OPEN_NO_ACCESS    3

#define GD_E_TRUNC_DIR         0
#define GD_E_TRUNC_STAT        1
#define GD_E_TRUNC_UNLINK      2

#define GD_E_CREAT_DIR         0
#define GD_E_CREAT_FORMAT      1
#define GD_E_CREAT_EXCL        2

#define GD_E_FORMAT_BAD_TYPE   0
#define GD_E_FORMAT_BAD_SPF    1
#define GD_E_FORMAT_N_FIELDS   2
#define GD_E_FORMAT_N_COLS     3
#define GD_E_FORMAT_MAX_I      4
#define GD_E_FORMAT_NUMBITS    5
#define GD_E_FORMAT_BITNUM     6
#define GD_E_FORMAT_BITSIZE    7
#define GD_E_FORMAT_BAD_LINE   9
#define GD_E_FORMAT_N_RAW     10
#define GD_E_FORMAT_RES_NAME  11
#define GD_E_FORMAT_ENDIAN    12

#define GD_E_LINFILE_OPEN      0
#define GD_E_LINFILE_LENGTH    1

struct RawEntryType {
  char* field;
  int field_type;
  char* file;
  gd_type_t data_type;
  int fp;
  size_t size;
  int samples_per_frame;
};

struct LincomEntryType {
  char* field;
  int field_type;
  int n_infields;
  char *in_fields[GD_MAX_LINCOM];
  double m[GD_MAX_LINCOM];
  double b[GD_MAX_LINCOM];
};

struct LinterpEntryType {
  char* field;
  int field_type;
  char *raw_field;
  char *linterp_file;
  int n_interp;
  double *x;
  double *y;
};

struct MultiplyEntryType {
  char* field;
  int field_type;
  char *in_fields[2];
};

struct BitEntryType {
  char* field;
  int field_type;
  char *raw_field;
  int bitnum;
  int numbits;
};

struct PhaseEntryType {
  char* field;
  int field_type;
  char *raw_field;
  int shift;
};

void* _GD_Alloc(DIRFILE* D, gd_type_t type, int n) __nonnull ((1))
  __attribute_malloc__ __THROW __wur;
void _GD_ClearGetDataError(DIRFILE* D) __nonnull ((1)) __THROW;
void _GD_ConvertType(DIRFILE* D, const void *data_in, gd_type_t in_type,
    void *data_out, gd_type_t out_type, size_t n) __nonnull ((1, 2, 4)) __THROW;
size_t  _GD_DoField(DIRFILE *D, const char *field_code, off64_t first_frame,
    off64_t first_samp, size_t num_frames, size_t num_samp,
    gd_type_t return_type, void *data_out) __nonnull ((1, 2));
void _GD_FixEndianness(DIRFILE* D, char* databuffer, size_t size, size_t ns)
  __nonnull ((1, 2));
int _GD_GetLine(FILE *fp, char *line, int* linenum) __nonnull((1, 2, 3));
unsigned int  _GD_GetSPF(const char *field_code, DIRFILE* D) __nonnull ((1, 2));
struct gd_entry_t* _GD_FindField(DIRFILE* D, const char* field_code)
  __nonnull ((1, 2)) __THROW;
gd_type_t _GD_LegacyType(char c) __THROW __attribute__ ((__const__));
void _GD_LinterpData(DIRFILE* D, const void *data, gd_type_t type, int npts,
    double *lx, double *ly, int n_ln) __nonnull ((1, 2, 5, 6)) __THROW;
void _GD_ReadLinterpFile(DIRFILE* D, struct LinterpEntryType *E)
  __nonnull ((1, 2));
void _GD_ScaleData(DIRFILE* D, void *data, gd_type_t type, int npts, double m,
    double b) __nonnull ((1, 2)) __THROW;
void _GD_SetGetDataError(DIRFILE* D, int error, int suberror,
    const char* format_file, int line, const char* token) __nonnull ((1))
  __THROW;

/* typecast the generic EntryType to a particular field type */
#define ENTRY(a,b) ((struct a ## EntryType*)(b))

#endif
