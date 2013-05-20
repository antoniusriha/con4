AC_INIT([con4], 0.1)
AM_INIT_AUTOMAKE

PKG_CHECK_MODULES(QT4, [QtCore QtGui >= 4.8.0], [
  AC_PATH_PROGS(MOC, [moc-qt4 moc], moc,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
  AC_PATH_PROG(RCC, rcc, rcc,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
  AC_PATH_PROGS(UIC, [uic-qt4 uic], uic,`eval $PKG_CONFIG --variable=exec_prefix QtCore`/bin)
], [
  AC_MSG_ERROR([Qt 4 library not found])
])

#
# Setup qmake
#
AC_PATH_PROG(QMAKE, qmake, no)
if test "x$QMAKE" = "xno"; then
	AC_MSG_ERROR([qmake not found])
fi

AC_CONFIG_FILES(Makefile)
AC_OUTPUT

