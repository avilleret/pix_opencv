## Pd library template version 1.0.14
# For instructions on how to use this template, see:
#  http://puredata.info/docs/developer/MakefileTemplate
LIBRARY_NAME = pix_opencv

# add your .cc source files, one object per file, to the SOURCES
# variable, help files will be included automatically, and for GUI
# objects, the matching .tcl file too
SOURCES = pix_opencv_edge.cc \
				  pix_opencv_laplace.cc \
				  pix_opencv_morphology.cc \
				  pix_opencv_distrans.cc \
				  pix_opencv_motempl.cc \
				  pix_opencv_haarcascade.cc \
				  pix_opencv_contours_boundingrect.cc \
				  pix_opencv_bgsubstract.cc \
				  pix_opencv_contours_convexity.cc \
				  pix_opencv_dft.cc \
				  pix_opencv_lk.cc \
				  pix_opencv_hist_compare.cc \
				  pix_opencv_knear.cc \
				  pix_opencv_threshold.cc \
				  pix_opencv_floodfill.cc \
				  pix_opencv_athreshold.cc \
				  pix_opencv_bgstats.cc \
				  pix_opencv_camshift.cc \
				  pix_opencv_hu_compare.cc \
				  pix_opencv_pgh_compare.cc \
				  pix_opencv_hough_circles.cc \
				  pix_opencv_hough_lines.cc \
				  pix_opencv_hu_moments.cc \
				  pix_opencv_contours_convexhull.cc \
				  pix_opencv_colorfilt.cc \
				  pix_opencv_of_bm.cc \
				  pix_opencv_of_hs.cc \
				  pix_opencv_of_lk.cc \
				  pix_opencv_calibration.cc \
				  pix_opencv_warpperspective.cc \
				  pix_opencv_findchessboardcorners.cc \
				  pix_opencv_blobtrack.cc \
				  pix_opencv_contours.cc \
				  pix_opencv_matchshape.cc \
				  pix_opencv_opticalflow.cc \
				  pix_opencv_trackKnn.cc \
				  pix_opencv_surf.cc \
					pix_opencv_backgroundsubtractor.cc
					
HEADERS = `ls -1 *.h *.hpp`

# list all pd objects (i.e. myobject.pd) files here, and their helpfiles will
# be included automatically
PDOBJECTS =

# the surces examples folder itself and some other data (will be copied as is)
EXAMPLES = 00_opencv_haarcascade+pix_image 01_opencv_contrours_boundingrect+pmpd_ambient 02_opencv_motempl+particle_system 03_opencv_hist_compare+text_trigger 04_opencv_colorfilt+video_projector 05_perspective_correction 06_GPU_opticalflow 07_shape_extractor 08_GPU_morphology 09_vp_calibration 10-fast_blobtracker   

# manuals and related files, in the 'manual' subfolder
MANUAL =

# if you want to include any other files in the source and binary tarballs,
# list them here.  This can be anything from header files, test patches,
# documentation, etc.  README.txt and LICENSE.txt are required and therefore
# automatically included
OVERVIEW = pix_opencv-overview.pd
EXTRA_DIST = dessin.tif $(OVERVIEW)
EXTRA_DIST_FOLDER = model haarcascades plus
EXTRA_DIST_FOLDER = model haarcascades


# unit tests and related files here, in the 'unittests' subfolder
UNITTESTS = 



#------------------------------------------------------------------------------#
#
# things you might need to edit if you are using other C libraries
#
#------------------------------------------------------------------------------#

ALL_CFLAGS = -I"$(PD_INCLUDE)"  `pkg-config --cflags opencv` -IFaceTracker/include/
CFLAGS_linux = `pkg-config --cflags Gem` `pkg-config --cflags pd` 
CFLAGS_macosx = -I$(PD_PATH)/include/Gem -I$(PD_PATH)/include
FAT_FLAGS=-arch i386
ALL_LDFLAGS =  
SHARED_LDFLAGS =
FACETRACKER_LIBS += FaceTracker/src/lib/CLM.o  FaceTracker/src/lib/FCheck.o  FaceTracker/src/lib/FDet.o  FaceTracker/src/lib/IO.o  FaceTracker/src/lib/Patch.o  FaceTracker/src/lib/PAW.o  FaceTracker/src/lib/PDM.o  FaceTracker/src/lib/Tracker.o
ALL_LIBS += `pkg-config --libs opencv`

