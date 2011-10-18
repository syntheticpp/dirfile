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
#include "internal.h"

#ifdef STDC_HEADERS
#include <string.h>
#include <stdlib.h>
#endif

off64_t _GD_GetFilePos(DIRFILE *D, gd_entry_t *E, off64_t index_pos)
{
  int i;
  off64_t pos = -1, pos2;

  dtrace("%p, %p, %lli", D, E, (long long)index_pos);

  if (++D->recurse_level >= GD_MAX_RECURSE_LEVEL) {
    _GD_SetError(D, GD_E_RECURSE_LEVEL, 0, NULL, 0, E->field);
    dreturn("%u", 0);
    D->recurse_level--;
    return 0;
  }

  switch (E->field_type) {
    case GD_RAW_ENTRY:
      /* We must open the file to know its starting offset */
      if (_GD_InitRawIO(D, E, 0, 0))
          break;
      pos = E->e->u.raw.file[0].pos +
        D->fragment[E->fragment_index].frame_offset;
      break;
    case GD_LINCOM_ENTRY:
      if (_GD_BadInput(D, E, 0))
        break;

      pos = _GD_GetFilePos(D, E->e->entry[0], -1);
      if (!D->error)
        for (i = 1; i < E->EN(lincom,n_fields); ++i) {
          if (_GD_BadInput(D, E, i)) {
            pos = -1;
            break;
          }
          pos2 = _GD_GetFilePos(D, E->e->entry[i], pos);
          if (pos2 != pos) {
            _GD_SetError(D, GD_E_DOMAIN, GD_E_DOMAIN_MULTIPOS, NULL, 0, NULL);
            pos = -1;
            break;
          }
        }
      break;
    case GD_LINTERP_ENTRY:
    case GD_BIT_ENTRY:
    case GD_POLYNOM_ENTRY:
    case GD_SBIT_ENTRY:
    case GD_RECIP_ENTRY:
      if (_GD_BadInput(D, E, 0))
          break;
      pos = _GD_GetFilePos(D, E->e->entry[0], -1);
      break;
    case GD_MULTIPLY_ENTRY:
    case GD_DIVIDE_ENTRY:
      if (_GD_BadInput(D, E, 0) || _GD_BadInput(D, E, 1))
          break;
      pos = _GD_GetFilePos(D, E->e->entry[0], 0);
      pos2 = _GD_GetFilePos(D, E->e->entry[0], pos);
      if (!D->error && pos != pos2) {
        _GD_SetError(D, GD_E_DOMAIN, GD_E_DOMAIN_MULTIPOS, NULL, 0, NULL);
        pos = -1;
      }
      break;
    case GD_PHASE_ENTRY:
      if (_GD_BadInput(D, E, 0))
        break;
      pos = _GD_GetFilePos(D, E->e->entry[0], -1);
      if (pos >= 0)
        pos += E->EN(phase,shift);
      break;
    case GD_INDEX_ENTRY:
      if (index_pos == -1)
        pos = E->e->u.index_pos;
      else
        pos = index_pos;
      break;
    case GD_NO_ENTRY:
    case GD_CONST_ENTRY:
    case GD_STRING_ENTRY:
    case GD_CARRAY_ENTRY:
      _GD_InternalError(D);
  }

  D->recurse_level--;
  dreturn("%lli", (long long)pos);
  return pos;
}

/* Get the current I/O position of the given field
*/
off64_t gd_tell64(DIRFILE *D, const char *field_code_in)
{
  off64_t pos = -1;
  gd_entry_t* entry;
  char* field_code;
  int repr;

  dtrace("%p, \"%s\"", D, field_code_in);

  if (D->flags & GD_INVALID) {/* don't crash */
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%u", 0);
    return 0;
  }

  _GD_ClearError(D);

  entry = _GD_FindFieldAndRepr(D, field_code_in, &field_code, &repr, NULL, 1);

  if (D->error) {
    dreturn("%u", 0);
    return 0;
  }

  if (entry->field_type & GD_SCALAR_ENTRY)
    _GD_SetError(D, GD_E_DIMENSION, GD_E_DIM_CALLER, NULL, 0, field_code);
  else 
    pos = _GD_GetFilePos(D, entry, -1);

  if (field_code != field_code_in)
    free(field_code);

  dreturn("%lli", (long long)pos);
  return pos;
}

off_t gd_tell(DIRFILE *D, const char *field_code)
{
  return (off_t)gd_tell64(D, field_code);
}

