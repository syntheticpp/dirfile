# Copyright (C) 2009 D. V. Wiebe
#
##########################################################################
#
# This file is part of the GetData project.
#
# GetData is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the
# Free Software Foundation; either version 2.1 of the License, or (at your
# option) any later version.
#
# GetData is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with GetData; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
AUTOMAKE_OPTIONS = foreign

if CC_WALL
WALL=-Wall
endif

if GETDATA_DEBUG
DEBUG_C = ../../src/debug.c
endif

SUBDIRS=test

python_LTLIBRARIES = pygetdata.la
LIBS=

BUILT_SOURCES = pyconstants.c
AM_CFLAGS = ${WALL} -fno-strict-aliasing
AM_CPPFLAGS = $(CPPFLAGS) $(PYTHON_CPPFLAGS) $(NUMPY_CPPFLAGS)
pygetdata_la_LDFLAGS = -module -avoid-version \
											 -export-symbols-regex initpygetdata
pygetdata_la_LIBADD = $(PYTHON_LIBS) ../../src/libgetdata.la
pygetdata_la_SOURCES = pygetdata.c pydirfile.c pyentry.c pyfragment.c \
											 ${DEBUG_C} pygetdata.h
nodist_pygetdata_la_SOURCES = pyconstants.c

pyconstants.c: ../make_parameters
	../make_parameters p > $@

../make_parameters: ../make_parameters.c
	cd .. && ${MAKE} make_parameters

clean-local:
	rm -rf ${BUILT_SOURCES} *~ 

