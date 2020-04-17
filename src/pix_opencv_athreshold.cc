#include "pix_opencv_athreshold.h"
#include <opencv2/imgproc.hpp>

CPPEXTERN_NEW(pix_opencv_athreshold)

/////////////////////////////////////////////////////////
//
// pix_opencv_athreshold
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_athreshold :: pix_opencv_athreshold()
{ 
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("max_value"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("blocksize"));

  max_value = 255;
  x_threshold_mode  = 0;
  x_threshold_method  = cv::ADAPTIVE_THRESH_MEAN_C;
  x_blocksize  = 3;
  x_dim = 0;

  comp_xsize=320;
  comp_ysize=240;

  rgba = cv::Mat(cv::Size(comp_xsize,comp_ysize), CV_8UC4);
  rgb = cv::Mat(cv::Size(comp_xsize,comp_ysize), CV_8UC3);
  gray = cv::Mat(cv::Size(comp_xsize,comp_ysize), CV_8UC1);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_athreshold :: ~pix_opencv_athreshold()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_athreshold :: processImage(imageStruct &image)
{
  cv::Mat mat;
  switch(image.csize)
  {
    case 1:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC1, image.data);
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
    default:
      error("image format not supported");
      return;
  }
  
  // Applies fixed-level thresholding to single-channel array.
  switch(x_threshold_mode) {
    case 0:
      cv::adaptiveThreshold(mat, mat, (float)max_value, x_threshold_method, cv::THRESH_BINARY, x_blocksize, x_dim);
      break;
    case 1:
      cv::adaptiveThreshold(mat, mat, (float)max_value, x_threshold_method, cv::THRESH_BINARY_INV, x_blocksize, x_dim);
      break;
  }
}

void pix_opencv_athreshold :: floatMaxValueMess (float maxvalue)
{
  if ( (int)maxvalue>0 ) max_value = (int)maxvalue;
}

void pix_opencv_athreshold :: floatModeMess (float mode)
{
  if ( ( (int)mode==0 ) || ( (int)mode==1 ) ) x_threshold_mode = (int)mode;
}

void pix_opencv_athreshold :: floatMethodMess (float method)
{
  if ( (int)method==cv::ADAPTIVE_THRESH_MEAN_C ) x_threshold_method = cv::ADAPTIVE_THRESH_MEAN_C;
  if ( (int)method==cv::ADAPTIVE_THRESH_GAUSSIAN_C ) x_threshold_method = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
}

void pix_opencv_athreshold :: floatBlockSizeMess (float blocksize)
{
  if ( ( (int)blocksize>=3 ) && ( (int)(blocksize+1)%2 == 0 ) )
  {
    x_blocksize = (int)blocksize;
  }
}

void pix_opencv_athreshold :: floatDimMess (float dim)
{
  if ( (int)dim>0 ) x_dim = (int)dim;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_athreshold :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_athreshold::floatModeMessCallback,
                  gensym("mode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_athreshold::floatMethodMessCallback,
                  gensym("method"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_athreshold::floatMaxValueMessCallback,
                  gensym("max_value"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_athreshold::floatBlockSizeMessCallback,
                  gensym("blocksize"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_athreshold::floatDimMessCallback,
                  gensym("dim"), A_FLOAT, A_NULL);
}

void pix_opencv_athreshold :: floatModeMessCallback(void *data, t_floatarg mode)
{
  GetMyClass(data)->floatModeMess((float)mode);
}

void pix_opencv_athreshold :: floatMethodMessCallback(void *data, t_floatarg method)
{
  GetMyClass(data)->floatMethodMess((float)method);
}

void pix_opencv_athreshold :: floatMaxValueMessCallback(void *data, t_floatarg maxvalue)
{
  GetMyClass(data)->floatMaxValueMess((float)maxvalue);
}

void pix_opencv_athreshold :: floatBlockSizeMessCallback(void *data, t_floatarg blocksize)
{
  GetMyClass(data)->floatBlockSizeMess((float)blocksize);
}

void pix_opencv_athreshold :: floatDimMessCallback(void *data, t_floatarg dim)
{
  GetMyClass(data)->floatDimMess((float)dim);
}
