dnl
dnl   /*------------------------------------------------------------------+
dnl    |                             SuBStrAte                            |
dnl    |------------------------------------------------------------------|
dnl    |                          autoconf.ac                             |
dnl    |------------------------------------------------------------------|
dnl    |                   First version: 05/05/2015                      |
dnl    +------------------------------------------------------------------+
dnl
dnl Input file for autoconf to build a configuration shellscript.

m4_define([version_major], [0])
m4_define([version_minor], [1])
m4_define([version_revision], [1])

AC_PREREQ(2.53)
dnl Fill here the @bug email adress.
AC_INIT([SuBStrAte], [version_major.version_minor.version_revision],
        [thomaskuntz@sharedmemory.fr])
dnl A common file, which serve as a test.
AC_CONFIG_SRCDIR([include/substrate/substrate.h])
dnl Put as most as possible configuration files to an auxialiry
dnl directory.
AC_CONFIG_AUX_DIR(autoconf)
AC_CONFIG_MACRO_DIR([m4])

dnl Initialize automake.
AM_INIT_AUTOMAKE([foreign])
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

dnl /**************************************************************************
dnl  *                              Checking                                  *
dnl  **************************************************************************/


dnl Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROG(CD, cd)
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_CHECK_PROGS(DOXYGEN,doxygen,doxygen)
  
AX_CC_MAXOPT
AC_SUBST(CFLAGS_WARN)
AX_CFLAGS_WARN_ALL(CFLAGS_WARN)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stddef.h stdlib.h string.h strings.h unistd.h])

#dnl Checks for library functions.
#AC_CHECK_FUNCS(strtol)


dnl /**************************************************************************
dnl  *                             Option setting                             *
dnl  **************************************************************************/


dnl /**************************************************************************
dnl  *                            Where is GMP?                               *
dnl  **************************************************************************/

gmp_package="yes"
gmp_include_package="yes"
gmp_library_package="yes"

NEED_MP="no"

dnl --with-gmp=gmp-path
AC_ARG_WITH(gmp,
        [  --with-gmp=DIR          DIR where the gmp package is installed],
  [ echo "Package gmp : $withval" &&
    gmp_package=$withval &&
          GMP_INC=$gmp_package/include &&
          GMP_LIB=$gmp_package/lib &&
          CPPFLAGS=-DCANDL_LINEAR_VALUE_IS_MP &&
          NEED_MP="yes"])

AC_ARG_WITH(gmp-include,
        [  --with-gmp-include=DIR  DIR where gmp.h is installed],
  [ echo "Package gmp-include : $withval" &&
    gmp_include_package=$withval &&
          GMP_INC=$gmp_include_package &&
          CPPFLAGS=-DCANDL_LINEAR_VALUE_IS_MP &&
    NEED_MP="yes"])

AC_ARG_WITH(gmp-library,
        [  --with-gmp-library=DIR  DIR where the gmp library is installed],
  [ echo "Package gmp-library : $withval" &&
    gmp_library_package=$withval &&
          GMP_LIB=$gmp_library_package &&
          CPPFLAGS=-DCANDL_LINEAR_VALUE_IS_MP &&
    NEED_MP="yes"])

AC_ARG_ENABLE(int-version,
  [  --enable-int-version    'int' (32 bits) version is built],
  [ echo "Package int : $enableval" &&
    BITS="32" &&
          CPPFLAGS=-DCANDL_LINEAR_VALUE_IS_LONG])

AC_ARG_ENABLE(llint-version,
  [  --enable-llint-version  'long long int' (64 bits) version is built],
  [ echo "Package long long int : $enableval" &&
    BITS="64" &&
          CPPFLAGS=-DCANDL_LINEAR_VALUE_IS_LONGLONG])

AC_ARG_ENABLE(mp-version,
  [  --enable-mp-version     'MP' (multiple precision) version is built],
  [ echo "Package mp : $enableval" &&
    BITS="MP" &&
          CPPFLAGS=-DCANDL_LINEAR_VALUE_IS_MP &&
          NEED_MP="yes"])

AC_ARG_ENABLE(piplib-hybrid,
  [  --enable-piplib-hybrid   Link with piplib-hybrid],
  [ echo "Piplib-hybrid support : $enableval" &&
          if test "x$enableval" = "xyes"; then 
            CPPFLAGS=-DCANDL_HAS_PIPLIB_HYBRID
          fi])

dnl Checking for gmp
AC_MSG_CHECKING(whether gmp works)
if test "$gmp_package" = "no"; then
  echo "GMP package not defined"
  AC_MSG_RESULT(no)
  TO_BUILD_MP=""