#------------------------------------------------------------------------------#
#
# you shouldn't need to edit anything below here, if we did it right :)
#
#------------------------------------------------------------------------------#

# these can be set from outside without (usually) breaking the build
CFLAGS = -Wall -W -g
LDFLAGS =
LIBS =

# get library version from meta file
LIBRARY_VERSION = $(shell sed -n 's|^\#X text [0-9][0-9]* [0-9][0-9]* VERSION \(.*\);|\1|p' $(LIBRARY_NAME)-meta.pd)

ALL_CFLAGS += -DPD -DVERSION='"$(LIBRARY_VERSION)"'

PD_INCLUDE = $(PD_PATH)/include/pd
# where to install the library, overridden below depending on platform
prefix = /usr/local
libdir = $(prefix)/lib
pkglibdir = $(libdir)/pd-externals
objectsdir = $(pkglibdir)

INSTALL = install
INSTALL_PROGRAM = $(INSTALL) -p -m 644
INSTALL_DATA = $(INSTALL) -p -m 644
INSTALL_DIR     = $(INSTALL) -p -m 755 -d

ALLSOURCES := $(SOURCES) $(SOURCES_android) $(SOURCES_cygwin) $(SOURCES_macosx) \
	         $(SOURCES_iphoneos) $(SOURCES_linux) $(SOURCES_windows)

DISTDIR=$(LIBRARY_NAME)-$(LIBRARY_VERSION)
ORIGDIR=pd-$(LIBRARY_NAME:~=)_$(LIBRARY_VERSION)

UNAME := $(shell uname -s)
ifeq ($(UNAME),Darwin)
  CPU := $(shell uname -p)
  ifeq ($(CPU),arm) # iPhone/iPod Touch
    SOURCES += $(SOURCES_iphoneos)
    EXTENSION = pd_darwin
    SHARED_EXTENSION = dylib
    OS = iphoneos
    PD_PATH = /Applications/Pd-extended.app/Contents/Resources
    IPHONE_BASE=/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin
    CC=$(IPHONE_BASE)/gcc
    CPP=$(IPHONE_BASE)/cpp
    CXX=$(IPHONE_BASE)/g++
    ISYSROOT = -isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS3.0.sdk
    IPHONE_CFLAGS = -miphoneos-version-min=3.0 $(ISYSROOT) -arch armv6
    OPT_CFLAGS = -fast -funroll-loops -fomit-frame-pointer
    ALL_CFLAGS := $(IPHONE_CFLAGS) $(ALL_CFLAGS)
    ALL_LDFLAGS += -arch armv6 -bundle -undefined dynamic_lookup $(ISYSROOT)
    SHARED_LDFLAGS += -arch armv6 -dynamiclib -undefined dynamic_lookup $(ISYSROOT)
    ALL_LIBS += -lc $(LIBS_iphoneos)
    STRIP = strip -x
    DISTBINDIR=$(DISTDIR)-$(OS)
  else # Mac OS X
    SOURCES += $(SOURCES_macosx)
    EXTENSION = pd_darwin
    SHARED_EXTENSION = dylib
    OS = macosx
    PD_PATH = /Applications/Pd-extended.app/Contents/Resources
    OPT_CFLAGS = -ftree-vectorize -ftree-vectorizer-verbose=2 -fast
# build universal 32-bit on 10.4 and 32/64 on newer
    ifeq ($(shell uname -r | sed 's|\([0-9][0-9]*\)\.[0-9][0-9]*\.[0-9][0-9]*|\1|'), 8)
