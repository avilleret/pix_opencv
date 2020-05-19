#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Base/GemPixObj.h"

#define MAX_HISTOGRAMS_TO_COMPARE 80

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_hough_lines

    Hough lines detection algorithm

KEYWORDS
    pix

DESCRIPTION

-----------------------------------------------------------------*/

class GEM_EXPORT pix_opencv_hough_lines : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_hough_lines, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_hough_lines();

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_hough_lines();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    int comp_xsize;
    int comp_ysize;

    void     floatNightModeMess(t_float nightmode);
    void     floatModeMess(t_floatarg mode);
    void     floatThresholdMess(t_floatarg threshold);
    void     floatMinLengthMess(t_floatarg minlength);
    void     floatGapMess(t_floatarg gap);
    void     floatAResolutionMess(t_floatarg aresolution);
    void     floatDResolutionMess(t_floatarg dresolution);
    void     floatMaxLinesMess(t_floatarg maxlines);

    t_outlet *m_dataout;

    int x_mode;
    int x_threshold;
    int x_maxlines;
    double x_minlength;
    double x_gap;
    double x_aresolution;
    double x_dresolution;
    int night_mode;

  private:

    //////////
    // Static member functions
    static void     floatNightModeMessCallback(void *data, t_float nightmode);
    static void     floatModeMessCallback(void *data, t_floatarg mode);
    static void     floatThresholdMessCallback(void *data, t_floatarg threshold);
    static void     floatMinLengthMessCallback(void *data, t_floatarg minlength);
    static void     floatGapMessCallback(void *data, t_floatarg gap);
    static void     floatAResolutionMessCallback(void *data, t_floatarg aresolution);
    static void     floatDResolutionMessCallback(void *data, t_floatarg dresolution);
    static void     floatMaxLinesMessCallback(void *data, t_floatarg maxlines);

    // The output and temporary images
    t_atom x_list[5];
};
