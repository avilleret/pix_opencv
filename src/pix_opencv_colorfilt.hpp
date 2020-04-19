#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_colorfilt
    
    Color filter object

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_colorfilt : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_colorfilt, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_colorfilt();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_colorfilt();

    	//////////
    	// Do the processing
      virtual void 	processImage(imageStruct &image);
    	
	//////////
    	// Set the new edge threshold
    	void	    	floatToleranceMess(float tolerance);
    	void	    	floatRMess(float r);
    	void	    	floatGMess(float g);
    	void	    	floatBMess(float b);
    	void	    	pickMess(float xcur, float ycur);
    	void	    	drawColor(void);

    	// The color tolerance
        int x_tolerance;
        cv::Vec3i x_color;

        t_outlet *x_R;  // output R component of selected color
        t_outlet *x_G;  // output G component of selected color
        t_outlet *x_B;  // output B component of selected color

        t_canvas *x_canvas;

	// to detect changes in the image size
	int 		comp_xsize;
	int		comp_ysize;

    private:

      cv::Mat bgr;
    	//////////
    	// Static member functions
    	static void    	floatToleranceMessCallback(void *data, float tolerance);
    	static void    	floatRMessCallback(void *data, float r);
    	static void    	floatGMessCallback(void *data, float g);
    	static void    	floatBMessCallback(void *data, float b);
    	static void   	pickMessCallback(void *data, float xcur, float ycur);

};
