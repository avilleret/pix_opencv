#~bin_PROGRAMS = pix_opencv
AUTOMAKE_OPTIONS = foreign
pkglib_LTLIBRARIES = pix_opencv.la

pix_opencv_la_CXXFLAGS = @GEM_CPPFLAGS@ @GEM_ARCH_CXXFLAGS@ @PD_CPPFLAGS@  `pkg-config --cflags pd`
pix_opencv_la_LDFLAGS = -module -avoid-version -shared -shrext .@EXTENSION@  @GEM_LDFLAGS@ @GEM_ARCH_LDFLAGS@ @PD_LDFLAGS@ `pkg-config --libs opencv`
#~pix_opencv_la_HEADERS = `ls -1 *.h *.hpp`

#~pix_opencv_la_SOURCES = pix_opencv_edge.cc
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
				  #~pix_opencv_surf.cc \
				  pix_opencv_backgroundsubtractor.cc \
				  pix_opencv_clahe.cc \
				  pix_opencv.cc

## uha, this is ugly
$(abs_builddir)/.libs/pix_opencv.@GEXTENSION@: pix_opencv.la

$(top_builddir)/pix_opencv.@EXTENSION@: $(abs_builddir)/.libs/pix_opencv.@EXTENSION@ 
		rm -f $@ && cd $(top_builddir) && test -e $< && $(LN_S) $< pix_opencv.@EXTENSION@ || true

.PHONY: clean-conveniencelink

clean-conveniencelink:
	test -L $(top_builddir)/pix_opencv.@EXTENSION@ && rm -f $(top_builddir)/pix_opencv.@EXTENSION@ || true


all-local:: $(top_builddir)/pix_opencv.@EXTENSION@

clean-local:: clean-conveniencelink

.o: .cc
	$(CXX) $(ALL_CFLAGS) -o "$*.o" -c "$*.cc"
