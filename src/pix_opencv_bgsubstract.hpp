#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_bgsubstract
    
    Change pix to greyscale

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_bgsubstract : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_bgsubstract, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_bgsubstract();

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_bgsubstract();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    //////////
    // Set the new edge threshold
    void	    	floatThreshMess(float x_threshold);
    void	    	SetMess();
    // The new threshold
    int 		x_threshold;
    int 		x_set;

    // to detect changes in the image size
    int 	comp_xsize;
    int		comp_ysize;

  private:
    
    //////////
    // Static member functions
    static void 	floatTreshMessCallback(void *data, t_floatarg thresh_value);
    static void 	SetMessCallback(void *data);

    cv::Mat orig;
    cv::Mat rgb;
    cv::Mat gray;
    cv::Mat prev_gray;
    cv::Mat grayLow;
    cv::Mat grayUp;
    cv::Mat diff_8U;

};