else
  if test "$NEED_MP" = "no"; then
    echo "Mode normal GMP"
    TO_BUILD="$TO_BUILD MP"
    AC_CHECK_HEADER(gmp.h,
                    [AC_SEARCH_LIBS([__gmpz_init], [gmp],
                      [LIBS="$LIBS -lgmp"],
          [echo "Can't find gmp library." &&
           echo "MP version will not be built." &&
           TO_BUILD_MP=""])],
        [echo "Can't find gmp headers." &&
         echo "MP version will not be built." &&
         TO_BUILD_MP=""])
  else
    dnl Default given by --with-X is "yes", --without-X is "no". We also
    dnl initialized manually all gmp_package* variables to "yes" (thus they are
    dnl supposed to be "yes" except if the user set them himself).
    if test "$gmp_package" != "yes" ; then
      echo "(GMP path has been set by user)"
      GMP_DIR=$gmp_package
      dnl Useful for AC_CHECK_X to find what we want.
      CPPFLAGS="-I$GMP_DIR/include $CPPFLAGS"
      LDFLAGS="-L$GMP_DIR/lib $LDFLAGS"
    fi

    if test "$gmp_include_package" != "yes" ; then
      CPPFLAGS="-I$GMP_INC $CPPFLAGS"
    fi

    if test "$gmp_library_package" != "yes" ; then
      LDFLAGS="-L$GMP_LIB $LDFLAGS"
    fi

    AC_CHECK_HEADER(gmp.h,
                    [],
                    [AC_MSG_ERROR(Can't find gmp headers.)])
    AC_SEARCH_LIBS([__gmpz_init], [gmp],
                 [LIBS="$LIBS -lgmp"],
                 [AC_MSG_ERROR(Can't find gmp library.)])

    AC_MSG_RESULT(yes)
  fi
fi



dnl /**************************************************************************
dnl  *                   Where is the PIP Library?                            *
dnl  **************************************************************************/

AX_SUBMODULE(piplib,system|build|bundled,system)

AC_SUBST(PIPLIB_CPPFLAGS)
AC_SUBST(PIPLIB_LDFLAGS)
AC_SUBST(PIPLIB_LIBS)

PIPLIB_LA="piplib_undefined"
case "$BITS" in
32)
  PIPLIB_LA="piplib_sp"
  ;;
64)
  PIPLIB_LA="piplib_dp"
  ;;
MP)
  PIPLIB_LA="piplib_gmp"
  ;;
esac

case "$with_piplib" in
bundled)
  PIPLIB_CPPFLAGS="-I$srcdir/piplib/include -Ipiplib/include"
  PIPLIB_LIBS="$srcdir/piplib/lib$PIPLIB_LA.la"
  ;;
build)
  PIPLIB_CPPFLAGS="-I$piplib_srcdir/include -I$with_piplib_builddir/include"
  PIPLIB_LIBS="$with_piplib_builddir/lib$PIPLIB_LA.la"
  ;;
system)
  if test "x$with_piplib_prefix" = "x"; then
    echo "Missing system prefix to PIP library, using default one : /usr/local/"
    with_piplib_prefix="/usr/local/"
  fi
  if test "x$with_piplib_prefix" != "x"; then
    PIPLIB_CPPFLAGS="-I$with_piplib_prefix/include"
  fi
  if test "x$with_piplib_exec_prefix" != "x"; then
    PIPLIB_LDFLAGS="-L$with_piplib_exec_prefix/lib"
  fi
  PIPLIB_LIBS="$with_piplib_prefix/lib/lib$PIPLIB_LA.la -l$PIPLIB_LA"
  echo $PIPLIB_LIBS
esac
AM_CONDITIONAL(BUNDLED_PIPLIB, test $with_piplib = bundled)




dnl /**************************************************************************
dnl  *                   Where is the OpenScop Library?                       *
dnl  **************************************************************************/

AX_SUBMODULE(osl,system|build|bundled,system)

AC_SUBST(OSL_CPPFLAGS)
AC_SUBST(OSL_LDFLAGS)
AC_SUBST(OSL_LIBS)
case "$with_osl" in
bundled)
  osl_prefix="$srcdir/osl/"
  OSL_CPPFLAGS="-I$osl_prefix/include -Iosl/include"
  OSL_LIBS="$osl_prefix/libosl.la"
  ;;
build)
  osl_prefix="$with_osl_builddir"
  OSL_CPPFLAGS="-I$osl_prefix/include"
  OSL_LIBS="$osl_prefix/libosl.la"
  ;;
system | *)
  #default path to osl lib if it wasn't given as argument
  if test "x$with_osl_prefix" = "x"; then
    echo "Missing system prefix to osl library, using default one : /usr/local/"
    with_osl_prefix="/usr/local/"
fi
  osl_prefix="$with_osl_prefix"
  OSL_CPPFLAGS="-I$osl_prefix/include"
  OSL_LIBS="$osl_prefix/lib/libosl.so -losl"
