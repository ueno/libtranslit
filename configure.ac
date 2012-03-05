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

AC_PREREQ(2.63)
AC_CONFIG_MACRO_DIR([m4])

AC_INIT([libtranslit], [0.0.2], [ueno@unixuser.org])

TRANSLIT_API_VERSION=1.0
TRANSLIT_API_MAJOR_VERSION=1
TRANSLIT_API_MINOR_VERSION=0
TRANSLIT_API_PC_VERSION=1.0
TRANSLIT_LIBRARY_SUFFIX="-$TRANSLIT_API_VERSION"

AC_SUBST([TRANSLIT_API_VERSION])
AC_SUBST([TRANSLIT_API_MAJOR_VERSION])
AC_SUBST([TRANSLIT_API_MINOR_VERSION])
AC_SUBST([TRANSLIT_API_PC_VERSION])
AC_SUBST([TRANSLIT_LIBRARY_SUFFIX])
AC_SUBST([TRANSLIT_LIBRARY_SUFFIX_U],[AS_TR_SH([$TRANSLIT_LIBRARY_SUFFIX])])

dnl Init automake
AM_INIT_AUTOMAKE([color-tests])
AM_MAINTAINER_MODE([enable])
AC_GNU_SOURCE

dnl Check for programs
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CC_STDC
AC_PROG_INSTALL
AC_C_BIGENDIAN

# define PACKAGE_VERSION_* variables
AM_DISABLE_STATIC
AC_ISC_POSIX
AC_HEADER_STDC
LT_INIT

# If library source has changed since last release, increment revision
# If public symbols have been added, removed or changed since last release,
#  increment current and set revision to 0
# If public symbols have been added since last release, increment age
# If public symbols have been removed since last release, set age to 0
m4_define([libtranslit_lt_current], [0])
m4_define([libtranslit_lt_revision], [0])
m4_define([libtranslit_lt_age], [0])
LT_CURRENT=libtranslit_lt_current
LT_REVISION=libtranslit_lt_revision
LT_AGE=libtranslit_lt_age
AC_SUBST([LT_CURRENT])
AC_SUBST([LT_REVISION])
AC_SUBST([LT_AGE])

# check for glib
AM_PATH_GLIB_2_0
PKG_CHECK_MODULES([GLIB], [glib-2.0], ,
  [AC_MSG_ERROR([can't find glib])])
PKG_CHECK_MODULES([GIO], [gio-2.0], ,
  [AC_MSG_ERROR([can't find gio])])
PKG_CHECK_MODULES([GOBJECT], [gobject-2.0], ,
  [AC_MSG_ERROR([can't find gobject])])
PKG_CHECK_MODULES([GMODULE], [gmodule-2.0], ,
  [AC_MSG_ERROR([can't find gmodule])])

# check for icu
AC_ARG_ENABLE([icu],
	AS_HELP_STRING([--enable-icu], [Enable ICU filter]),
	[enable_icu=$enableval], [enable_icu=auto])
if test "x$enable_icu" != "xno"; then
   PKG_CHECK_MODULES([ICU], [icu-io], enable_icu=yes, enable_icu=no)
fi
AM_CONDITIONAL([ENABLE_ICU], [test "x$enable_icu" = "xyes"])

# check for m17n-lib
AC_ARG_ENABLE([m17n-lib],
	AS_HELP_STRING([--enable-m17n-lib], [Enable m17n-lib filter]),
	[enable_m17n_lib=$enableval], [enable_m17n_lib=auto])
if test "x$enable_m17n_lib" != "xno"; then
   PKG_CHECK_MODULES([M17N], [m17n-shell], enable_m17n_lib=yes, enable_m17n_lib=no)
fi
AM_CONDITIONAL([ENABLE_M17N_LIB], [test "x$enable_m17n_lib" = "xyes"])

# check for gtk-doc
m4_ifdef([GTK_DOC_CHECK], [
GTK_DOC_CHECK([1.14],[--flavour no-tmpl])
],[
AM_CONDITIONAL([ENABLE_GTK_DOC], false)
])

# check gobject introspection
GOBJECT_INTROSPECTION_CHECK([0.9.0])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile
libtranslit/Makefile
libtranslit/libtranslit.pc
modules/Makefile
tests/Makefile
docs/Makefile])
AC_OUTPUT
