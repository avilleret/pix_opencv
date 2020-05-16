#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Base/GemPixObj.h"

#define MAX_HISTOGRAMS_TO_COMPARE 80

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_hough_circles
    
    Hough circles detection algorithm

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/

class GEM_EXPORT pix_opencv_hough_circles : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_hough_circles, GemPixObj)

    public:

	//////////
	// Constructor
    	pix_opencv_hough_circles();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_hough_circles();

    	//////////
    	// Do the processing
      virtual void 	processImage(imageStruct &image);

        int comp_xsize;
        int comp_ysize;

        void     floatNightModeMess(t_float nightmode);
        void     floatThresholdMess(t_floatarg threshold);
        void     floatThreshold2Mess(t_floatarg threshold);
        void     floatMinDistMess(t_floatarg mindist);
        void     floatResolutionMess(t_floatarg resolution);
        void     floatMaxCirclesMess(t_floatarg maxcircles);

        t_outlet *m_dataout;

        int x_threshold;
        int x_threshold2;
        int x_maxcircles;
        double x_mindist;
        double x_resolution;
        int night_mode;

    private:
    
    	//////////
    	// Static member functions
        static void     floatNightModeMessCallback(void *data, t_floatarg nightmode);
        static void     floatThresholdMessCallback(void *data, t_floatarg threshold);
        static void     floatThreshold2MessCallback(void *data, t_floatarg threshold);
        static void     floatMinDistMessCallback(void *data, t_floatarg mindist);
        static void     floatResolutionMessCallback(void *data, t_floatarg resolution);
        static void     floatMaxCirclesMessCallback(void *data, t_floatarg maxcircles);

	// The output and temporary images
        t_atom x_list[4];
};
