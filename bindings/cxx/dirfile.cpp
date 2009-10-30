// (C) 2008, 2009 D. V. Wiebe
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
#include "getdata/dirfile.h"

using namespace GetData;

Dirfile::Dirfile()
{
  D = dirfile_open("", 0);
  error_string = NULL;
}

Dirfile::Dirfile(const char* filedir, unsigned long flags,
    gd_parser_callback_t sehandler, void* extra)
{
  D = dirfile_cbopen(filedir, flags, sehandler, extra);
  error_string = NULL;
}

Dirfile::Dirfile(DIRFILE* dirfile)
{
  D = dirfile;
  error_string = NULL;
}

Dirfile::~Dirfile()
{
  if (error_string != NULL)
    delete error_string;

  dirfile_close(D);
}

int Dirfile::Add(GetData::Entry &entry)
{
  int ret = dirfile_add(D, &entry.E);
  entry.SetDirfile(this);
  return ret;
}

int Dirfile::AddSpec(const char *spec, int format_file)
{
  return dirfile_add_spec(D, spec, format_file);
}

int Dirfile::MAdd(GetData::Entry &entry, const char *parent)
{
  int ret = dirfile_madd(D, &entry.E, parent);
  entry.SetDirfile(this);
  return ret;
}

int Dirfile::MAddSpec(const char *spec, const char *parent)
{
  return dirfile_madd_spec(D, spec, parent);
}

Entry *Dirfile::Entry(const char* field_code)
{
  GetData::EntryType type = (GetData::EntryType)get_entry_type(D, field_code);

  switch(type) {
    case RawEntryType:
      return new GetData::RawEntry(this, field_code);
    case LincomEntryType:
      return new GetData::LincomEntry(this, field_code);
    case LinterpEntryType:
      return new GetData::LinterpEntry(this, field_code);
    case BitEntryType:
      return new GetData::BitEntry(this, field_code);
    case SBitEntryType:
      return new GetData::SBitEntry(this, field_code);
    case MultiplyEntryType:
      return new GetData::MultiplyEntry(this, field_code);
    case PhaseEntryType:
      return new GetData::PhaseEntry(this, field_code);
    case PolynomEntryType:
      return new GetData::PolynomEntry(this, field_code);
    case ConstEntryType:
      return new GetData::ConstEntry(this, field_code);
    case StringEntryType:
      return new GetData::StringEntry(this, field_code);
    case IndexEntryType:
      return new GetData::IndexEntry(this, field_code);
    case NoEntryType:
      break;
  }

  return NULL;
}

int Dirfile::Flush(const char* field_code)
{
  return dirfile_flush(D, field_code);
}

int Dirfile::MetaFlush()
{
  return dirfile_metaflush(D);
}

int Dirfile::Error()
{
  return get_error(D);
}

const char* Dirfile::ErrorString(size_t len)
{
  if (error_string == NULL)
    error_string = new char[4096];

  if (len > 4096)
    len = 4096;

  return get_error_string(D, error_string, len);
}

int Dirfile::Include(const char* file, int format_file, unsigned long flags)
{
  return dirfile_include(D, file, format_file, flags);
}

unsigned int Dirfile::SamplesPerFrame(const char* field_code)
{
  return get_spf(D, field_code);
}

unsigned int Dirfile::NFields()
{
  return get_nfields(D);
}

unsigned int Dirfile::NFieldsByType(EntryType type)
{
  return get_nfields_by_type(D, (gd_entype_t)type);
}

const char** Dirfile::FieldListByType(EntryType type)
{
  return get_field_list_by_type(D, (gd_entype_t)type);
}

unsigned int Dirfile::NMFields(const char *parent)
{
  return get_nmfields(D, parent);
}

unsigned int Dirfile::NMFieldsByType(const char *parent, EntryType type)
{
  return get_nmfields_by_type(D, parent, (gd_entype_t)type);
}

const char** Dirfile::MFieldListByType(const char *parent, EntryType type)
{
  return get_mfield_list_by_type(D, parent, (gd_entype_t)type);
}

const void *Dirfile::Constants(DataType type)
{
  return get_constants(D, (gd_type_t)type);
}

const char **Dirfile::Strings()
{
  return get_strings(D);
}

