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

#ifndef INCLUDE_PIX_OPENCV_MATCHSHAPE_H_
#define INCLUDE_PIX_OPENCV_MATCHSHAPE_H_

#ifndef _EiC
#include "opencv2/legacy/legacy.hpp"

#endif

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_matchshape
    
	square pattern detector
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_matchshape : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_matchshape, GemPixObj)

    public:

	// Constructor
	pix_opencv_matchshape();
    	
    protected:
    // process messages
    void clearMess(void);
    void templateMess(t_symbol*s, int argc, t_atom*argv);
	void contourMess(t_symbol*s, int argc, t_atom*argv);
	void thresholdMess(float arg);
	void methodMess(int arg);
	
   	// Destructor
   	virtual ~pix_opencv_matchshape();

   	// Do the processing
   	virtual void 	processRGBAImage(imageStruct &image);
   	virtual void 	processRGBImage(imageStruct &image);
	virtual void 	processYUVImage(imageStruct &image);
   	virtual void 	processGrayImage(imageStruct &image);

    private:
    
    t_outlet *m_dataout; // info outlet
    double m_threshold;
    int m_method;
    std::vector<std::vector<cv::Point2f> > 	m_template_vec_vec;
    std::vector<cv::Mat> 					m_template_vec_mat;
	    
};
#endif	// for header file
