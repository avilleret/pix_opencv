////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
// based on code written by Lluis Gomez i Bigorda ( lluisgomez _at_ hangar _dot_ org ) (pix_opencv)
// Template for pix_opencv class

#include "pix_opencv_bgstats2.h"
#include "Gem/Exception.h"
#include <RTE/MessageCallbacks.h>

using namespace cv;

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_opencv_bgstats2, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT);

/////////////////////////////////////////////////////////
//
// pix_opencv_bgstats2
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_bgstats2 :: pix_opencv_bgstats2(t_floatarg threshold, t_floatarg initFrames)
    : m_threshold(threshold), m_initFrames(initFrames)
{ 
  initModule_video();
  setUseOptimized(true);
  setNumThreads(8);
  
  m_fgbg = Algorithm::create<BackgroundSubtractorGMG>("BackgroundSubtractor.GMG");
  if (m_fgbg.empty())
  {
    throw(GemException("Failed to create BackgroundSubtractor.GMG Algorithm."));
  }
  
  if ( m_initFrames == 0. ) m_initFrames = 20;
  if ( m_threshold == 0. ) m_threshold = 0.7;
  
  m_fgbg->set("initializationFrames", m_initFrames);
  m_fgbg->set("decisionThreshold", m_threshold);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_bgstats2 :: ~pix_opencv_bgstats2()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////    	
void pix_opencv_bgstats2 :: processImage(imageStruct &image)
{ 
  Mat imgMat;
  if ( image.csize == 1 ){
    imgMat = Mat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
  } else if ( image.csize == 4 ){
    imgMat = Mat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
  } else {
    verbose(1,"suport only RGBA or GRAY image");
    return;
  }
  
  (*m_fgbg)(imgMat, m_fgmask);
  
  if ( image.csize == 1 ){ // if grayscale, send out fgmask
    image.data = m_fgmask.data;
  } else { // else, set only alpha channel
    std::vector<cv::Mat> split;
    cv::split(imgMat, split);
    split.pop_back(); // delete alpha channel
    split.push_back(m_fgmask); // add fgmask as alpha channel
    cv::merge(split, imgMat);
  }
}

void pix_opencv_bgstats2 :: initFramesMess(double val){
  m_fgbg->set("initializationFrames", val);
}

void pix_opencv_bgstats2 :: thresholdMess(double val){
  m_fgbg->set("decisionThreshold", val);
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_bgstats2 :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "initFrames",  initFramesMess,     double);
  CPPEXTERN_MSG1(classPtr, "threshold",   thresholdMess,     double);
}
