AC_INIT([hexcalc], [2.1.0])
AC_PREREQ([2.69])

AC_CONFIG_AUX_DIR([.build-aux])
AC_CONFIG_MACRO_DIR([.m4])

AM_SILENT_RULES([yes])
AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_SRCDIR([src/hexcalc.cc])
AC_CONFIG_HEADER([include/config.h])

# Checks for programs.
AC_PROG_CXX


# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT8_T
AC_TYPE_INTMAX_T
AC_TYPE_UINTMAX_T

AC_ARG_WITH([readline],
			[AS_HELP_STRING([--without-readline], [Do not use the GNU Readline library])]
)

AS_IF([test "x${with_readline}" != "xno"], [
	AC_CHECK_LIB([readline], [readline],, [AC_MSG_ERROR([Library readline not found. Install it from http://directory.fsf.org/readline.html])])
])

AM_CONDITIONAL([WITH_READLINE], [test "x${ac_cv_lib_readline_readline}" = "xyes"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