const void *Dirfile::MConstants(const char *parent, DataType type)
{
  return get_mconstants(D, parent, (gd_type_t)type);
}

const char **Dirfile::MStrings(const char *parent)
{
  return get_mstrings(D, parent);
}

const char** Dirfile::FieldList()
{
  return get_field_list(D);
}

const char** Dirfile::MFieldList(const char *parent)
{
  return get_mfield_list(D, parent);
}

unsigned int Dirfile::NVectors()
{
  return get_nvectors(D);
}

const char** Dirfile::VectorList()
{
  return get_vector_list(D);
}

unsigned int Dirfile::NMVectors(const char *parent)
{
  return get_nmvectors(D, parent);
}

const char** Dirfile::MVectorList(const char *parent)
{
  return get_mvector_list(D, parent);
}

off_t Dirfile::NFrames()
{
  return get_nframes(D);
}

size_t Dirfile::GetConstant(const char *field_code, DataType type,
    void *data_out)
{
  return get_constant(D, field_code, (gd_type_t)type, data_out);
}

size_t Dirfile::GetData(const char* field_code, off_t first_frame,
    off_t first_sample, size_t num_frames, size_t num_samples,
    DataType type, void* data_out)
{
  return getdata(D, field_code, first_frame, first_sample, num_frames,
      num_samples, (gd_type_t)type, data_out);
}

size_t Dirfile::GetString(const char *field_code, size_t len, char* data_out)
{
  return get_string(D, field_code, len, data_out);
}

size_t Dirfile::PutConstant(const char *field_code, DataType type,
    const void *data_in)
{
  return put_constant(D, field_code, (gd_type_t)type, data_in);
}

size_t Dirfile::PutData(const char* field_code, off_t first_frame,
    off_t first_sample, size_t num_frames, size_t num_samples,
    DataType type, const void* data_in)
{
  return putdata(D, field_code, first_frame, first_sample, num_frames,
      num_samples, (gd_type_t)type, data_in);
}

size_t Dirfile::PutString(const char *field_code, const char* data_in)
{
  return put_string(D, field_code, data_in);
}

GetData::Fragment* Dirfile::Fragment(int index)
{
  if (index < 0 || index >= get_nfragments(D))
    return NULL;

  return new GetData::Fragment(this, index);
}

int Dirfile::NFragments()
{
  return get_nfragments(D);
}

const char* Dirfile::ReferenceFilename()
{
  const char* ref = dirfile_reference(D, NULL);

  if (ref == NULL)
    return NULL;

  return get_raw_filename(D, ref);
}

int Dirfile::Discard()
{
  int ret = dirfile_discard(D);

  if (!ret)
    D = NULL;

  return ret;
}

int Dirfile::Close()
{
  int ret = dirfile_close(D);

  if (!ret)
    D = NULL;

  return ret;
}

void Dirfile::SetCallback(gd_parser_callback_t sehandler, void* extra)
{
  dirfile_parser_callback(D, sehandler, extra);
}

RawEntry* Dirfile::Reference(const char* field_code)
{
  const char* ref = dirfile_reference(D, field_code);

  if (ref == NULL)
    return NULL;

  return new RawEntry(this, ref);
}

int Dirfile::AlterSpec(const char *line, int recode)
{
  return dirfile_alter_spec(D, line, recode);
}

int Dirfile::MAlterSpec(const char* line, const char *parent, int recode)
{
  return dirfile_malter_spec(D, line, parent, recode);
}

int Dirfile::Delete(const char* field_code, int flags)
{
  return dirfile_delete(D, field_code, flags);
}

int Dirfile::UnInclude(int fragment_index, int del)
{
  return dirfile_uninclude(D, fragment_index, del);
}

DataType Dirfile::NativeType(const char* field_code)
{
  return (DataType)get_native_type(D, field_code);
}

int Dirfile::Validate(const char* field_code)
{
  return dirfile_validate(D, field_code);
}

double Dirfile::FrameNum(const char* field_code, double value,
    off_t frame_start, off_t frame_end)
{
  return get_framenum_subset(D, field_code, value, frame_start, frame_end);
}

int Dirfile::FragmentIndex(const char* field_code)
{
  return get_fragment_index(D, field_code);
}

const char* Dirfile::Name()
{
  return dirfilename(D);
}
