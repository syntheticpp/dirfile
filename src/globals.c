/* Copyright (C) 2008-2012 D. V. Wiebe
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

/* This is nothing other than what the caller gave us.  Presumably it should
 * be better at keeping track of such things than us, but this is present in
 * the event that it is not. */
const char *gd_dirfilename(DIRFILE* D) gd_nothrow
{
  dtrace("%p", D);

  if (D->flags & GD_INVALID) {/* don't crash */
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  dreturn("\"%s\"", D->name);
  return D->name;
}

const char *gd_reference(DIRFILE* D, const char* field_code) gd_nothrow
{
  gd_entry_t *E;
  char *ptr;

  dtrace("%p, \"%s\"", D, field_code);

  if (D->flags & GD_INVALID) {/* don't crash */
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);
  
  /* if no field specified, return only the field name */
  if (field_code == NULL) {
    if (D->reference_field == NULL) {
      dreturn("%p", NULL);
      return NULL;
    }

    dreturn("\"%s\"", D->reference_field->field);
    return D->reference_field->field;
  }

  /* check access mode */
  if ((D->flags & GD_ACCMODE) == GD_RDONLY) {
    _GD_SetError(D, GD_E_ACCMODE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  /* Check field */
  E = _GD_FindField(D, field_code, D->entry, D->n_entries, 1, NULL);

  if (E == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, GD_E_CODE_MISSING, NULL, 0, field_code);
    dreturn("%p", NULL);
    return NULL;
  }

  if (E->field_type != GD_RAW_ENTRY) {
    _GD_SetError(D, GD_E_BAD_FIELD_TYPE, GD_E_FIELD_BAD, NULL, 0, field_code);
    dreturn("%p", NULL);
    return NULL;
  }

  /* Check protection */
  if (D->fragment[0].protection & GD_PROTECT_FORMAT) {
    _GD_SetError(D, GD_E_PROTECTED, GD_E_PROTECTED_FORMAT, NULL, 0,
        D->fragment[0].cname);
    dreturn("%p", NULL);
    return NULL;
  }
  
  ptr = _GD_Strdup(D, E->field);

  if (ptr == NULL) {
    dreturn("%p", NULL);
    return NULL;
  }

  /* set the new reference field */
  D->reference_field = E;
  free(D->fragment[0].ref_name);
  D->fragment[0].ref_name = ptr;
  D->fragment[0].modified = 1;
  D->flags &= ~GD_HAVE_VERSION;

  dreturn("\"%s\"", D->reference_field->field);
  return D->reference_field->field;
}

/* the mask of allowed flags */
#define GD_FLAG_MASK ( GD_VERBOSE | GD_PRETTY_PRINT)
unsigned long gd_flags(DIRFILE *D, unsigned long set, unsigned long reset)
  gd_nothrow
{
  dtrace("%p, 0x%lX, 0x%lX", D, set, reset);

  _GD_ClearError(D);

  set &= GD_FLAG_MASK;
  reset &= GD_FLAG_MASK;

  D->flags = (D->flags | set) & ~(D->flags & reset);
  D->open_flags = (D->open_flags | set) & ~(D->open_flags & reset);

  dreturn("0x%lX", D->flags & GD_FLAG_MASK);
  return D->flags & GD_FLAG_MASK;
}

int gd_verbose_prefix(DIRFILE *D, const char *prefix) gd_nothrow
{
  char *ptr = NULL;
  dtrace("%p, \"%s\"", D, prefix);

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%i", -1);
    return -1;
  }

  _GD_ClearError(D);

  if (prefix) {
    ptr = _GD_Strdup(D, prefix);
    if (D->error) {
      dreturn("%i", -1);
      return -1;
    }
  }

  free(D->error_prefix);
  D->error_prefix = ptr;

  dreturn("%i", 0);
  return 0;
}

void gd_mplex_lookback(DIRFILE *D, int lookback) gd_nothrow
{
  dtrace("%p, %i", D, lookback);

  _GD_ClearError(D);

  D->lookback = lookback;

  dreturnvoid();
}
