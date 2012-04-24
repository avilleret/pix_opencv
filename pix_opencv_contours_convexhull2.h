/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Threshold filter

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_CONTOURS_CONVEXHULL2_H_
#define INCLUDE_PIX_OPENCV_CONTOURS_CONVEXHULL2_H_

#ifndef _EiC
#include "cv.h"
#endif

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_contours_convexhull2
    
	detects contours and send them out
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_contours_convexhull2 : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_contours_convexhull2, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_contours_convexhull2();
    	
    protected:
    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_contours_convexhull2();

   	//////////
   	// Do the processing
   	virtual void 	processRGBAImage(imageStruct &image);
   	virtual void 	processRGBImage(imageStruct &image);
	virtual void 	processYUVImage(imageStruct &image);
   	virtual void 	processGrayImage(imageStruct &image);

	// Messages handling
	void epsilonMess(double arg);
	void areaMess(double arg);

    private:
    
    t_outlet *m_contourout; // contour outlet
    t_outlet *m_dataout; // info outlet
    std::vector<std::vector<cv::Point> > m_contours;	
    double m_area_threshold;	// min area for contour
    double m_epsilon;
	    
};
#endif	// for header file
