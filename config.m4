dnl If your extension references something external, use with:

PHP_ARG_ENABLE(tdlib, whether to enable TdLib support,
[  --enable-tdlib      Enable TdLib support])

PHP_ARG_WITH(tdlib, for tdlib support,
[  --with-tdlib-dir=DIR Installation prefix of TdLib.
                          If DIR is not specified, use the system library.])
if test "$PHP_TDLIB" != "no"; then

  dnl If a path was given, expect to find the library in $PHP_LIBDIR
  dnl and the include file in include/
  if test "$PHP_TDLIB_DIR" != "yes"; then

    LIBTDLIB_DIR=$PHP_TDLIB_DIR/$PHP_LIBDIR
    LIBTDLIB_INCDIR=$PHP_TDLIB_DIR/include

    AC_DEFINE(HAVE_LIBTDLIB, 1, [ ])
    PHP_ADD_INCLUDE($LIBTDLIB_INCDIR)
    PHP_ADD_LIBRARY_WITH_PATH(tdjson, $LIBTDLIB_DIR, TDLIB_SHARED_LIBADD)

  dnl Otherwise find the header and shared library by the normal means:
  else

    dnl Find the header file:
    AC_MSG_CHECKING([for td/telegram/td_json_client.h in default paths])
    for i in /usr/local/include /usr/include ; do
      if test -r $i/td/telegram/td_json_client.h; then
        LIBTDLIB_INCDIR="$i";
        AC_MSG_RESULT([found in $i])
        break;
      fi
    done
    if test -z "$LIBTDLIB_INCDIR"; then
      AC_MSG_RESULT([not found])
      AC_MSG_ERROR([Could not find td_json_client.h])
    fi

    dnl Find the shared library:
    PHP_CHECK_LIBRARY(tdjson, td_json_client_create,
         [
      AC_DEFINE(HAVE_LIBTDLIB, 1, [ ])
      PHP_ADD_INCLUDE($LIBTDLIB_INCDIR)
      PHP_ADD_LIBRARY(tdjson, 1, TDLIB_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Could not find libtdjson.so or symbol td_json_client_create. Check version and config.log for more information.])
    ],[
      -ltdjson
    ])

  fi

  PHP_SUBST(TDLIB_SHARED_LIBADD)
  PHP_NEW_EXTENSION(tdlib, tdlib.c, $ext_shared)
  PHP_INSTALL_HEADERS([ext/tdlib], [php_tdlib.h])

fi