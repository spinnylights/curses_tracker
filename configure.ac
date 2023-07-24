#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([curses_tracker], [0.0.1], [bugs@milky.flowers])

AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/curve.cpp])

# Checks for programs.
AC_PROG_CXX

AX_CXX_COMPILE_STDCXX([17], [noext], [])

# Checks for libraries.
#
dnl Check for SDL2
SDL_VERSION=2.28.1
AM_PATH_SDL2($SDL_VERSION,
             :,
             AC_MSG_ERROR([SDL version $SDL_VERSION not found!]))

AX_WITH_CURSES

AX_LIB_SQLITE3

# Checks for header files.
AC_CHECK_HEADERS([sys/ioctl.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_CHECK_FUNCS([floor memset pow putenv setlocale strerror])

AM_INIT_AUTOMAKE([subdir-objects foreign])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
