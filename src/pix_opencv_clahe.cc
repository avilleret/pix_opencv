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

CPPEXTERN_NEW_WITH_THREE_ARGS(pix_opencv_clahe, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT);

/////////////////////////////////////////////////////////
//
// pix_opencv_clahe
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_clahe :: pix_opencv_clahe(t_float clipLimit, int width, int height)
  : m_clipLimit(40), m_tileGridSize(Size(8,8)), m_rendering(false)
{ 
  if ( clipLimit > 0. ){
    m_clipLimit = clipLimit;
  }
  if ( width > 0 && height > 0 ){
    m_tileGridSize=cv::Size(width, height);
  } else if (width > 0 || height > 0) {
    int max = width > height ? width : height;
    m_tileGridSize=Size ( max, max );
  }
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
  
  ocl::DevicesInfo devicesInfo;
  ocl::getOpenCLDevices(devicesInfo);
  post("Found %d OpenCL device(s).", devicesInfo.size());
  for ( size_t i = 0; i < devicesInfo.size(); i++){
    post("%s %s", devicesInfo[i]->deviceVendor.c_str(), devicesInfo[i]->deviceName.c_str());
  }
 
  m_cpuFilter = createCLAHE();
 
  if ( devicesInfo.size() == 0 ){
    post("can't find OpenCL device, switch to CPU mode");
    m_gpuMode = false;
  } else {
    m_oclFilter = ocl::createCLAHE();
    m_gpuMode = true;
  }
  clipLimitMess(m_clipLimit);
  tileGridSizeMess(m_tileGridSize.width,m_tileGridSize.height);
  
  m_rendering = true;
}

void pix_opencv_clahe :: stopRendering(){
  m_rendering = false;
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
  
  if ( m_gpuMode ) {
    try  {
      d_outframe = m_gray;
      m_oclFilter->apply(d_outframe, d_outframe);
      d_outframe.download(m_gray);
    } catch (cv::Exception& e) {
      error("can't use OpenCL, do you have OpenCL driver installed ?");
      error("error %d : %s", e.code, e.err.c_str());
      m_gpuMode = false;
      return;
    }
  } else {
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

void pix_opencv_clahe :: clipLimitMess(t_float limit){
  m_clipLimit=limit;
  if ( m_rendering ){
    if ( m_gpuMode ){
      m_oclFilter->setClipLimit(m_clipLimit);
    } else {
      m_cpuFilter->setClipLimit(m_clipLimit);
    }
  }
}

void pix_opencv_clahe :: tileGridSizeMess(int width, int height){
  m_tileGridSize=cv::Size(MAX(width,1),MAX(height,1));
  if ( m_rendering ){
    if ( m_gpuMode ){
      m_oclFilter->setTilesGridSize(m_tileGridSize);
    } else {
      m_cpuFilter->setTilesGridSize(m_tileGridSize);
    }
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_clahe :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "clipLimit",   clipLimitMess, t_float);
  CPPEXTERN_MSG2(classPtr, "tileGridSize",   tileGridSizeMess, int, int);
}
