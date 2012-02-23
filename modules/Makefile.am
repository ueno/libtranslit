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

moduledir = $(pkglibdir)/modules
module_LTLIBRARIES =
module_flags =							\
	--export-dynamic					\
	--avoid-version						\
	--module						\
	--no-undefined						\
	--export-symbols-regex '^translit_module_(load|unload)'	\
	$(NULL)

AM_CFLAGS =					\
	$(GLIB_CFLAGS)				\
	$(GIO_CFLAGS)				\
	$(GOBJECT_CFLAGS)			\
	$(GMODULE_CFLAGS)			\
	$(NULL)
AM_LDFLAGS =					\
	$(GLIB_LIBS)				\
	$(GIO_LIBS)				\
	$(GOBJECT_LIBS)				\
	$(GMODULE_LIBS)				\
	$(NULL)

if ENABLE_ICU
module_LTLIBRARIES += libtransliticu.la
libtransliticu_la_SOURCES = transliteratoricu.c icumodule.c
libtransliticu_la_CFLAGS =			\
	-I$(top_srcdir)				\
	$(ICU_CFLAGS)				\
	$(AM_CFLAGS)				\
	$(NULL)
libtransliticu_la_LDFLAGS = $(module_flags)
libtransliticu_la_LIBADD =			\
	$(ICU_LIBS)				\
	$(AM_LDFLAGS)				\
	$(NULL)
noinst_HEADERS = transliteratoricu.h
endif

if ENABLE_M17N_LIB
module_LTLIBRARIES += libtranslitm17n.la
libtranslitm17n_la_SOURCES = transliteratorm17n.c m17nmodule.c
libtranslitm17n_la_CFLAGS =			\
	-I$(top_srcdir)				\
	$(M17N_CFLAGS)				\
	$(AM_CFLAGS)				\
	$(NULL)
libtranslitm17n_la_LDFLAGS = $(module_flags)
libtranslitm17n_la_LIBADD =			\
	$(M17N_LIBS)				\
	$(AM_LDFLAGS)				\
	$(NULL)
noinst_HEADERS = transliteratorm17n.h
endif

-include $(top_srcdir)/git.mk

