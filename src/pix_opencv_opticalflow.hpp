/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) G�nther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_OPTICALFLOW_H_
#define INCLUDE_PIX_OPENCV_OPTICALFLOW_H_

#include "opencv2/opencv.hpp"
#include "Base/GemBase.h"
#include "Gem/Exception.h"
#include "Gem/State.h"
#include "Base/GemPixObj.h"
#include "RTE/MessageCallbacks.h"
#include <stdio.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
  pix_opencv_opticalflow
  apply a simple flow method to compute optical flow

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/

static cv::Vec3b computeColor(float fx, float fy);
inline bool isFlowCorrect(cv::Point2f u);

class GEM_EXPORT pix_opencv_opticalflow : public GemPixObj
{
  CPPEXTERN_HEADER(pix_opencv_opticalflow, GemPixObj)

  public:

  //////////
	// Constructor
  pix_opencv_opticalflow();
      
  protected:
    
  //////////
  // Destructor
  virtual ~pix_opencv_opticalflow();

  //////////
  // Do the processing
  virtual void processImage(imageStruct &image);

  // Messages handling
  void gainMess(double arg);
  void normalizeMess(double arg);

  private:
  
  cv::Mat prev, flow;
  t_float m_gain;
  bool m_normalize;
    
  cv::Mat m_prev, m_curr, m_flow;
  
  t_outlet *m_dataout_middle; // contour outlet
  t_outlet *m_dataout_right; // info outlet
};
#endif	// for header file
