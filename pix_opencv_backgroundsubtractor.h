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

#ifndef INCLUDE_PIX_OPENCV_BACKGROUNDSUBTRACTOR_H_
#define INCLUDE_PIX_OPENCV_BACKGROUNDSUBTRACTOR_H_

#include "opencv2/opencv.hpp"

#include "Base/GemPixObj.h"
#include <RTE/MessageCallbacks.h>
#include "Gem/Exception.h"

#include <iostream>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_backgroundsubtractor
    
    generic dynamic background subtractor, should implement severals algo
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_backgroundsubtractor: public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_backgroundsubtractor, GemPixObj)

  public:

	    //////////
	    // Constructor
    	pix_opencv_backgroundsubtractor(t_floatarg threshold, t_floatarg initFrames);
    	
  protected:
  
    // Message handling
    void enumParamsMess();
    void setParamMess(t_symbol *s, int argc, t_atom* argv);
    void getParamMess(t_symbol *paramName);
    void paramHelpMess();
    void algoMess(t_symbol *s, int argc, t_atom* argv);

    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_backgroundsubtractor();

   	//////////
   	// Do the processing
   	virtual void 	processImage(imageStruct &image);

  private:
    //~cv::Ptr<cv::BackgroundSubtractor> m_fgbg;
    cv::Ptr<cv::BackgroundSubtractor> m_fgbgMOG;
    cv::Ptr<cv::BackgroundSubtractorGMG> m_fgbgGMG;
    cv::Mat m_fgmask, m_segm;
    
    t_outlet *m_dataout;
    
    t_float m_threshold, m_initFrames;
    
    std::vector<std::string> m_bgsub_algos;
  
};
#endif	// for header file