esac
AM_CONDITIONAL(BUNDLED_OSL, test $with_osl = bundled)


dnl /**************************************************************************
dnl  *                         Where is Candl ?                               *
dnl  **************************************************************************/

AX_SUBMODULE(candl,system|build|bundled,system)

AC_SUBST(CANDL_CPPFLAGS)
AC_SUBST(CANDL_LDFLAGS)
AC_SUBST(CANDL_LIBS)
case "$with_candl" in
bundled)
  CANDL_CPPFLAGS="-I$srcdir/candl/include -Icandl/include -DCANDL_LINKED"
  CANDL_LIBS="$srcdir/candl/libcandl.la"
  ;;
build)
  CANDL_CPPFLAGS="-I$candl_srcdir/include -I$with_candl_builddir/include -DCANDL_LINKED"
  CANDL_LIBS="$with_candl_builddir/libcandl.la"
  ;;
system)
  #default path to candl lib if it wasn't given as argument
  if test "x$with_candl_prefix" = "x"; then
      echo "Missing system prefix to candl library, using default one : /usr/local/"
      with_candl_prefix="/usr/local/"
  fi
  CANDL_CPPFLAGS="-I$with_candl_prefix/include -DCANDL_LINKED"
  CANDL_LIBS="$with_candl_prefix/lib/libcandl.so -lcandl"
esac
AM_CONDITIONAL(BUNDLED_CANDL, test $with_candl = bundled)

dnl /**************************************************************************
dnl  *                         Where is Pluto ?                               *
dnl  **************************************************************************/

AX_SUBMODULE(pluto,system|build|bundled,bundled)

AC_SUBST(PLUTO_CPPFLAGS)
AC_SUBST(PLUTO_LDFLAGS)
AC_SUBST(PLUTO_LIBS)
case "$with_pluto" in
bundled)
  PLUTO_CPPFLAGS="-I$srcdir/pluto/include -Ipluto/include -Ipluto/src -DPLUTO_LINKED"
  PLUTO_LIBS="$srcdir/pluto/src/libpluto.la"
  ;;
build)
  PLUTO_CPPFLAGS="-I$pluto_srcdir/include -I$with_pluto_builddir/include -DPLUTO_LINKED"
  PLUTO_LIBS="$with_pluto_builddir/libpluto.la"
  ;;
system | *)
  #default path to pluto lib if it wasn't given as argument
  if test "x$with_pluto_prefix" = "x"; then
    echo "Missing system prefix to pluto library, using default one : /usr/local/"
    with_pluto_prefix="/usr/local/"
  fi
  PLUTO_CPPFLAGS="-I$with_pluto_prefix/include -DPLUTO_LINKED"
  PLUTO_LIBS="$with_pluto_prefix/lib/libpluto.la -lpluto"
esac
AM_CONDITIONAL(BUNDLED_PLUTO, test $with_pluto = bundled)

dnl /**************************************************************************
dnl  *                            Substitutions                               *
dnl  **************************************************************************/


dnl Substitutions to do.
AC_SUBST(ac_aux_dir)
AC_SUBST(abs_top_srcdir)

AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_REVISION)

dnl Configure Makefiles.
AC_CONFIG_FILES([Makefile doc/Makefile])

if test $with_osl = bundled; then
  #AC_CONFIG_SUBDIRS(osl)
  configureopts="--with-gmp=system \
  --with-gmp-prefix=/usr/local/ \
  --prefix=$prefix"
  (cd openscop/
   ./configure ${configureopts}
   )
fi
if test $with_candl = bundled; then
#  AC_CONFIG_SUBDIRS(candl)
#TODO remove build piplib
  configureopts="--prefix=$prefix \
      --with-piplib=system \
      --with-piplib-prefix=/usr/local/ "
  if test $with_osl = bundled OR $with_osl = build ; then
    configureopts=$configureopts"--with-osl=build \
    --with-osl-builddir=$osl_prefix"
  else
    configureopts=$configureopts"--with-osl=system \
    --with-osl-prefix=$osl_prefix"
  fi
  (cd candl/
   ./configure ${configureopts}   )
fi

#if test $with_pluto = bundled; then
  ##AC_CONFIG_SUBDIRS(pluto)
  #configureopts=""
  #(cd pluto/
   #./configure ${configureopts}   )
#fi

AC_OUTPUT

echo "             /*-----------------------------------------------+"
echo "              |        SuBStrAte configuration is OK          |"
echo "              +-----------------------------------------------*/"
echo "It appears that your system is OK to start Clay compilation. You need"
echo "now to type \"make\". Lastly type \"make install\" to install Clay on"
echo "your system (log as root if necessary)."
