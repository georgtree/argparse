#!/bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during Tcl installation
dnl	to configure the system for the local environment.

AC_INIT([argparse],[0.60])

TEA_INIT()

AC_CONFIG_AUX_DIR(tclconfig)

TEA_PATH_TCLCONFIG
TEA_LOAD_TCLCONFIG
TEA_PREFIX
TEA_SETUP_COMPILER

TEA_ADD_SOURCES([generic/argparse.c])
TEA_ADD_HEADERS([generic/argparse.h])
TEA_ADD_INCLUDES([])
TEA_ADD_LIBS([])
TEA_ADD_CFLAGS([])
TEA_ADD_STUB_SOURCES([])
TEA_ADD_TCL_SOURCES([argparse.tcl])

AC_ARG_ENABLE([tcl-only],
  [AS_HELP_STRING([--enable-tcl-only], [Set to install tcl-only version of the package])],
  [enable_tcl_only=$enableval], [enable_tcl_only=no])

if test "x$enable_tcl_only" = xyes; 
then
  AC_SUBST(HAVE_TCL_ONLY, 1)
else
  AC_SUBST(HAVE_TCL_ONLY, 0)
fi

TEA_PUBLIC_TCL_HEADERS
TEA_ENABLE_THREADS
TEA_ENABLE_SHARED
TEA_CONFIG_CFLAGS
TEA_ENABLE_SYMBOLS
TEA_MAKE_LIB
TEA_PROG_TCLSH

AC_SUBST(TCLLIBPATH)

AC_CONFIG_FILES([Makefile pkgIndex.tcl])

AC_OUTPUT
