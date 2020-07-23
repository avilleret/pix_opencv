#pragma once
#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>

#include "Base/GemPixObj.h"

#define MAX_MARKERS 500
const int MAX_COUNT = 500;

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_lk

    LK point detection and tracking

KEYWORDS
    pix

DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_lk : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_lk, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_lk();

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_lk();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    void  winSizeMess(float winsize);
    void  nightModeMess(float nightmode);
    void  qualityMess(float quality);
    void  initMess(void);
    void  markMess(int, t_atom*);
    void  deleteMess(float index);
    void  clearMess(void);
    void  minDistanceMess(float mindistance);
    void  maxMoveMess(float maxmove);
    void  ftoleranceMess(float ftolerance);
    void  delaunayMess(t_symbol *s);
    void  pdelaunayMess(float fpoint, float fthreshold);

    int comp_xsize{};
    int comp_ysize{};

    t_outlet *m_dataout;
    t_atom x_list[3];
    int win_size;
    double quality;
    int min_distance;
    int night_mode;
    int maxmove;
    int markall;
    int ftolerance;
    int delaunay;
    int threshold;

  private:

    //////////
    // Static member functions
    static void  winSizeMessCallback(void *data, t_floatarg winsize);
    static void  nightModeMessCallback(void *data, t_floatarg nightmode);
    static void  qualityMessCallback(void *data, t_floatarg quality);
    static void  initMessCallback(void *data);
    static void  markMessCallback(void *data, t_symbol* name, int argc, t_atom* argv);
    static void  deleteMessCallback(void *data, t_floatarg index);
    static void  clearMessCallback(void *data);
    static void  minDistanceMessCallback(void *data, t_floatarg mindistance);
    static void  maxMoveMessCallback(void *data, t_floatarg maxmove);
    static void  ftoleranceMessCallback(void *data, t_floatarg ftolerance);
    static void  delaunayMessCallback(void *data, t_symbol *s);
    static void  pdelaunayMessCallback(void *data, t_floatarg fpoint, t_floatarg fthreshold);

    int x_xmark[MAX_MARKERS];
    int x_ymark[MAX_MARKERS];
    int x_found[MAX_MARKERS];
    std::vector<cv::Point2f> points[2];
    std::vector<uchar> status;
    int count;
    int need_to_init;
    int flags;
    int add_remove_pt;
    cv::Point pt;

    // structures needed for the delaunay
    cv::Rect x_fullrect;

    cv::Mat gray, prev_gray;
    cv::Subdiv2D x_subdiv;
};