#      FAT_FLAGS = -arch ppc -arch i386 -mmacosx-version-min=10.4
    else
      SOURCES += $(SOURCES_iphoneos)
# Starting with Xcode 4.0, the PowerPC compiler is not installed by default
      ifeq ($(wildcard /usr/llvm-gcc-4.2/libexec/gcc/powerpc*), )
#        FAT_FLAGS = -arch i386 -arch x86_64 -mmacosx-version-min=10.5
      else
#        FAT_FLAGS = -arch ppc -arch i386 -arch x86_64 -mmacosx-version-min=10.4
      endif
    endif
    ALL_CFLAGS += $(FAT_FLAGS) -fPIC -I/sw/include $(CFLAGS_macosx)
    # if the 'pd' binary exists, check the linking against it to aid with stripping
    BUNDLE_LOADER = $(shell test ! -e $(PD_PATH)/bin/pd || echo -bundle_loader $(PD_PATH)/bin/pd)
    ALL_LDFLAGS += $(FAT_FLAGS) -headerpad_max_install_names -bundle $(BUNDLE_LOADER) \
	-undefined dynamic_lookup -L/sw/lib
    SHARED_LDFLAGS += $(FAT_FLAGS) -dynamiclib -undefined dynamic_lookup \
	-install_name @loader_path/$(SHARED_LIB) -compatibility_version 1 -current_version 1.0
    ALL_LIBS += -lc $(LIBS_macosx)
    STRIP = strip -x
    DISTBINDIR=$(DISTDIR)-$(OS)
# install into ~/Library/Pd on Mac OS X since /usr/local isn't used much
    pkglibdir=$(HOME)/Library/Pd
  endif
