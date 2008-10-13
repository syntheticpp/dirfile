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
#include <stdlib.h>
#include <string.h>
#endif

const void* get_metaconstant_values(DIRFILE* D, const char* parent,
    gd_type_t return_type)
{
  dtrace("%p, \"%s\", 0x%x", D, parent, return_type);

  int i, n;
  void* fl;

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  const gd_entry_t* P = _GD_GetEntry(D, parent, NULL);

  if (P == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, 0, NULL, 0, parent);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  struct _gd_private_entry* e = P->e;

  if (e->n_meta_const == 0) {
    dreturn("%p", NULL);
    return NULL;
  }

  free(e->const_value_list);
  fl = _GD_Alloc(D, return_type, e->n_meta_const);

  if (fl == NULL) {
    dreturn("%p", NULL);
    return NULL;
  }

  for (i = n = 0; i < e->n_meta; ++i) {
    if (e->meta_entry[i]->field_type == GD_CONST_ENTRY)
      if (_GD_DoField(D, e->meta_entry[i], e->meta_entry[i]->field, 0, 0, 0, 0,
            return_type, fl + n++ * GD_SIZE(return_type)) != 1)
        break;
  }

  e->const_value_list = fl;

  dreturn("%p", e->const_value_list);
  return e->const_value_list;
}

const char** get_metaconstant_list(DIRFILE* D, const char* parent)
{
  dtrace("%p, \"%s\"", D, parent);

  int i, n;
  char** fl;

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  const gd_entry_t* P = _GD_GetEntry(D, parent, NULL);

  if (P == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, 0, NULL, 0, parent);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  struct _gd_private_entry* e = P->e;

  size_t offs = strlen(P->field) + 1;

  if (e->n_meta_const == 0) {
    dreturn("%p", NULL);
    return NULL;
  }

  fl = realloc((char**)e->const_list, sizeof(const char*) *
      (e->n_meta_const + 1));

  if (fl == NULL) {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  for (i = n = 0; i < e->n_meta; ++i) {
    if (e->meta_entry[i]->field_type == GD_CONST_ENTRY)
      fl[n++] = e->meta_entry[i]->field + offs;
  }
  fl[n] = NULL;

  e->const_list = (const char**)fl;

  dreturn("%p", e->const_list);
  return e->const_list;
}

const char** get_metastring_values(DIRFILE* D, const char* parent)
{
  dtrace("%p, \"%s\"", D, parent);

  int i, n;
  char** fl;

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  const gd_entry_t* P = _GD_GetEntry(D, parent, NULL);

  if (P == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, 0, NULL, 0, parent);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  struct _gd_private_entry* e = P->e;

  if (e->n_meta_string == 0) {
    dreturn("%p", NULL);
    return NULL;
  }

  fl = realloc((char**)e->string_value_list, sizeof(const char*) *
      (e->n_meta_string + 1));

  if (fl == NULL) {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  for (i = n = 0; i < e->n_meta; ++i) {
    if (e->meta_entry[i]->field_type == GD_STRING_ENTRY)
      fl[n++] = e->meta_entry[i]->e->string;
  }
  fl[i] = NULL;

  e->string_value_list = (const char**)fl;

  dreturn("%p", e->string_value_list);
  return e->string_value_list;
}

const char** get_metastring_list(DIRFILE* D, const char* parent)
{
  dtrace("%p, \"%s\"", D, parent);

  int i, n;
  char** fl;

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  const gd_entry_t* P = _GD_GetEntry(D, parent, NULL);

  if (P == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, 0, NULL, 0, parent);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  struct _gd_private_entry* e = P->e;

  size_t offs = strlen(P->field) + 1;

  if (e->n_meta_string == 0) {
    dreturn("%p", NULL);
    return NULL;
  }

  fl = realloc((char**)e->string_list, sizeof(const char*) *
      (e->n_meta_string + 1));

  if (fl == NULL) {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  for (i = n = 0; i < e->n_meta; ++i) {
    if (e->meta_entry[i]->field_type == GD_STRING_ENTRY)
      fl[n++] = e->meta_entry[i]->field + offs;
  }
  fl[n] = NULL;

  e->string_list = (const char**)fl;

  dreturn("%p", e->string_list);
  return e->string_list;
}

const char** get_metavector_list(DIRFILE* D, const char* parent)
{
  dtrace("%p, \"%s\"", D, parent);

  int i, n;
  char** fl;

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  const gd_entry_t* P = _GD_GetEntry(D, parent, NULL);

  if (P == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, 0, NULL, 0, parent);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  struct _gd_private_entry* e = P->e;

  size_t offs = strlen(P->field) + 1;

  if (e->n_meta == 0) {
    dreturn("%p", NULL);
    return NULL;
  }

  fl = realloc((char**)e->vector_list, sizeof(const char*) *
      (e->n_meta + 1 - e->n_meta_string - e->n_meta_const));

  if (fl == NULL) {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  for (i = n = 0; i < e->n_meta; ++i) {
    if (!(e->meta_entry[i]->field_type & GD_SCALAR_ENTRY))
      fl[n++] = e->meta_entry[i]->field + offs;
  }
  fl[n] = NULL;

  e->vector_list = (const char**)fl;

  dreturn("%p", e->vector_list);
  return e->vector_list;
}

const char** get_metafield_list(DIRFILE* D, const char* parent)
{
  dtrace("%p, \"%s\"", D, parent);

  int i, n;
  char** fl;

  if (D->flags & GD_INVALID) {
    _GD_SetError(D, GD_E_BAD_DIRFILE, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  const gd_entry_t* P = _GD_GetEntry(D, parent, NULL);

  if (P == NULL) {
    _GD_SetError(D, GD_E_BAD_CODE, 0, NULL, 0, parent);
    dreturn("%p", NULL);
    return NULL;
  }

  _GD_ClearError(D);

  struct _gd_private_entry* e = P->e;

  size_t offs = strlen(P->field) + 1;

  if (e->n_meta == 0) {
    dreturn("%p", NULL);
    return NULL;
  }

  fl = realloc((char**)e->field_list, sizeof(const char*) *
      (e->n_meta + 1));

  if (fl == NULL) {
    _GD_SetError(D, GD_E_ALLOC, 0, NULL, 0, NULL);
    dreturn("%p", NULL);
    return NULL;
  }

  for (i = n = 0; i < e->n_meta; ++i)
    fl[n++] = e->meta_entry[i]->field + offs;
  fl[n] = NULL;

  e->field_list = (const char**)fl;

  dreturn("%p", e->field_list);
  return e->field_list;
}