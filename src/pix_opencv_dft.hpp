#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_dft
    
    Discrete Fourier Transform

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_dft : public GemPixObj
{
  CPPEXTERN_HEADER(pix_opencv_dft, GemPixObj)

  public:

    //////////
    // Constructor
    pix_opencv_dft();

  protected:

    //////////
    // Destructor
    ~pix_opencv_dft();

    //////////
    // Do the processing
    void 	processImage(imageStruct &image) override;

    int x_calculate{1};

  private:
    
    //////////
    // Static member functions
    static void 	calculateCallback(void *data);
};
