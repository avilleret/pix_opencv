AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src help examples

ALL_CFLAGS = -I"$(PD_INCLUDE)"

pix_opencvdir=$(pkglibdir)

dist_pix_opencv_DATA = \
	pix_opencv-meta.pd \
	COPYING.txt \
	LICENSE.txt \
	README.txt
# pix_opencv-help.pd

EXTRA_DIST = \
 m4 \
 configure.ac \
 pix_opencv-meta.pd.in \
 Makefile.am \
 autogen.sh

DISTDIR=$(PACKAGE_NAME)-$(PACKAGE_VERSION)
ORIGDIR=pd-$(PACKAGE_NAME)_$(PACKAGE_VERSION)
OS=$(shell uname -s)
DISTBINDIR=$(DISTDIR)-$(OS)

libdir: all
	$(INSTALL) -p -m 755 -d $(DISTBINDIR)
	$(INSTALL_DATA) $(PACKAGE_NAME)-meta.pd  $(DISTBINDIR)
	$(INSTALL_DATA) $(PACKAGE_NAME).$(EXTENSION)  $(DISTBINDIR)
	$(INSTALL_DATA) $(dist_pix_opencv_DATA) $(DISTBINDIR)
	test -z "$(strip $(EXTRA_DIST))" || \
		rsync -a $(EXTRA_DIST)    $(DISTBINDIR)
	test -z "$(strip $(SUBDIRS))" || \
	  rsync -a --exclude 'Makefile' --exclude 'Makefile.in' $(SUBDIRS) $(DISTBINDIR)
#	tar --exclude-vcs -czpf $(DISTBINDIR).tar.gz $(DISTBINDIR)
