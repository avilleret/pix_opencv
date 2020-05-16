#include "m_pd.h"

extern "C" void pix_opencv_calibration_setup();
extern "C" void pix_opencv_athreshold_setup();
#ifdef HAVE_BGSEGM
extern "C" void pix_opencv_backgroundsubtractor_setup();
#endif
extern "C" void pix_opencv_bgsubstract_setup();
extern "C" void pix_opencv_camshift_setup();
extern "C" void pix_opencv_clahe_setup();
extern "C" void pix_opencv_colorfilt_setup();
extern "C" void pix_opencv_contours_setup();
extern "C" void pix_opencv_dft_setup();
extern "C" void pix_opencv_distrans_setup();
extern "C" void pix_opencv_edge_setup();
#ifdef HAVE_OBJDETECT
extern "C" void pix_opencv_facetracker_setup();
#endif
extern "C" void pix_opencv_findchessboardcorners_setup();
extern "C" void pix_opencv_floodfill_setup();
extern "C" void pix_opencv_haarcascade_setup();
extern "C" void pix_opencv_hist_compare_setup();
extern "C" void pix_opencv_hough_circles_setup();
extern "C" void pix_opencv_hu_compare_setup();


extern "C" {

typedef struct pix_opencv
{
  t_object x_ob;
} t_pix_opencv;

t_class *pix_opencv_class;

    /* this is called when a new "pix_opencv" object is created. */
void *pix_opencv_new(void)
{
    t_pix_opencv *x = (t_pix_opencv *)pd_new(pix_opencv_class);
    return (void *)x;
}

void pix_opencv_setup(void)
{
    verbose(-1,"pix_opencv library by Antoine Villeret et al. 2012-2020");
    verbose(-1,"\tbased on work by Yves Degoyon and Lluis Gomez i Bigorda");
    verbose(-1,"\tbuilt on %s",__DATE__);
#if HAVE_LIBOPENCV_NONFREE
    verbose(-1,"\twith non-free features");
#else
    verbose(-1,"\twithout non-free features");
#endif
#ifdef HAVE_FACETRACKER
    verbose(-1,"\twith FaceTracker.");
#else
    verbose(-1,"\tand without FaceTracker.");
#endif
    pix_opencv_class = class_new(gensym("pix_opencv"), (t_newmethod)pix_opencv_new, 0,
    	sizeof(t_pix_opencv), 0, (t_atomtype) 0);

    pix_opencv_athreshold_setup();
#ifdef HAVE_BGSEGM
    pix_opencv_backgroundsubtractor_setup();
#endif
    pix_opencv_bgsubstract_setup();
    pix_opencv_calibration_setup();
    pix_opencv_camshift_setup();
    pix_opencv_clahe_setup();
    pix_opencv_colorfilt_setup();
    pix_opencv_contours_setup();
    pix_opencv_dft_setup();
    pix_opencv_distrans_setup();
    pix_opencv_edge_setup();
#ifdef HAVE_OBJDETECT
    pix_opencv_facetracker_setup();
#endif
    pix_opencv_findchessboardcorners_setup();
    pix_opencv_floodfill_setup();
    pix_opencv_haarcascade_setup();
    pix_opencv_hist_compare_setup();
    pix_opencv_hough_circles_setup();
    pix_opencv_hu_compare_setup();
}

} // extern "C"
