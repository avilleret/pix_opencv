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
#if HAVE_LIBOPENCV_CL    
#include "opencv2/ocl/ocl.hpp"
#endif /* HAVE_LIBOPENCV_CL */

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
    	pix_opencv_backgroundsubtractor();
    	
  protected:
  
    // Message handling
    void enumParamsMess();
    void setParamMess(t_symbol *s, int argc, t_atom* argv);
    void getParamMess(t_symbol *paramName);
    void paramHelpMess();
    void algoMess(t_symbol *s, int argc, t_atom* argv);
    void cpuModeMess(int val);

    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_backgroundsubtractor();

   	//////////
   	// Do the processing
   	virtual void 	processImage(imageStruct &image);
    virtual void  startRendering();
    virtual void  stopRendering();
    
  private:
    cv::Ptr<cv::BackgroundSubtractor> m_fgbgMOG;
    cv::Ptr<cv::BackgroundSubtractorGMG> m_fgbgGMG;

#if HAVE_LIBOPENCV_CL    
    cv::ocl::MOG m_oclMOG;
    cv::ocl::MOG2 m_oclMOG2;
    //~ cv::ocl::GMG m_oclGMG;
    cv::ocl::oclMat d_input, d_fgmask;
#endif /* HAVE_LIBOPENCV_CL */
    
    cv::Mat m_fgmask, m_segm;
    
    std::string m_algoName;
    
    t_outlet *m_dataout;
    
    t_float m_threshold, m_initFrames;
    bool m_rendering, m_gpuMode, m_forceCPU;
    
    std::vector<std::string> m_bgsub_algos;
  
};
#endif	// for header file
