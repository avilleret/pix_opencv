#pragma once
#include <opencv2/opencv.hpp>

#include <Base/GemPixObj.h>
#include <RTE/MessageCallbacks.h>
#include <Gem/Exception.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_clahe

KEYWORDS
    pix

DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_clahe : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_clahe, GemPixObj)

    public:

      pix_opencv_clahe(t_float clipLimit, int width, int height);

    void clipLimitMess(float);
    void tileGridSizeMess(int,int);
  protected:

    virtual ~pix_opencv_clahe();

    virtual void 	processImage(imageStruct &image);
    virtual void  startRendering();
    virtual void  stopRendering();

  private:

    cv::Mat m_imgMat, m_gray;

    cv::Ptr<cv::CLAHE> m_filter{};

    bool m_gpuMode;
    float m_clipLimit;
    cv::Size m_tileGridSize;
    bool m_rendering;
};
