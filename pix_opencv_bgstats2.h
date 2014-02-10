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

#ifndef INCLUDE_PIX_OPENCV_BGSTATS2_H_
#define INCLUDE_PIX_OPENCV_BGSTATS2_H_

#include "opencv2/opencv.hpp"

#include "Base/GemPixObj.h"
#include <RTE/MessageCallbacks.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_bgstats2
    
	square pattern detector
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_bgstats2 : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_bgstats2, GemPixObj)

  public:

	    //////////
	    // Constructor
    	pix_opencv_bgstats2(t_floatarg threshold, t_floatarg initFrames);
    	
  protected:
  
    // Message handling
    void initFramesMess(double arg);
    void thresholdMess(double arg);

    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_bgstats2();

   	//////////
   	// Do the processing
   	virtual void 	processImage(imageStruct &image);

  private:
    cv::Ptr<cv::BackgroundSubtractorGMG> m_fgbg;
    cv::Mat m_fgmask, m_segm;
    
    t_float m_threshold, m_initFrames;
  
};
#endif	// for header file
