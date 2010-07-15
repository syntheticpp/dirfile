// (C) 2010 D. V. Wiebe
//
///////////////////////////////////////////////////////////////////////////
//
// This file is part of the GetData project.
//
// GetData is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; either version 2.1 of the License, or (at your
// option) any later version.
//
// GetData is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with GetData; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
#ifdef HAVE_CONFIG_H
#include "../../src/config.h"
#endif
#undef GETDATA_LEGACY_API
#include "getdata/dirfile.h"

#include <stdlib.h>
#include <cstring>

using namespace GetData;

RecipEntry::RecipEntry(const char* field_code, const char* in_field1,
    double dividend, int fragment_index) : Entry::Entry()
{
  E.field = strdup(field_code);
  E.field_type = GD_RECIP_ENTRY;
  E.in_fields[0] = strdup(in_field1);
  E.scalar[0] = 0;
  E.cdividend[0] = E.dividend = dividend;
  E.cdividend[1] = 0;
  E.comp_scal = 0;
  E.fragment_index = fragment_index;
}

RecipEntry::RecipEntry(const char* field_code, const char* in_field1,
    std::complex<double> dividend, int fragment_index) : Entry::Entry()
{
  E.field = strdup(field_code);
  E.field_type = GD_RECIP_ENTRY;
  E.in_fields[0] = strdup(in_field1);
  E.scalar[0] = 0;
  E.cdividend[0] = E.dividend = dividend.real();
  E.cdividend[1] = dividend.imag();
  E.comp_scal = 1;
  E.fragment_index = fragment_index;
}

int RecipEntry::SetInput(const char* field)
{
  char* ptr = strdup(field);

  if (ptr == NULL)
    return -1;

  free(E.in_fields[0]);
  E.in_fields[0] = ptr;

  if (D != NULL)
    return gd_alter_entry(D->D, E.field, &E, 0);
  
  return 0;
}

int RecipEntry::SetDividend(double dividend)
{
  E.cdividend[0] = E.dividend = dividend;
  E.cdividend[1] = 0;

  if (D != NULL)
    return gd_alter_entry(D->D, E.field, &E, 0);
  
  return 0;
}

int RecipEntry::SetDividend(const char *scale)
{
  int r = 0;

  free(E.scalar[0]);

  if (scale == NULL)
    E.scalar[0] = NULL;
  else
    E.scalar[0] = strdup(scale);

  if (D != NULL) {
    r = gd_alter_entry(D->D, E.field, &E, 0);

    if (!r) {
      r = gd_get_constant(D->D, scale, GD_COMPLEX128, &E.cdividend);
      E.dividend = E.cdividend[0];
    }
  }
  
  return r;
}

int RecipEntry::SetDividend(std::complex<double> dividend)
{
  E.dividend = E.cdividend[0] = dividend.real();
  E.cdividend[1] = dividend.imag();
  E.comp_scal = 1;

  if (D != NULL)
    return gd_alter_entry(D->D, E.field, &E, 0);
  
  return 0;
}
