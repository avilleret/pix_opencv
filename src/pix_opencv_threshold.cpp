#include "pix_opencv_threshold.hpp"
#include "pix_opencv_utils.hpp"

CPPEXTERN_NEW(pix_opencv_threshold)

/////////////////////////////////////////////////////////
//
// pix_opencv_threshold
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_threshold :: pix_opencv_threshold()
{ 
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("max"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("threshold"));
  threshold_value = 50;
  max_value = 255;
  threshold_mode  = 0;
  comp_xsize  = 0;
  comp_ysize  = 0;
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_threshold :: processImage(imageStruct &image)
{
  unsigned char *pixels = image.data;

  auto gray = image2mat_gray(image);

  cv::ThresholdTypes mode;
  
    // Applies fixed-level thresholding to single-channel array.
    switch(this->threshold_mode) {
    	case 0:
        mode = cv::THRESH_BINARY;
        break;
    	case 1:
        mode = cv::THRESH_BINARY_INV;
        break;
    	case 2:
        mode = cv::THRESH_TRUNC;
        break;
    	case 3:
        mode = cv::THRESH_TOZERO;
        break;
    	case 4:
	   break;
    }
    cv::threshold(gray, gray, (float)this->threshold_value, (float)this->max_value, mode);
  
    mat2image(gray, image);
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_threshold :: floatMaxMess (float maxvalue)
{
  if ( (int)maxvalue>0 ) this->max_value = maxvalue;
}
void pix_opencv_threshold :: floatThreshMess (float edge_thresh)
{
  this->threshold_value = edge_thresh;
}
void pix_opencv_threshold :: floatModeMess (float mode)
{
  if ((mode>=0)&&(mode<5)) this->threshold_mode = (int)mode;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_threshold :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_threshold::floatModeMessCallback,
  		  gensym("mode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_threshold::floatMaxMessCallback,
  		  gensym("max"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_threshold::floatThreshMessCallback,
  		  gensym("threshold"), A_FLOAT, A_NULL);
}
void pix_opencv_threshold :: floatMaxMessCallback(void *data, t_floatarg maxvalue)
{
  GetMyClass(data)->floatMaxMess((float)maxvalue);
}
void pix_opencv_threshold :: floatThreshMessCallback(void *data, t_floatarg edge_thresh)
{
  GetMyClass(data)->floatThreshMess((float)edge_thresh);
}
void pix_opencv_threshold :: floatModeMessCallback(void *data, t_floatarg mode)
{
  GetMyClass(data)->floatModeMess((float)mode);
}
