AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src help examples

ALL_CFLAGS = -I"$(PD_INCLUDE)"

#pkg-config support
## this is a bit hacky: we abused $(libdir) to point to .../lib/pd/extra
## so we cannot use it for the pkgconfigdir
## instead, we have to build it crom scratch
#pkgconfigdir = $(libdir)/pkgconfig
pkgconfigdir = ${exec_prefix}/lib/pkgconfig
pkgconfig_DATA = pix_opencv.pc

pix_opencvdir=$(pkglibdir)

dist_pix_opencv_DATA = \
	pix_opencv-meta.pd \
	COPYING.txt \
	LICENSE.txt \
	README.txt

EXTRA_DIST = autogen.sh
