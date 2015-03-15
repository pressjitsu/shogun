dnl
dnl $Id$
dnl

AC_MSG_CHECKING(for Shogun support)

PHP_ARG_WITH(shogun,,
[  --with-shogun        Build PHP with Shogun security module], no)

if test "$PHP_SHOGUN" != "no"; then

	PHP_ADD_MAKEFILE_FRAGMENT($abs_srcdir/sapi/shogun/Makefile.frag, $abs_srcdir/sapi/shogun, sapi/shogun)

	SAPI_SHOGUN_PATH=sapi/shogun/shogun

	SHOGUN_CFLAGS=""
	SHOGUN_SOURCES="shogun.c"
	
	PHP_SUBST(SHOGUN_CFLAGS)
	PHP_SUBST(SHOGUN_SOURCES)

	PHP_SELECT_SAPI(shogun, program, $SHOGUN_SOURCES, $SHOGUN_CFLAGS, '$(SAPI_SHOGUN_PATH)')

	BUILD_SHOGUN="\$(LIBTOOL) --mode=link \$(CC) -export-dynamic \$(CFLAGS_CLEAN) \$(EXTRA_CFLAGS) \$(EXTRA_LDFLAGS_PROGRAM) \$(LDFLAGS) \$(PHP_RPATHS) \$(PHP_GLOBAL_OBJS) \$(PHP_BINARY_OBJS) \$(PHP_SHOGUN_OBJS) \$(EXTRA_LIBS) \$(ZEND_EXTRA_LIBS) -o \$(SAPI_SHOGUN_PATH)"

	PHP_SUBST(SAPI_SHOGUN_PATH)
	PHP_SUBST(BUILD_SHOGUN)
fi

AC_MSG_RESULT($PHP_SHOGUN)
