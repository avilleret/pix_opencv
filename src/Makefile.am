#~bin_PROGRAMS = pix_opencv
AUTOMAKE_OPTIONS = foreign
pkglib_LTLIBRARIES = pix_opencv.la

pix_opencv_la_CXXFLAGS = @GEM_CPPFLAGS@ @PD_CPPFLAGS@  @FACETRACKER_CPPFLAGS@ @OPENCV_CFLAGS@
pix_opencv_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @GEM_LDFLAGS@ @PD_LDFLAGS@ @FACETRACKER_LDFLAGS@ @OPENCV_LDFLAGS@
pix_opencv_la_LIBADD = @OPENCV_LIBS@
pix_opencv_la_includedir=$(includedir)/src
pix_opencv_la_include_HEADERS = Blob.hpp \
					blobtrack.h \
					pix_opencv_athreshold.h \
					pix_opencv_backgroundsubtractor.h \
					pix_opencv_bgsubstract.h \
					pix_opencv_blobtrack.h \
					pix_opencv_calibration.h \
					pix_opencv_camshift.h \
					pix_opencv_clahe.h \
					pix_opencv_colorfilt.h \
					pix_opencv_contours_boundingrect.h \
					pix_opencv_contours_convexity.h \
					pix_opencv_contours.h \
					pix_opencv_dft.h \
					pix_opencv_distrans.h \
					pix_opencv_edge.h \
					pix_opencv_facetracker.h \
					pix_opencv_findchessboardcorners.h \
					pix_opencv_floodfill.h \
					pix_opencv_haarcascade.h \
					pix_opencv_hist_compare.h \
					pix_opencv_hough_circles.h \
					pix_opencv_hough_lines.h \
					pix_opencv_hu_compare.h \
					pix_opencv_hu_moments.h \
					pix_opencv_knear.h \
					pix_opencv_laplace.h \
					pix_opencv_lk.h \
					pix_opencv_matchshape.h \
					pix_opencv_morphology.h \
					pix_opencv_motempl.h \
					pix_opencv_opticalflow.h \
					pix_opencv_patreco.h \
					pix_opencv_pgh_compare.h \
					pix_opencv_surf.h \
					pix_opencv_template.h \
					pix_opencv_threshold.h \
					pix_opencv_trackKnn.h \
					pix_opencv_warpperspective.h

OVERVIEW=../pix_opencv-help.pd

pix_opencv_la_SOURCES = pix_opencv_edge.cc \
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
				  pix_opencv_camshift.cc \
				  pix_opencv_hu_compare.cc \
				  pix_opencv_pgh_compare.cc \
				  pix_opencv_hough_circles.cc \
				  pix_opencv_hough_lines.cc \
				  pix_opencv_hu_moments.cc \
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
				  pix_opencv_backgroundsubtractor.cc \
				  pix_opencv_clahe.cc \
				  pix_opencv.cc \
				  pix_opencv_surf.cc \
				  pix_opencv_facetracker.cc

#~ Override pix_opencv.la target to add ".WAIT" but it doesn't seems to work...
#~pix_opencv.la: .WAIT $(pix_opencv_la_OBJECTS) $(pix_opencv_la_DEPENDENCIES) $(EXTRA_pix_opencv_la_DEPENDENCIES) 
#~$(AM_V_CXXLD)$(pix_opencv_la_LINK) -rpath $(pkglibdir) $(pix_opencv_la_OBJECTS) $(pix_opencv_la_LIBADD) $(LIBS)
#~
#~.WAIT: 

## uha, this is ugly
$(abs_builddir)/.libs/pix_opencv.@GEXTENSION@: pix_opencv.la

$(top_builddir)/pix_opencv.@EXTENSION@: $(abs_builddir)/.libs/pix_opencv.@EXTENSION@ 
		rm -f $@ && cd $(top_builddir) && test -e $< && $(LN_S) $< pix_opencv.@EXTENSION@ || true

.PHONY: clean-conveniencelink help

clean-conveniencelink:
	test -L $(top_builddir)/pix_opencv.@EXTENSION@ && rm -f $(top_builddir)/pix_opencv.@EXTENSION@ || true


all-local:: $(top_builddir)/pix_opencv.@EXTENSION@ help

clean-local:: clean-conveniencelink

.o: .cc
	$(CXX) $(ALL_CFLAGS) -o "$*.o" -c "$*.cc"

help:
	echo "#N canvas 147 197 1566 537 10;" > $(OVERVIEW)
	echo "#X text 126 15 overview of all available pix_opencv objects;" >> $(OVERVIEW)
	echo "#X obj 30 20 pix_opencv;" >> $(OVERVIEW)
	ID=0 ; \
	for extern in $(pix_opencv_la_SOURCES:.cc=""); do \
		echo "#X obj `expr $$ID % 5 \* 300 + 50` `expr $$ID / 5 \* 30 + 40` $$extern;" >> $(OVERVIEW) && ID=`expr $$ID + 1` ; \
	done ;
