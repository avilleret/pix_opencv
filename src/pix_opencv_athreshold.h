#pragma once
#include <opencv2/core.hpp>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_athreshold
    
    Adaptive threshold object

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_athreshold : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_athreshold, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_athreshold();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_athreshold();

    	//////////
    	// Do the processing
      virtual void 	processImage(imageStruct &image);
    	
	//////////
    	// Set the new edge threshold
    	void	    	floatMaxValueMess(float maxvalue);
    	void	    	floatModeMess(float mode);
    	void	    	floatMethodMess(float method);
    	void	    	floatBlockSizeMess(float blocksize);
    	void	    	floatDimMess(float dim);

    	// The new edge threshold
        int max_value;
        int x_threshold_mode;
        int x_threshold_method;
        int x_blocksize;
        int x_dim;

    private:
    
    	//////////
    	// Static member functions
    	static void    	floatMaxValueMessCallback(void *data, float maxvalue);
    	static void    	floatModeMessCallback(void *data, float mode);
    	static void    	floatMethodMessCallback(void *data, float method);
    	static void    	floatBlockSizeMessCallback(void *data, float blocksize);
    	static void   	floatDimMessCallback(void *data, float dim);
};
