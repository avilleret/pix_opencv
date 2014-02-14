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

#include "pix_opencv_clahe.h"

using namespace cv;

CPPEXTERN_NEW(pix_opencv_clahe)

/////////////////////////////////////////////////////////
//
// pix_opencv_clahe
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_clahe :: pix_opencv_clahe()
{ 
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_clahe :: ~pix_opencv_clahe()
{ 
}

// StartRendering
void pix_opencv_clahe :: startRendering(){
  
  m_oclFilter = ocl::createCLAHE();
  m_cpuFilter = createCLAHE(); 
}

/////////////////////////////////////////////////////////
// processImage
//
///////////////////////////////////////////////////////// 	
void pix_opencv_clahe :: processImage(imageStruct &image)
{ 
  if ( image.csize == 1 ){
    m_gray = Mat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
  } else if (image.csize == 4) {
    m_imgMat = Mat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
    cvtColor(m_imgMat,m_gray,CV_RGBA2GRAY);
  } else {
    error("only support grayscale and RGBA image");
  }
  
  try {
    d_outframe = m_gray;
    m_oclFilter->apply(d_outframe, d_outframe);
    d_outframe.download(m_gray);
  } catch (...) {
    m_cpuFilter->apply(m_gray, m_gray);
  }
  
  if ( image.csize == 4 ){
    std::vector<Mat> split;
    cv::split(m_imgMat,split);
    split.pop_back();
    split.push_back(m_gray);
    cv::merge(split,m_imgMat);    
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_clahe :: obj_setupCallback(t_class *classPtr)
{
}
