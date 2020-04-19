#include "pix_opencv_clahe.hpp"
#include "pix_opencv_utils.hpp"

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
  : m_clipLimit(40), m_tileGridSize(cv::Size(8,8)), m_rendering(false)
{ 
  if ( clipLimit > 0. ){
    m_clipLimit = clipLimit;
  }
  if ( width > 0 && height > 0 ){
    m_tileGridSize=cv::Size(width, height);
  } else if (width > 0 || height > 0) {
    int max = width > height ? width : height;
    m_tileGridSize=cv::Size ( max, max );
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

  m_filter = createCLAHE();

  clipLimitMess(m_clipLimit);
  tileGridSizeMess(m_tileGridSize.width,m_tileGridSize.height);
  m_rendering = true;
}

void pix_opencv_clahe :: stopRendering(){
  m_rendering = false;
  m_filter.reset();
}

/////////////////////////////////////////////////////////
// processImage
//
///////////////////////////////////////////////////////// 	
void pix_opencv_clahe :: processImage(imageStruct &image)
{ 
  cv::Mat mat = image2mat(image);

  m_filter->apply(mat, mat);
}

void pix_opencv_clahe :: clipLimitMess(t_float limit){
  m_clipLimit=limit;
  if(m_filter)
    m_filter->setClipLimit(m_clipLimit);
}

void pix_opencv_clahe :: tileGridSizeMess(int width, int height){
  m_tileGridSize=cv::Size(MAX(width,1),MAX(height,1));
  if(m_filter)
    m_filter->setTilesGridSize(m_tileGridSize);
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
