#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_laplace
    
    Laplace transform / edge detection

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_laplace : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_laplace, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_laplace();

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_laplace();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    //////////
    // Set the new aperture_size
    void floatApertureMess(float aperture_size);
    // The new aperture size
    int aperture_size;
    // to detect changes in the image size
    int comp_xsize;
    int	comp_ysize;

  private:
    
    //////////
    // Static member functions
    static void 	floatApertureMessCallback(void *data, t_floatarg aperture_size);

    cv::Mat m_laplacian;
    cv::Mat m_out;
};
