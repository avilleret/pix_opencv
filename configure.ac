dnl Process this file with autoconf to produce a configure script.
AC_INIT(pix_opencv, 0.4, antoine.villeret@gmail.com)
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR(src)

AM_INIT_AUTOMAKE

AC_PROG_CXX

#~AC_CHECK_LIB([pd], [error],,
    #~[AC_MSG_ERROR([[missing pd library install puredata and try again.]])
  #~])
#~
#~AC_CHECK_LIB([Gem], [error],,
    #~[AC_MSG_ERROR([[missing Gem library install puredata-gem and try again.]])
  #~])

AC_CHECK_LIB([opencv_core], [error],,
    [AC_MSG_ERROR([[missing libopencv_core install it and try again.]])
  ])

#~AC_CHECK_LIB([opencv_calib3d], [findChessboardCorners],,
    #~[AC_MSG_ERROR([[missing a library from opencv try installing libopencv_calib3d]])
  #~])

AC_OUTPUT(Makefile src/Makefile)
