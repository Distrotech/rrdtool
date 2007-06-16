dnl $Id$
dnl config.m4 for extension rrdtool

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl
dnl Checks for the configure options
dnl

PHP_ARG_WITH(rrdtool, for rrdtool support,
[  --with-rrdtool[=DIR]       Include rrdtool support (requires rrdtool >= 1.0.49).])

if test "$PHP_RRDTOOL" != "no"; then

  if test "$PHP_RRDTOOL" != "yes"; then
  AC_MSG_CHECKING(if rrdtool specified path is valid)
    if test -r $PHP_RRDTOOL/include/rrd.h && test -f $PHP_RRDTOOL/lib/librrd.$SHLIB_SUFFIX_NAME -o -f $PHP_RRDTOOL/lib/librrd.a; then # path given as parameter
      RRDTOOL_DIR=$PHP_RRDTOOL
      RRDTOOL_INCDIR=$PHP_RRDTOOL/include
      RRDTOOL_LIBDIR=$PHP_RRDTOOL/lib
    AC_MSG_RESULT([yes])
    else
    AC_MSG_RESULT([no])
      AC_MSG_ERROR([The specified RRDTool path is Invalid or the installation is incomplete
      Please specify another path or reinstall the rrdtool distribution])
    fi
  else
    dnl Header path
    AC_MSG_CHECKING([for rrdtool header files in default path])
    for i in /usr/local/rrdtool /usr/local /usr /opt ""; do
     test -r $i/include/rrd.h && RRDTOOL_DIR=$i && RRDTOOL_INCDIR=$i/include
    done
    if test -z "$RRDTOOL_INCDIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the rrdtool distribution])
    else
     AC_MSG_RESULT(found in $RRDTOOL_INCDIR)
    fi
    dnl Library path
    AC_MSG_CHECKING([for rrdtool library files in default path])
    for i in librrd.$SHLIB_SUFFIX_NAME librrd.a; do
      test -f $RRDTOOL_DIR/lib/$i && RRDTOOL_LIBDIR=$RRDTOOL_DIR/lib
    done
    if test -z "$RRDTOOL_LIBDIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Please reinstall the rrdtool distribution])
    else
     AC_MSG_RESULT(found in $RRDTOOL_LIBDIR)
    fi
  fi

dnl Finish the setup

  RRD_H_PATH="$RRDTOOL_INCDIR/rrd.h"
  PHP_RRDTOOL_DIR=$RRDTOOL_DIR
  PHP_ADD_INCLUDE($RRDTOOL_INCDIR)

  PHP_CHECK_LIBRARY(rrd, rrd_create,
  [],[
    PHP_CHECK_LIBRARY(rrd, rrd_create,
    [],[
      AC_MSG_ERROR([wrong rrd lib version or lib not found])
    ],[
      -L$RRDTOOL_LIBDIR -ldl
    ])
  ],[
    -L$RRDTOOL_LIBDIR -ldl
  ])

 AC_MSG_CHECKING([rrdtool version])
  AC_TRY_COMPILE([
#include <$RRD_H_PATH>
  ], [int main() {
    double some_variable;
    some_variable = rrd_version();
    }
  ], [
    AC_MSG_RESULT([1.2.x])
    ac_cv_rrdversion=yes
    ], [
    AC_MSG_RESULT([1.0.x])
    ac_cv_rrdversion=no
    ])

  if test "$ac_cv_rrdversion" = yes; then
    AC_DEFINE(HAVE_RRD_12X, 1, [Whether you have rrd_verion])
  fi 

  PHP_ADD_LIBRARY_WITH_PATH(rrd, $RRDTOOL_LIBDIR, RRDTOOL_SHARED_LIBADD)

  PHP_NEW_EXTENSION(rrdtool, rrdtool.c, $ext_shared)
  PHP_SUBST(RRDTOOL_SHARED_LIBADD)
  AC_DEFINE(HAVE_RRDTOOL, 1, [ ])
fi
