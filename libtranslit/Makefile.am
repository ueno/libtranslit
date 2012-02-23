# Copyright (C) 2012 Daiki Ueno <ueno@unixuser.org>
# Copyright (C) 2012 Red Hat, Inc.

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

lib_LTLIBRARIES = libtranslit.la

moduledir = $(pkglibdir)/modules

libtranslitincludedir = $(includedir)/translit-@TRANSLIT_API_VERSION@
libtranslitinclude_HEADERS =			\
	translit.h				\
	translittransliterator.h		\
	$(NULL)

CLEANFILES =

libtranslit_la_SOURCES = translittransliterator.c
libtranslit_la_CFLAGS =				\
	-I$(top_srcdir)				\
	-DMODULEDIR=\"$(moduledir)\"		\
	$(GLIB_CFLAGS)				\
	$(GIO_CFLAGS)				\
	$(GOBJECT_CFLAGS)			\
	$(GMODULE_CFLAGS)			\
	$(NULL)
libtranslit_la_LIBADD =				\
	$(GLIB_LIBS)				\
	$(GIO_LIBS)				\
	$(GOBJECT_LIBS)				\
	$(GMODULE_LIBS)				\
	$(NULL)
libtranslit_la_LDFLAGS =						\
        -version-info "$(LT_CURRENT)":"$(LT_REVISION)":"$(LT_AGE)"	\
        -export-dynamic							\
        -export-symbols-regex "^translit_"				\
        $(NULL)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libtranslit.pc

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(builddir)
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION
Translit@TRANSLIT_LIBRARY_SUFFIX@.gir: libtranslit.la
Translit@TRANSLIT_LIBRARY_SUFFIX_U@_gir_SCANNERFLAGS = --pkg-export=libtranslit
Translit@TRANSLIT_LIBRARY_SUFFIX_U@_gir_INCLUDES = GLib-2.0 GObject-2.0 Gio-2.0
Translit@TRANSLIT_LIBRARY_SUFFIX_U@_gir_CFLAGS = $(libtranslit_la_CFLAGS)
Translit@TRANSLIT_LIBRARY_SUFFIX_U@_gir_LIBS = libtranslit.la
Translit@TRANSLIT_LIBRARY_SUFFIX_U@_gir_FILES = $(libtranslit_la_SOURCES) $(libtranslitinclude_HEADERS)

INTROSPECTION_GIRS += Translit@TRANSLIT_LIBRARY_SUFFIX@.gir
girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

-include $(top_srcdir)/git.mk
