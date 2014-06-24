dnl Process this file with autoconf to produce a configure script.
AC_INIT([pix_opencv], 0.4, antoine.villeret@gmail.com,[pix_opencv])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src)

AC_SUBST(PD_CPPFLAGS)
AC_SUBST(PD_LDFLAGS)
AC_SUBST(GEM_CPPFLAGS)
AC_SUBST(GEM_LDFLAGS)
AC_SUBST(EXTENSION)

AM_INIT_AUTOMAKE([1.10 foreign])

AC_ARG_WITH([pd],       [  --with-pd=</path/to/pd> where to look for pd-headers and and -libs])
AC_ARG_WITH([gem],       [  --with-gem=</path/to/gem> where to look for gem-headers and and -libs])
AC_ARG_WITH([extension],[  --with-extension=<ext>  enforce a certain extension for the dynamic library (e.g. dll)])

ARCH=$(uname -m)
KERN=$(uname -s)

AC_PROG_CXX
AC_PROG_LIBTOOL

if test "x$with_pd" != "x"; then
 if test -d "${with_pd}"; then
   PDPATH=${with_pd}
 fi
 if test -d "${PDPATH}/src"; then
  AC_MSG_RESULT([adding ${PDPATH}/src to INCLUDES])
  PD_CPPFLAGS="-I${PDPATH}/src ${CPPFLAGS}"

  AC_MSG_RESULT([adding ${PDPATH}/src to LDFLAGS])
  PD_LDFLAGS="-L${PDPATH}/src ${LDFLAGS}"
 else
  if test -d "${PDPATH}"; then
   AC_MSG_RESULT([adding ${PDPATH} to INCLUDES])
   PD_CPPFLAGS="-I${PDPATH} ${CPPFLAGS}"
  fi
 fi
 if test -d "${PDPATH}/bin"; then
  AC_MSG_RESULT([adding ${PDPATH}/bin to LDFLAGS])
  PD_LDFLAGS="-L${PDPATH}/bin ${LDFLAGS}"
 else
  if test -d "${PDPATH}"; then
   AC_MSG_RESULT([adding ${PDPATH} to LDFLAGS])
   PD_LDFLAGS="-L${PDPATH} ${LDFLAGS}"
  fi
 fi
else 
  AC_MSG_RESULT([no pd path given, trying with pkg-config data])
  PD_CPPFLAGS=`pkg-config --cflags pd`
  PD_LDFLAGS=`pkg-config --libs pd`
fi

if test "x$with_gem" != "x"; then
 if test -d "${with_gem}"; then
   GEMPATH=${with_gem}
 fi
 if test -d "${GEMPATH}/src"; then
  AC_MSG_RESULT([adding ${GEMPATH}/src to INCLUDES])
  GEM_CPPFLAGS="-I${GEMPATH}/src ${CPPFLAGS}"

  AC_MSG_RESULT([adding ${GEMPATH}/src to LDFLAGS])
  GEM_LDFLAGS="-L${GEMPATH}/src ${LDFLAGS}"
 else
  if test -d "${GEMPATH}"; then
   AC_MSG_RESULT([adding ${GEMPATH} to INCLUDES])
   GEM_CPPFLAGS="-I${GEMPATH} ${CPPFLAGS}"
  fi
 fi
 if test -d "${GEMPATH}/bin"; then
  AC_MSG_RESULT([adding ${GEMPATH}/bin to LDFLAGS])
  GEM_LDFLAGS="-L${GEMPATH}/bin ${LDFLAGS}"
 else
  if test -d "${GEMPATH}"; then
   AC_MSG_RESULT([adding ${GEMPATH} to LDFLAGS])
   GEM_LDFLAGS="-L${GEMPATH} ${LDFLAGS}"
  fi
 fi
else 
  AC_MSG_RESULT([no Gem path given, trying with pkg-config data])
  GEM_CPPFLAGS=`pkg-config --cflags Gem`
  GEM_LDFLAGS=`pkg-config --libs Gem`
fi

AC_MSG_CHECKING([extension])
if test "x$with_extension" != "x"
then
 EXTENSION=$with_extension
else
 case "$KERN" in
 Darwin)
   EXTENSION=pd_darwin
   ;;
 Linux)
   if test "$ARCH" == x86_64
   then
     EXTENSION=l_ia64
   else
     EXTENSION=l_i386
   fi
   ;;
 *CYGWIN*)
   EXTENSION=dll
   ;;
 *MINGW*)
   EXTENSION=dll
   ;;
 *)
   EXTENSION=pd_linux
   ;;
 esac
fi
AC_MSG_RESULT([$EXTENSION])

#~AC_CHECK_LIB([pd], [error],,
    #~[AC_MSG_ERROR([[missing pd library install puredata and try again.]])
  #~])
#~
#~AC_CHECK_LIB([Gem], [error],,
    #~[AC_MSG_ERROR([[missing Gem library install puredata-gem and try again.]])
  #~])

#~AC_CHECK_LIB([opencv_core], [error],,
    #~[AC_MSG_ERROR([[missing libopencv_core install it and try again.]])
  #~])

#~AC_CHECK_LIB([opencv_calib3d], [findChessboardCorners],,
    #~[AC_MSG_ERROR([[missing a library from opencv try installing libopencv_calib3d]])
  #~])
  
# SIMD support section is from Gem's configure.ac

## SIMD support
# naive approach is to just check, which SIMD-flags the compiler takes
# and use these
# drawback: while the compiler might support compilation for a specific
# SIMD instruction set, the hardware you are compiling for might not

# proposal:
# by default the configure-task should only enable flags that are known
# to work on the machine we are compiling.
# the user can override this by explicitely stating that they want the
# flag to be enabled (if possible)
# e.g. "./configure --enable-sse2" on a pentium-3 should
# produce a binary that will only run on machines with SSE2 registers
# whereas "./configure" will make a binary with only MMX enabled.
#
# but how do we test for the CPU-caps? on linux we could just read /proc/cpuinfo
# but on other OSs?

# we make it a bit complicated in order to allow people to enforce the
# compilation with a specific SIMD-instruction set, even on platforms
# that don't support it
# why? think cross-compilation

GEM_CHECK_FAT(GEM_ARCH)
AC_MSG_RESULT($GEM_ARCH)
AC_SUBST(GEM_ARCH_CXXFLAGS)
AC_SUBST(GEM_ARCH_LDFLAGS)

if test "x${ARCH}" = "xi386" || test "x${ARCH}" = "xi686" || test "x${ARCH}" = "xx86_64"; then
 if test "x$enable_altivec" != "xyes"; then enable_altivec="no"; fi
fi

if test "x${ARCH}" = "xPower Macintosh" || test "x${ARCH}" = "xpowerpc"; then
 if test "x$enable_mmx"  != "xyes"; then enable_mmx="no" ; fi
 if test "x$enable_sse2" != "xyes"; then enable_sse2="no"; fi
fi

if test "x$enable_mmx" != "xno"; then
  GEM_CHECK_CXXFLAGS([-mmmx])
fi
if test "x$enable_sse2" = "xyes"; then
  GEM_CHECK_CXXFLAGS([-msse2])
fi
if test "x$enable_altivec" != "xno"; then
  GEM_CHECK_CXXFLAGS([-faltivec])
fi
if test "x$enable_powerpc" = "xyes"; then
  GEM_CHECK_CXXFLAGS([-mpowerpc-gfxopt])
fi

AC_OUTPUT(Makefile src/Makefile)
