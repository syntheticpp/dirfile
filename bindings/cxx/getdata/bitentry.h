// Copyright (C) 2008-2010 D. V. Wiebe
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

#ifndef GETDATA_BITENTRY_H
#define GETDATA_BITENTRY_H

#include <getdata/entry.h>

namespace GetData {

  class Dirfile;

  class BitEntry : public Entry {
    friend class Dirfile;

    public:
      BitEntry() : Entry() { E.field_type = GD_BIT_ENTRY; };

      BitEntry(const char* field_code, const char* in_field, int bitnum,
          int numbits = 1, int fragment_index = 0);

      virtual const char *Input() const { return E.in_fields[0]; };

      virtual int FirstBit() const { return E.u.bit.bitnum; };

      virtual int NumBits() const { return E.u.bit.numbits; };

      int SetInput(const char* field);
      int SetFirstBit(int first_bit);
      int SetFirstBit(const char* first_bit);
      int SetNumBits(int num_bits);
      int SetNumBits(const char* num_bits);

      virtual const char *Scalar(int index = 0) const;

      virtual int ScalarIndex(int index = 0) const;

    private:
      BitEntry(const GetData::Dirfile *dirfile, const char* field_code) :
        Entry(dirfile, field_code) { };
  };
}

#endif
