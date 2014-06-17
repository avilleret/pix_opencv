AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src
EXTRA_DIST = autogen.sh

ALL_CFLAGS = -I"$(PD_INCLUDE)"  `pkg-config --cflags opencv`
LIBS = `pkg-config --libs pd` `pkg-config --libs Gem` `pkg-config --libs opencv`
