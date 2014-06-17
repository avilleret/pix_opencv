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

#ifndef INCLUDE_PIX_OPENCV_CLAHE_H_
#define INCLUDE_PIX_OPENCV_CLAHE_H_

#include "opencv2/opencv.hpp"
#include "opencv2/ocl/ocl.hpp"


#include "Base/GemPixObj.h"
#include "RTE/MessageCallbacks.h"
#include "Gem/Exception.h"


using namespace cv;


/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_clahe
    	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_clahe : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_clahe, GemPixObj)

  public:

	    //////////
	    // Constructor
    	pix_opencv_clahe(t_float clipLimit, int width, int height);
    	
      /////////
      // Message handler
      void clipLimitMess(float);
      void tileGridSizeMess(int,int);
  protected:
    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_clahe();

   	//////////
   	// Do the processing
   	virtual void 	processImage(imageStruct &image);
    virtual void  startRendering();
    virtual void  stopRendering();
  
  private:
  
    Mat m_imgMat, m_gray;
    ocl::oclMat d_outframe, d_frame;
    
    Ptr<CLAHE> m_oclFilter, m_cpuFilter;
    
    bool m_gpuMode;
    float m_clipLimit;
    Size m_tileGridSize;
    bool m_rendering;
};
#endif	// for header file
