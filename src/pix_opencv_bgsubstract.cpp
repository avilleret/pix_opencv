#include "pix_opencv_bgsubstract.hpp"
#include "pix_opencv_utils.hpp"
#include <opencv2/imgproc.hpp>

CPPEXTERN_NEW(pix_opencv_bgsubstract)

/////////////////////////////////////////////////////////
//
// pix_opencv_bgsubstract
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_bgsubstract :: pix_opencv_bgsubstract()
{ 
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("ft1"));
  x_threshold = 13;
  x_set       = 1;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_bgsubstract :: ~pix_opencv_bgsubstract()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_bgsubstract :: processGrayImage(imageStruct &image)
{ 
    cv::Mat gray = image2mat(image);

    if (x_set) {
      prev_gray = gray.clone();
      x_set=0;
    } 

    cv::subtract(prev_gray,cv::Scalar(x_threshold), grayLow);
    cv::add(prev_gray,cv::Scalar(x_threshold), grayUp);
    cv::inRange (gray, grayLow, grayUp, diff_8U);

    cv::bitwise_not(diff_8U,diff_8U);

    mat2image(diff_8U, image);
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_bgsubstract :: floatThreshMess (float x_threshold)
{
  this->x_threshold = (int)x_threshold;
}
void pix_opencv_bgsubstract :: SetMess ()
{
  this->x_set = 1;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_bgsubstract :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_bgsubstract::floatTreshMessCallback,
  		  gensym("ft1"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_bgsubstract::SetMessCallback,
  		  gensym("set"), A_NULL);
}
void pix_opencv_bgsubstract :: floatTreshMessCallback(void *data, t_floatarg x_threshold)
{
  GetMyClass(data)->floatThreshMess((float)x_threshold);
}
void pix_opencv_bgsubstract :: SetMessCallback(void *data)
{
  GetMyClass(data)->SetMess();
}
