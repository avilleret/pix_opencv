#pragma once
#include <opencv2/opencv.hpp>

#include "Base/GemPixObj.h"

#define MAX_HISTOGRAMS_TO_COMPARE 80

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_hist_compare
    
    Histogram reognition object using Open CV

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/

class GEM_EXPORT pix_opencv_hist_compare : public GemPixObj
{
  CPPEXTERN_HEADER(pix_opencv_hist_compare, GemPixObj)

  public:

    //////////
    // Constructor
    pix_opencv_hist_compare();

  protected:

  //////////
  // Destructor
  virtual ~pix_opencv_hist_compare();

  //////////
  // Do the processing
  virtual void 	processImage(imageStruct &image);

  void  saveMess(float index);

  t_outlet *m_dataout;
  t_outlet *m_measureout;

  private:
    
    //////////
    // Static member functions
    static void     saveMessCallback(void *data, t_floatarg index);

    std::vector<std::vector<cv::Mat>> saved_hists;

    // The output and temporary images
    int save_now;
};
