// (C) 2008 D. V. Wiebe
//
///////////////////////////////////////////////////////////////////////////
//
// This file is part of the GetData project.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// GetData is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License
// along with GetData; if not, write to the Free Software Foundation,
// Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#ifndef GETDATA_MULTIPLYENTRY_H
#define GETDATA_MULTIPLYENTRY_H

#define NO_GETDATA_LEGACY_API

extern "C" {
#include <getdata.h>
}
#include <getdata/entry.h>

namespace GetData {

  class Dirfile;

  class MultiplyEntry : public Entry {
    friend class Dirfile;

    public:
      MultiplyEntry(const char* field_code, const char* in_field1,
          const char* in_field2, int fragment_index = 0);

      virtual const char *Input(int index = 0) {
        return E.in_fields[(index == 0) ? 0 : 1];
      };

      int SetInput(const char* field, int index);

    private:
      MultiplyEntry(GetData::Dirfile *dirfile, const char* field_code) :
        Entry(dirfile, field_code) { };
  };
}

#endif