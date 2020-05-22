AC_PREREQ(2.60)
AC_INIT([Minecraft-Daemon], [version-0.1])

dnl Switch to a C++ compiler, and check if it works.
AC_LANG(C++)
AC_PROG_CXX

AC_GNU_SOURCE

dnl Check for C++17 support
AX_CXX_COMPILE_STDCXX([17])

dnl Check for TCLAP and AudioFile
AC_CHECK_HEADERS(tclap/CmdLine.h, [], [AC_ERROR([TCLAP is required for this project.])])
dnl AC_CHECK_HEADERS(tao/json.hpp, [], [AC_ERROR([The Art of C++ / json is required for this project.])])
dnl AC_CHECK_HEADERS(tao/config.hpp, [], [AC_ERROR([The Art Of C++ / config is required for this project.])])
AC_CHECK_HEADERS(rude/config.h, [], [AC_ERROR([Rudeconfig is required for this project.])])

dnl AC_SEARCH_LIBS(rude, rudeconfig, [], [AC_ERROR([Rudeconfig is required for this project.])])

dnl Add compiler flags based on type of build
dnl If it is a debug build (DEBUG_BUILD is defined)
if [[ ! -z ${DEBUG_BUILD+x} ]]; then
    CXXFLAGS="-g"
dnl If it is a coverage build (COVERAGE_BUILD is defined)
elif [[ ! -z ${COVERAGE_BUILD+x} ]]; then
    CXXFLAGS="-ftest-coverage"
dnl If it is a regular build
else
    CXXFLAGS="-O2"
fi

dnl Process Makefile.in to create Makefile
AC_OUTPUT(Makefile)