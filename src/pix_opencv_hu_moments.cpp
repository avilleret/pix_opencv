#include "pix_opencv_hu_moments.hpp"
#include "pix_opencv_utils.hpp"

#include <opencv2/imgproc.hpp>

CPPEXTERN_NEW(pix_opencv_hu_moments)

/////////////////////////////////////////////////////////
//
// pix_opencv_hu_moments
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_hu_moments :: pix_opencv_hu_moments()
{ 
    m_dataout = outlet_new(this->x_obj, &s_anything);

    comp_xsize = 320;
    comp_ysize = 240;

    x_binary = 0;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_hu_moments :: ~pix_opencv_hu_moments()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_hu_moments :: processImage(imageStruct &image)
{
  cv::Mat gray = image2mat_gray(image);

  auto mmts = cv::moments(gray, x_binary);
  double hu[7];
  cv::HuMoments(mmts, hu);

  for(int i = 0; i<7; i++)
    SETFLOAT(&rlist[i], hu[i]);

  outlet_list( m_dataout, 0, 7, rlist );
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_hu_moments :: floatBinaryMess (float binary)
{
  if ( ((int)binary==1) || ((int)binary==0) ) x_binary = (int)binary;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_hu_moments :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_hu_moments::floatBinaryMessCallback,
  		  gensym("binary"), A_FLOAT, A_NULL);
}

void pix_opencv_hu_moments :: floatBinaryMessCallback(void *data, t_floatarg binary)
{
  GetMyClass(data)->floatBinaryMess((float)binary);
}

