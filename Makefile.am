AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src help examples

ALL_CFLAGS = -I"$(PD_INCLUDE)"

pix_opencvdir=$(pkglibdir)

dist_pix_opencv_DATA = \
	pix_opencv-meta.pd \
	COPYING.txt \
	LICENSE.txt \
	README.txt \
	pix_opencv-help.pd

EXTRA_DIST = autogen.sh
