#
# This file is protected by Copyright. Please refer to the COPYRIGHT file distributed with this 
# source distribution.
# 
# This file is part of REDHAWK Basic Components DataConverter.
# 
# REDHAWK Basic Components DataConverter is free software: you can redistribute it and/or modify it under the terms of 
# the GNU General Public License as published by the Free Software Foundation, either 
# version 3 of the License, or (at your option) any later version.
# 
# REDHAWK Basic Components DataConverter is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
# without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
# PURPOSE.  See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with this 
# program.  If not, see http://www.gnu.org/licenses/.
#

AC_INIT(rh.DataConverter, 4.0.2)
AM_INIT_AUTOMAKE([nostdinc foreign])
AC_CONFIG_MACRO_DIR([m4])

m4_ifdef([AC_USE_INTEL],[AC_USE_INTEL])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

if test "x$GCC" = xyes ; then
    CFLAGS="-msse3 -O3 $CFLAGS"
    CXXFLAGS="-msse3 -O3 $CXXFLAGS"
fi

AC_CORBA_ORB
OSSIE_CHECK_OSSIE
OSSIE_SDRROOT_AS_PREFIX

m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Dependencies
PKG_CHECK_MODULES([PROJECTDEPS], [ossie >= 2.0 omniORB4 >= 4.1.0])
PKG_CHECK_MODULES([INTERFACEDEPS], [bulkio >= 2.0])
OSSIE_ENABLE_LOG4CXX
AX_BOOST_BASE([1.41])
AX_BOOST_SYSTEM
AX_BOOST_THREAD
AX_BOOST_REGEX

PKG_CHECK_MODULES(IPP, ipp >= 1.0,[HAVE_IPP=1],[HAVE_IPP=0])
if test "$HAVE_IPP" -eq 1; then
AC_SUBST(IPP_CFLAGS)
AC_SUBST(IPP_LIBS)
CXXFLAGS="$CXXFLAGS $IPP_CFLAGS -DHAVE_IPP"
LIBS="$LIBS $IPP_LIBS"
else
PKG_CHECK_MODULES(FFTW, fftw3f >= 3.0.0,,exit)
AC_SUBST(FFTW_CFLAGS)
AC_SUBST(FFTW_LIBS)
CXXFLAGS="$CXXFLAGS $FFTW_CFLAGS"
LIBS="$LIBS $FFTW_LIBS -lfftw3f"
fi

CHECK_VECTOR_IMPL

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