static int _GD_Seek(DIRFILE *D, gd_entry_t *E, off64_t offset, int pad)
{
  int i;

  dtrace("%p, %p, %lli, %i", D, E, (long long)offset, pad);

  if (++D->recurse_level >= GD_MAX_RECURSE_LEVEL) {
    _GD_SetError(D, GD_E_RECURSE_LEVEL, 0, NULL, 0, E->field);
    D->recurse_level--;
    dreturn("%i", 1);
    return 1;
  }

  switch (E->field_type) {
    case GD_RAW_ENTRY:
      /* open/create the file, if necessary */
      if (_GD_InitRawIO(D, E, GD_EF_SEEK, pad))
        break;

      if ((*_gd_ef[E->e->u.raw.file[0].subenc].seek)(E->e->u.raw.file, offset,
            E->EN(raw,data_type), pad) == -1)
      {
        _GD_SetError(D, GD_E_RAW_IO, 0, E->e->u.raw.file[0].name, errno, NULL);
      }
      break;
    case GD_LINCOM_ENTRY:
      for (i = 0; i < E->EN(lincom,n_fields); ++i)
        if (!_GD_BadInput(D, E, i))
          _GD_Seek(D, E->e->entry[i], offset, pad);
        else
          break;
      break;
    case GD_MULTIPLY_ENTRY:
    case GD_DIVIDE_ENTRY:
      if (_GD_BadInput(D, E, 1))
        break;
      if (_GD_Seek(D, E->e->entry[1], offset, pad))
        break;
      /* fallthrough */
    case GD_LINTERP_ENTRY:
    case GD_BIT_ENTRY:
    case GD_POLYNOM_ENTRY:
    case GD_SBIT_ENTRY:
    case GD_RECIP_ENTRY:
      if (!_GD_BadInput(D, E, 0))
        _GD_Seek(D, E->e->entry[0], offset, pad);
      break;
    case GD_PHASE_ENTRY:
      if (!_GD_BadInput(D, E, 0))
        _GD_Seek(D, E->e->entry[0], offset + E->EN(phase,shift), pad);
      break;
    case GD_INDEX_ENTRY:
      E->e->u.index_pos = offset;
      break;
    case GD_NO_ENTRY:
    case GD_CONST_ENTRY:
    case GD_STRING_ENTRY:
    case GD_CARRAY_ENTRY:
      _GD_InternalError(D);
  }

  D->recurse_level--;
  dreturn("%i", (D->error == 0) ? 0 : 1);
  return (D->error == 0) ? 0 : 1;
}

/* Set the I/O position of the given field
 */
off64_t gd_seek64(DIRFILE *D, const char *field_code_in, off64_t frame_num,
    off64_t sample_num, int whence)
{
  gd_spf_t spf = 0;
  off64_t pos = 0;
  gd_entry_t* entry;
  char* field_code;
  int repr, is_index = 0, pad = (whence & GD_SEEK_PAD) ? 1 : 0;

  dtrace("%p, \"%s\", %lli, %lli, %x", D, field_code_in, frame_num, sample_num,
      whence);

  if (D->flags & GD_INVALID) {/* don't crash */
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%u", 0);
    return 0;
  }

  _GD_ClearError(D);

  entry = _GD_FindFieldAndRepr(D, field_code_in, &field_code, &repr, NULL, 1);

  if (D->error) {
    dreturn("%u", -1);
    return -1;
  }

  if (entry->field_type & GD_SCALAR_ENTRY) {
    _GD_SetError(D, GD_E_DIMENSION, GD_E_DIM_CALLER, NULL, 0, field_code);
    if (field_code != field_code_in)
      free(field_code);
    dreturn("%u", -1);
    return -1;
  }

  if (frame_num) {
    spf = _GD_GetSPF(D, entry);

    if (D->error) {
      if (field_code != field_code_in)
        free(field_code);
      dreturn("%u", -1);
      return -1;
    }
  }

  sample_num += frame_num * spf;

  whence &= (GD_SEEK_SET | GD_SEEK_CUR | GD_SEEK_END);
  if (whence == GD_SEEK_SET)
    pos = 0;
  else if (whence == GD_SEEK_CUR)
    pos = _GD_GetFilePos(D, entry, 1);
  else if (whence == GD_SEEK_END) {
    pos = _GD_GetEOF(D, entry, NULL, &is_index);
    if (is_index)
      _GD_SetError(D, GD_E_BAD_FIELD_TYPE, GD_E_FIELD_BAD, NULL, 0, field_code);
  } else
    _GD_SetError(D, GD_E_ARGUMENT, GD_E_ARG_WHENCE, NULL, 0, NULL);

  if (!D->error)
    _GD_Seek(D, entry, sample_num + pos, pad);

  if (field_code != field_code_in)
    free(field_code);

  if (D->error)
    pos = -1;
  else
    pos = _GD_GetFilePos(D, entry, 1);

  dreturn("%lli", (long long)pos);
  return pos;
}

off_t gd_seek(DIRFILE *D, const char *field_code, off_t frame_num,
    off_t sample_num, int whence)
{
  return (off_t)gd_seek64(D, field_code, frame_num, sample_num, whence);
}
/* vim: ts=2 sw=2 et tw=80
*/