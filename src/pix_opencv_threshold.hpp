#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_threshold
    
    Threshold filter

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_threshold : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_threshold, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_threshold();
    	
    protected:
    	
    	// Do the processing
      void 	processImage(imageStruct &image) override;
    	
	//////////
    	// Set the new edge threshold
    	void	    	floatMaxMess(float maxvalue);
    	void	    	floatThreshMess(float edge_thresh);
    	void	    	floatModeMess(float mode);

    	// The new edge threshold
	float 		threshold_value;
    	float       	max_value;
    	int 		threshold_mode;

	// to detect changes in the image size
	int 		comp_xsize;
	int		comp_ysize;

    private:
    
    	//////////
    	// Static member functions
    	static void 	floatMaxMessCallback(void *data, t_floatarg maxvalue);
    	static void 	floatThreshMessCallback(void *data, t_floatarg thresh_value);
    	static void 	floatModeMessCallback(void *data, t_floatarg thresh_mode_value);
};
