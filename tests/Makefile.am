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

TESTS_ENVIRONMENT = TRANSLIT_MODULE_PATH=$(top_builddir)/modules/.libs
TESTS = basic
noinst_PROGRAMS = $(TESTS)

basic_SOURCES = basic.c
basic_CFLAGS =					\
	-I$(top_srcdir)				\
	$(GLIB_CFLAGS)				\
	$(GIO_CFLAGS)				\
	$(GOBJECT_CFLAGS)			\
	$(GMODULE_CFLAGS)			\
	$(NULL)

basic_LDADD =						\
	$(GLIB_LIBS)					\
	$(GIO_LIBS)					\
	$(GOBJECT_LIBS)					\
	$(GMODULE_LIBS)					\
	$(top_builddir)/libtranslit/libtranslit.la	\
	$(NULL)

-include $(top_srcdir)/git.mk
