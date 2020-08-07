#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <Base/GemPixObj.h>

#define MAX_MARKERS 500
const int MAX_COUNT = 500;

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_camshift

    Continously adaptive mean-shift tracker

KEYWORDS
    pix

DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_camshift : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_camshift, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_camshift();

  protected:

    //////////
    // Destructor
    ~pix_opencv_camshift();

    //////////
    // Do the processing
    void 	processImage(imageStruct &image) override;

    void  backProjectMess(float backproject);
    void  vMinMess(float vmin);
    void  vMaxMess(float vmax);
    void  sMinMess(float smin);
    void  trackMess(float px, float py);
    void  rWidthMess(float rwidth);
    void  rHeightMess(float rheight);

    cv::Size m_last_size{};

    t_outlet *m_dataout;
    int     x_track{0};
    int     x_init{0};
    int     x_rwidth{20};
    int     x_rheight{20};
    int     x_backproject{0};
    int     x_vmin{50};
    int     x_vmax{256};
    int     x_smin{30};

  private:

    //////////
    // Static member functions
    static void  backProjectMessCallback(void *data, float backproject);
    static void  vMinMessCallback(void *data, float vmin);
    static void  vMaxMessCallback(void *data, float vmax);
    static void  sMinMessCallback(void *data, float smin);
    static void  trackMessCallback(void *data, float px, float py);
    static void  rWidthMessCallback(void *data, float rwidth);
    static void  rHeightMessCallback(void *data, float rheight);

    // Internal Open CV data
    cv::Mat gray;
    cv::Mat hsv;
    cv::Mat hue;
    cv::Mat mask;
    cv::Mat backproject;
    cv::Point origin;
    cv::Rect selection;
    cv::Rect trackwindow;

    t_atom x_list[5];
};
