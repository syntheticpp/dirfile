# Copyright (C) 2009, 2011 D. V. Wiebe
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

SUBDIRS=test

if GDIDL_EXTERNAL
GDIDL_GETDATA_LIBS=$(GETDATA_LIBS)
else
GDIDL_GETDATA_LIBS=../../src/libgetdata.la
endif

idl_LTLIBRARIES = idl_getdata.la
nodist_idl_HEADERS = idl_getdata.dlm

# idl makes heavy use of type punning, ergo -fno-strict-aliasing
AM_CFLAGS = ${WALL} $(IDL_CFLAGS) -fno-strict-aliasing $(GETDATA_CFLAGS)
BUILT_SOURCES=sublist.c constants.c
idl_getdata_la_LDFLAGS = -module -avoid-version -export-symbols-regex IDL_Load \
												 -precious-files-regex 'idl_getdata\.dlm' $(IDL_LIBS)
idl_getdata_la_LIBADD = $(GDIDL_GETDATA_LIBS)
idl_getdata_la_SOURCES = getdata.c
nodist_idl_getdata_la_SOURCES = constants.c sublist.c

sublist.c.in: getdata.c makedlm.sh
	${SHELL} ./makedlm.sh -c $< > $@

sublist.c: sublist.stamp
	@if test ! -f $@; then \
		rm -f $<; \
		$(MAKE) $<; \
		fi

if HAVE_DIFF

sublist.stamp: sublist.c.in
	@if $(DIFF) sublist.c sublist.c.in >/dev/null 2>&1; then \
		echo "sublist.c is unchanged"; \
		else \
		rm -f sublist.c; \
		cp sublist.c.in sublist.c; \
		fi; \
		touch sublist.stamp

else

sublist.stamp: sublist.c.in
	@rm -f sublist.c; \
		cp sublist.c.in sublist.c; \
		touch sublist.stamp

endif

constants.c: ../make_parameters
	../make_parameters i > $@

../make_parameters: ../make_parameters.c
	cd .. && ${MAKE} make_parameters

idl_getdata.dlm: getdata.c makedlm.sh
	${SHELL} ./makedlm.sh -d $< > $@

all-local: .libs/idl_getdata.dlm

# This is required to properly run the test suite
.libs/idl_getdata.dlm: idl_getdata.dlm
	if [ ! -e .libs ]; then mkdir -p .libs; fi
	cp $< .libs

clean-local:
	rm -rf ${BUILT_SOURCES} *~ idl_getdata.dlm makedlm.sh sublist.c.in sublist.stamp