endif
# Tho Android uses Linux, we use this fake uname to provide an easy way to
# setup all this things needed to cross-compile for Android using the NDK
ifeq ($(UNAME),ANDROID)
  CPU := arm
  SOURCES += $(SOURCES_android)
  EXTENSION = so
  SHARED_EXTENSION = so
  OS = android
  PD_PATH = /usr
  NDK_BASE := /usr/local/android-ndk
  NDK_PLATFORM_LEVEL ?= 5
  NDK_ABI=arm
  NDK_SYSROOT=$(NDK_BASE)/platforms/android-$(NDK_PLATFORM_LEVEL)/arch-$(NDK_ABI)
  NDK_UNAME := $(shell uname -s | tr '[A-Z]' '[a-z]')
  NDK_COMPILER_VERSION=4.6
  NDK_TOOLCHAIN=$(wildcard \
	$(NDK_BASE)/toolchains/$(NDK_ABI)*-$(NDK_COMPILER_VERSION)/prebuilt/$(NDK_UNAME)-x86)
  CC := $(wildcard $(NDK_TOOLCHAIN)/bin/*-linux-android*-gcc) --sysroot=$(NDK_SYSROOT)
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  CFLAGS += 
  LDFLAGS += -rdynamic -shared
  SHARED_LDFLAGS += -Wl,-soname,$(SHARED_LIB) -shared
  LIBS += -lc $(LIBS_android)
  STRIP := $(wildcard $(NDK_TOOLCHAIN)/bin/$(NDK_ABI)-linux-android*-strip) \
	--strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq ($(UNAME),Linux)
  CPU := $(shell uname -m)
  EXTENSION = pd_linux
  ifeq ($(findstring arm,$(CPU)),arm)
    EXTENSION = l_arm
  endif
  ifeq ($(CPU),i386)
    EXTENSION = l_i386
  endif
  ifeq ($(CPU),i486)
    EXTENSION = l_i386
  endif
  ifeq ($(CPU),i586)
    EXTENSION = l_i386
  endif
  ifeq ($(CPU),i686)
    EXTENSION = l_i386
  endif
  ifeq ($(CPU),amd64)
    # amd64 and ia64 aren't the same thing, but that's how its done in pd...
    EXTENSION = l_ia64
  endif
  ifeq ($(CPU),x86_64)
    # x86_64 and ia64 aren't the same thing, but that's how its done in pd...
    EXTENSION = l_ia64
  endif
  SOURCES += $(SOURCES_linux)
  SHARED_EXTENSION = so
  OS = linux
  PD_PATH = /usr
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -fPIC $(CFLAGS_linux)
  ALL_LDFLAGS += -rdynamic -shared -fPIC -Wl,-rpath,"\$$ORIGIN",--enable-new-dtags
  SHARED_LDFLAGS += -Wl,-soname,$(SHARED_LIB) -shared
  ALL_LIBS += -lc $(LIBS_linux)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq ($(UNAME),GNU)
  # GNU/Hurd, should work like GNU/Linux for basically all externals
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_linux)
  EXTENSION = pd_linux
  SHARED_EXTENSION = so
  OS = linux
  PD_PATH = /usr
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -fPIC
  ALL_LDFLAGS += -rdynamic -shared -fPIC -Wl,-rpath,"\$$ORIGIN",--enable-new-dtags
  SHARED_LDFLAGS += -shared -Wl,-soname,$(SHARED_LIB)
  ALL_LIBS += -lc $(LIBS_linux)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq ($(UNAME),GNU/kFreeBSD)
  # Debian GNU/kFreeBSD, should work like GNU/Linux for basically all externals
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_linux)
  EXTENSION = pd_linux
  SHARED_EXTENSION = so
  OS = linux
  PD_PATH = /usr
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -fPIC
  ALL_LDFLAGS += -rdynamic -shared -fPIC -Wl,-rpath,"\$$ORIGIN",--enable-new-dtags
  SHARED_LDFLAGS += -shared -Wl,-soname,$(SHARED_LIB)
  ALL_LIBS += -lc $(LIBS_linux)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)-$(shell uname -m)
endif
ifeq (CYGWIN,$(findstring CYGWIN,$(UNAME)))
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_cygwin)
  EXTENSION = dll
  ifeq ($(CPU),i386)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),i486)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),i586)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),i686)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),amd64)
    # amd64 and ia64 aren't the same thing, but that's how its done in pd...
    EXTENSION = m_ia64
  endif
  ifeq ($(CPU),x86_64)
    # x86_64 and ia64 aren't the same thing, but that's how its done in pd...
    EXTENSION = m_ia64
  endif
  SHARED_EXTENSION = $(EXTENSION)
  OS = cygwin
  PD_PATH = $(shell cygpath $$PROGRAMFILES)/pd
  OPT_CFLAGS = -O6 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += 
  ALL_LDFLAGS += -rdynamic -shared -L"$(PD_PATH)/src" -L"$(PD_PATH)/bin"
  SHARED_LDFLAGS += -shared -Wl,-soname,$(SHARED_LIB)
  ALL_LIBS += -lc -lpd $(LIBS_cygwin)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)
endif
ifeq (MINGW,$(findstring MINGW,$(UNAME)))
  CPU := $(shell uname -m)
  SOURCES += $(SOURCES_windows)
  EXTENSION = dll
    ifeq ($(CPU),i386)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),i486)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),i586)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),i686)
    EXTENSION = m_i386
  endif
  ifeq ($(CPU),amd64)
    # amd64 and ia64 aren't the same thing, but that's how its done in pd...
    EXTENSION = m_ia64
  endif
  ifeq ($(CPU),x86_64)
    # x86_64 and ia64 aren't the same thing, but that's how its done in pd...
    EXTENSION = m_ia64
  endif
  SHARED_EXTENSION = $(EXTENSION)
  OS = windows
  PD_PATH = $(shell cd "$$PROGRAMFILES/pd" && pwd)
  # MinGW doesn't seem to include cc so force gcc
  CC=gcc
  OPT_CFLAGS = -O3 -funroll-loops -fomit-frame-pointer
  ALL_CFLAGS += -mms-bitfields
  ALL_LDFLAGS += -s -shared -Wl,--enable-auto-import
  SHARED_LDFLAGS += -shared
  ALL_LIBS += -L"$(PD_PATH)/src" -L"$(PD_PATH)/bin" -L"$(PD_PATH)/obj" \
	-lpd -lwsock32 -lkernel32 -luser32 -lgdi32 -liberty $(LIBS_windows)
  STRIP = strip --strip-unneeded -R .note -R .comment
  DISTBINDIR=$(DISTDIR)-$(OS)
endif

# in case somebody manually set the HELPPATCHES above
HELPPATCHES ?= $(SOURCES:.cc=-help.pd) $(PDOBJECTS:.pd=-help.pd)

ALL_CFLAGS := $(ALL_CFLAGS) $(CFLAGS) $(OPT_CFLAGS)
ALL_LDFLAGS := $(LDFLAGS) $(ALL_LDFLAGS)
ALL_LIBS := $(LIBS) $(ALL_LIBS)

SHARED_SOURCE ?= $(wildcard lib$(LIBRARY_NAME).cc)
SHARED_HEADER ?= $(shell test ! -e $(LIBRARY_NAME).h || echo $(LIBRARY_NAME).h)
SHARED_LIB ?= $(SHARED_SOURCE:.cc=.$(SHARED_EXTENSION))
SHARED_TCL_LIB = $(wildcard lib$(LIBRARY_NAME).tcl)

.PHONY = install libdir_install single_install install-doc install-examples install-manual install-unittests clean distclean dist etags $(LIBRARY_NAME)

all: $(SOURCES:.cc=.$(EXTENSION)) $(SHARED_LIB)

facetracker: pix_opencv_facetracker.$(EXTENSION)

pix_opencv_facetracker.$(EXTENSION): pix_opencv_facetracker.o
	$(CC) $(ALL_LDFLAGS) -o "pix_opencv_facetracker.$(EXTENSION)" "pix_opencv_facetracker.o" $(FACETRACKER_LIBS) $(ALL_LIBS) $(SHARED_LIB)
	chmod a-x "pix_opencv_facetracker.$(EXTENSION)"
	
pix_opencv_facetracker.o: pix_opencv_facetracker.cc
	$(CXX) $(ALL_CFLAGS) -o pix_opencv_facetracker.o -c pix_opencv_facetracker.cc

%.o: %.cc
	$(CXX) $(ALL_CFLAGS) -o "$*.o" -c "$*.cc"

%.$(EXTENSION): %.o $(SHARED_LIB)
	$(CC) $(ALL_LDFLAGS) -o "$*.$(EXTENSION)" "$*.o"  $(ALL_LIBS) $(SHARED_LIB)
	chmod a-x "$*.$(EXTENSION)"

# this links everything into a single binary file
$(LIBRARY_NAME): $(SOURCES:.cc=.o) $(LIBRARY_NAME).o lib$(LIBRARY_NAME).o
	$(CC) $(ALL_LDFLAGS) -o $(LIBRARY_NAME).$(EXTENSION) $(SOURCES:.cc=.o) \
		$(LIBRARY_NAME).o lib$(LIBRARY_NAME).o $(ALL_LIBS)
	chmod a-x $(LIBRARY_NAME).$(EXTENSION)

$(SHARED_LIB): $(SHARED_SOURCE:.cc=.o)
	$(CC) $(SHARED_LDFLAGS) -o $(SHARED_LIB) $(SHARED_SOURCE:.cc=.o) $(ALL_LIBS)

install: libdir_install

# The meta and help files are explicitly installed to make sure they are
# actually there.  Those files are not optional, then need to be there.
libdir_install: $(SOURCES:.cc=.$(EXTENSION)) $(SHARED_LIB) install-doc install-examples install-manual install-unittests
	$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(INSTALL_DATA) $(LIBRARY_NAME)-meta.pd \
		$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(SOURCES))" || (\
		$(INSTALL_PROGRAM) $(SOURCES:.cc=.$(EXTENSION)) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME) && \
		$(STRIP) $(addprefix $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/,$(SOURCES:.cc=.$(EXTENSION))))
	test -z "$(strip $(SHARED_LIB))" || \
		$(INSTALL_DATA) $(SHARED_LIB) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(wildcard $(SOURCES:.cc=.tcl)))" || \
		$(INSTALL_DATA) $(wildcard $(SOURCES:.cc=.tcl)) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(PDOBJECTS))" || \
		$(INSTALL_DATA) $(PDOBJECTS) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(SHARED_TCL_LIB))" || \
		$(INSTALL_DATA) $(SHARED_TCL_LIB) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)

# install library linked as single binary
single_install: $(LIBRARY_NAME) install-doc install-examples install-manual install-unittests
	$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(INSTALL_PROGRAM) $(LIBRARY_NAME).$(EXTENSION) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(STRIP) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/$(LIBRARY_NAME).$(EXTENSION)

install-doc:
	$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	test -z "$(strip $(SOURCES) $(PDOBJECTS))" || \
		$(INSTALL_DATA) $(HELPPATCHES) \
			$(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)
	$(INSTALL_DATA) README.txt $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/README.txt
	$(INSTALL_DATA) LICENSE.txt $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/LICENSE.txt

install-examples:
	test -z "$(strip $(EXAMPLES))" || \
		$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/examples && \
		for file in $(EXAMPLES); do \
			$(INSTALL_DATA) examples/$$file $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/examples; \
		done

install-manual:
	test -z "$(strip $(MANUAL))" || \
		$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/manual && \
		for file in $(MANUAL); do \
			$(INSTALL_DATA) manual/$$file $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/manual; \
		done

install-unittests:
	test -z "$(strip $(UNITTESTS))" || \
		$(INSTALL_DIR) $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/unittests && \
		for file in $(UNITTESTS); do \
			$(INSTALL_DATA) unittests/$$file $(DESTDIR)$(objectsdir)/$(LIBRARY_NAME)/unittests; \
		done

clean:
	-rm -f -- $(SOURCES:.cc=.o) $(SOURCES_LIB:.cc=.o) $(SHARED_SOURCE:.cc=.o)
	-rm -f -- $(SOURCES:.cc=.$(EXTENSION))
	-rm -f -- $(LIBRARY_NAME).o
	-rm -f -- $(LIBRARY_NAME).$(EXTENSION)
	-rm -f -- $(SHARED_LIB)

clean_facetracker:
	-rm -f pix_opencv_facetracker.o
	-rm -f pix_opencv_facetracker.$(EXTENSION)

distclean: clean
	-rm -f -- $(DISTBINDIR).tar.gz
	-rm -rf -- $(DISTBINDIR)
	-rm -f -- $(DISTDIR).tar.gz
	-rm -rf -- $(DISTDIR)
	-rm -f -- $(ORIGDIR).tar.gz
	-rm -rf -- $(ORIGDIR)


$(DISTBINDIR):
	$(INSTALL_DIR) $(DISTBINDIR)

libdir: all $(DISTBINDIR) overview lib3rd
	$(INSTALL_DATA) $(LIBRARY_NAME)-meta.pd  $(DISTBINDIR)
	if [ -s pix_opencv_facetracker.$(EXTENSION) ] ; then \
		$(INSTALL_DATA) pix_opencv_facetracker.$(EXTENSION) $(DISTBINDIR) ; \
	fi;
	$(INSTALL_DATA) $(HELPPATCHES) $(DISTBINDIR)
	$(INSTALL_DATA) $(SOURCES:.cc=.$(EXTENSION)) $(DISTBINDIR)
	test -z "$(strip $(EXAMPLES))" || \
		test -e $(DISTBINDIR)/examples || \
			mkdir $(DISTBINDIR)/examples
		for file in $(EXAMPLES); do \
			cp -r examples/$$file $(DISTBINDIR)/examples; \
		done
	test -z "$(strip $(EXTRA_DIST))" || \
		$(INSTALL_DATA) $(EXTRA_DIST)    $(DISTBINDIR)
	test -z "$(strip $(EXTRA_DIST_FOLDER))" || \
    cp -r $(EXTRA_DIST_FOLDER)    $(DISTBINDIR)
  ifeq ($(OS),windows)
		zip -r $(DISTBINDIR).zip $(DISTBINDIR)
  else
		tar --exclude-vcs -czpf $(DISTBINDIR).tar.gz $(DISTBINDIR)
  endif

#copy 3rd party lib into DISTBINDIR
lib3rd: $(DISTBINDIR)
	if [ $(OS) == windows ] ; then \
		$(INSTALL_DATA) `ls opencv*.dll` $(DISTBINDIR) ; \
	elif [ $(OS) == macosx ] ; then \
		./embed-mac-OpenCV-dependencies.sh && cp -r lib-$(EXTENSION) $(DISTBINDIR) ; \
	fi;

$(DISTDIR):
	$(INSTALL_DIR) $(DISTDIR)

$(ORIGDIR):
	$(INSTALL_DIR) $(ORIGDIR)

dist: $(DISTDIR)
	$(INSTALL_DATA) Makefile  $(DISTDIR)
	$(INSTALL_DATA) README.txt $(DISTDIR)
	$(INSTALL_DATA) LICENSE.txt $(DISTDIR)
	$(INSTALL_DATA) $(LIBRARY_NAME)-meta.pd  $(DISTDIR)
	test -z "$(strip $(ALLSOURCES))" || \
		$(INSTALL_DATA) $(ALLSOURCES)  $(DISTDIR)
	$(INSTALL_DATA) pix_opencv_facetracker.cc  $(DISTDIR)
	test -z "$(strip $(HEADERS))" || \
		$(INSTALL_DATA) $(HEADERS)  $(DISTDIR)
	test -z "$(strip $(wildcard $(ALLSOURCES:.cc=.tcl)))" || \
		$(INSTALL_DATA) $(wildcard $(ALLSOURCES:.cc=.tcl))  $(DISTDIR)
	test -z "$(strip $(wildcard $(LIBRARY_NAME).cc))" || \
		$(INSTALL_DATA) $(LIBRARY_NAME).cc  $(DISTDIR)
	test -z "$(strip $(SHARED_HEADER))" || \
		$(INSTALL_DATA) $(SHARED_HEADER)  $(DISTDIR)
	test -z "$(strip $(SHARED_SOURCE))" || \
		$(INSTALL_DATA) $(SHARED_SOURCE)  $(DISTDIR)
	test -z "$(strip $(SHARED_TCL_LIB))" || \
		$(INSTALL_DATA) $(SHARED_TCL_LIB)  $(DISTDIR)
	test -z "$(strip $(PDOBJECTS))" || \
		$(INSTALL_DATA) $(PDOBJECTS)  $(DISTDIR)
	test -z "$(strip $(HELPPATCHES))" || \
		$(INSTALL_DATA) $(HELPPATCHES) $(DISTDIR)
	test -z "$(strip $(EXTRA_DIST))" || \
		$(INSTALL_DATA) $(EXTRA_DIST)    $(DISTDIR)
	test -z "$(strip $(EXTRA_DIST_FOLDER))" || \
		cp -r $(EXTRA_DIST_FOLDER)    $(DISTDIR)/
	test -z "$(strip $(EXAMPLES))" || \
		$(INSTALL_DIR) $(DISTDIR)/examples && \
		for file in $(EXAMPLES); do \
			cp -r examples/$$file $(DISTDIR)/examples; \
		done
	test -z "$(strip $(MANUAL))" || \
		$(INSTALL_DIR) $(DISTDIR)/manual && \
		for file in $(MANUAL); do \
			$(INSTALL_DATA) manual/$$file $(DISTDIR)/manual; \
		done
	test -z "$(strip $(UNITTESTS))" || \
		$(INSTALL_DIR) $(DISTDIR)/unittests && \
		for file in $(UNITTESTS); do \
			$(INSTALL_DATA) unittests/$$file $(DISTDIR)/unittests; \
		done
	tar --exclude-vcs -czpf $(DISTDIR).tar.gz $(DISTDIR)

# make a Debian source package
dpkg-source:
	debclean
	make distclean dist
	mv $(DISTDIR) $(ORIGDIR)
	tar --exclude-vcs -czpf ../$(ORIGDIR).orig.tar.gz $(ORIGDIR)
	rm -f -- $(DISTDIR).tar.gz
	rm -rf -- $(DISTDIR) $(ORIGDIR)
	cd .. && dpkg-source -b $(LIBRARY_NAME)

etags: TAGS

TAGS: $(wildcard $(PD_INCLUDE)/*.h) $(SOURCES) $(SHARED_SOURCE) $(SHARED_HEADER)
	etags $(wildcard $(PD_INCLUDE)/*.h)
	etags -a *.h $(SOURCES) $(SHARED_SOURCE) $(SHARED_HEADER)
	etags -a --language=none --regex="/proc[ \t]+\([^ \t]+\)/\1/" *.tcl
	
overview:
	echo "#N canvas 147 197 1566 537 10;" > $(OVERVIEW)
	echo "#X text 126 15 overview of all available pix_opencv objects;" >> $(OVERVIEW)
	ID=0 ; \
	for extern in $(SOURCES:.cc=""); do \
		echo "#X obj `expr $$ID % 5 \* 300 + 50` `expr $$ID / 5 \* 30 + 40` $$extern;" >> $(OVERVIEW) && ID=`expr $$ID + 1` ; \
	done ; \
	echo "#X obj 30 `expr $$ID / 5 \* 30 + 80` cnv 15 250 60 empty empty extra 20 12 0 14 -4034 -66577 0;" >> $(OVERVIEW) ; \
	echo "#X obj 50 `expr $$ID / 5 \* 30 + 100` pix_opencv_facetracker;" >> $(OVERVIEW)

	
	

showsetup:
	@echo "CC: $(CC)"
	@echo "CFLAGS: $(CFLAGS)"
	@echo "LDFLAGS: $(LDFLAGS)"
	@echo "LIBS: $(LIBS)"
	@echo "ALL_CFLAGS: $(ALL_CFLAGS)"
	@echo "ALL_LDFLAGS: $(ALL_LDFLAGS)"
	@echo "ALL_LIBS: $(ALL_LIBS)"
	@echo "PD_INCLUDE: $(PD_INCLUDE)"
	@echo "PD_PATH: $(PD_PATH)"
	@echo "objectsdir: $(objectsdir)"
	@echo "LIBRARY_NAME: $(LIBRARY_NAME)"
	@echo "LIBRARY_VERSION: $(LIBRARY_VERSION)"
	@echo "SOURCES: $(SOURCES)"
	@echo "EXTENSION: $(EXTENSION)"
	@echo "SHARED_HEADER: $(SHARED_HEADER)"
	@echo "SHARED_SOURCE: $(SHARED_SOURCE)"
	@echo "SHARED_LIB: $(SHARED_LIB)"
	@echo "SHARED_TCL_LIB: $(SHARED_TCL_LIB)"
	@echo "PDOBJECTS: $(PDOBJECTS)"
	@echo "ALLSOURCES: $(ALLSOURCES)"
	@echo "ALLSOURCES TCL: $(wildcard $(ALLSOURCES:.cc=.tcl))"
	@echo "UNAME: $(UNAME)"
	@echo "CPU: $(CPU)"
	@echo "pkglibdir: $(pkglibdir)"
	@echo "DISTDIR: $(DISTDIR)"
	@echo "ORIGDIR: $(ORIGDIR)"
	@echo "NDK_TOOLCHAIN: $(NDK_TOOLCHAIN)"
	@echo "NDK_BASE: $(NDK_BASE)"
	@echo "NDK_SYSROOT: $(NDK_SYSROOT)"
