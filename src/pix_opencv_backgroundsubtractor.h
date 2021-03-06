/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Threshold filter

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) G�nther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_BACKGROUNDSUBTRACTOR_H_
#define INCLUDE_PIX_OPENCV_BACKGROUNDSUBTRACTOR_H_

#include <opencv2/bgsegm.hpp>

#include "Base/GemPixObj.h"
#include <RTE/MessageCallbacks.h>
#include "Gem/Exception.h"

#include <iostream>
#include <functional>
#include <map>
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
    cv::Ptr<cv::bgsegm::BackgroundSubtractorMOG> m_fgbgMOG;
    cv::Ptr<cv::bgsegm::BackgroundSubtractorGMG> m_fgbgGMG;


    struct MOGparam
    {
      std::string name;
      std::string description; 
      std::function<float(const cv::bgsegm::BackgroundSubtractorMOG&)> get;
      std::function<void(cv::bgsegm::BackgroundSubtractorMOG&, float)> set;
    };

    struct GMGparam
    {
      std::string name;
      std::string description; 
      std::function<float(const cv::bgsegm::BackgroundSubtractorGMG&)> get;
      std::function<void(cv::bgsegm::BackgroundSubtractorGMG&, float)> set;
    };
  
    std::vector<MOGparam> m_mog_params;
    std::vector<GMGparam> m_gmg_params;

    cv::Mat m_fgmask, m_segm;
    
    std::string m_algoName;
    
    t_outlet *m_dataout;
    
    t_float m_threshold, m_initFrames;
    bool m_rendering, m_gpuMode, m_forceCPU;
    
    std::vector<std::string> m_bgsub_algos;
  
};
#endif	// for header file
