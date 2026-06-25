#!/bin/bash -norc
dnl	This file is an input file used by the GNU "autoconf" program to
dnl	generate the file "configure", which is run during Tcl installation
dnl	to configure the system for the local environment.

AC_INIT([argparse],[0.61])
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

DOC_HTML_FILES="index.html objects.inv search.html searchindex.js genindex.html"
DOC_SRCDIR="${srcdir}/docs"
if test -d "$DOC_SRCDIR"; then
    # Add all top-level *.html files from docs/, not subdirectories.
    for f in "$DOC_SRCDIR"/*.html; do
        test -f "$f" || continue

        b=`basename "$f"`

        case " $DOC_HTML_FILES " in
            *" $b "*) ;;
            *) DOC_HTML_FILES="$DOC_HTML_FILES $b" ;;
        esac
    done
    # Add everything from docs/_static and docs/assets recursively.
    for d in _static assets; do
        if test -d "$DOC_SRCDIR/$d"; then
            for f in `find "$DOC_SRCDIR/$d" -type f -print`; do
                rel=`echo "$f" | sed "s|^$DOC_SRCDIR/||"`

                case " $DOC_HTML_FILES " in
                    *" $rel "*) ;;
                    *) DOC_HTML_FILES="$DOC_HTML_FILES $rel" ;;
                esac
            done
        fi
    done

else
    AC_MSG_WARN([documentation directory not found: $DOC_SRCDIR])
fi
#AC_MSG_NOTICE([DOC_HTML_FILES = $DOC_HTML_FILES])
AC_SUBST([DOC_HTML_FILES])

# Test files to include in the distribution.
# Paths are relative to $srcdir.
DIST_TEST_FILES="test/all.tcl test/argparse.test"
AC_SUBST([DIST_TEST_FILES])

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
