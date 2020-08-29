#include "pix_opencv_colorfilt.hpp"
#include "pix_opencv_utils.hpp"

#include <g_canvas.h>
#include <opencv2/imgproc.hpp>

CPPEXTERN_NEW(pix_opencv_colorfilt)

/////////////////////////////////////////////////////////
//
// pix_opencv_colorfilt
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_colorfilt :: pix_opencv_colorfilt()
  : x_color{128,128,128,0}
{ 
  inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("R"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("G"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, &s_float, gensym("B"));

  x_R  = outlet_new(this->x_obj, &s_float);
  x_G  = outlet_new(this->x_obj, &s_float);
  x_B  = outlet_new(this->x_obj, &s_float);

  outlet_float( x_R, x_color[0] );
  outlet_float( x_G, x_color[1] );
  outlet_float( x_B, x_color[2] );

  comp_xsize=320;
  comp_ysize=240;

  x_tolerance = 10;

  x_canvas = canvas_getcurrent();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_colorfilt :: ~pix_opencv_colorfilt()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_colorfilt :: processImage(imageStruct &image)
{
  m_input = image2mat(image);

  cv::Mat tmp = (m_input - x_color) / 3.;
  cv::Mat mask;

  cv::threshold(tmp, mask, x_tolerance, 255, cv::THRESH_BINARY);

  cv::Mat out = cv::Mat::zeros(m_input.rows, m_input.cols, m_input.type());
  m_input.copyTo(out,mask);
  mat2image(out, image);
}

void pix_opencv_colorfilt :: floatToleranceMess (float tolerance)
{
  if ( (int)tolerance>0 ) x_tolerance = (int)tolerance;
}

void pix_opencv_colorfilt :: floatRMess (float r)
{
  if ( ( (int)r>=0 ) && ( (int)r<=255 ) ) x_color[0] = (int)r;
}

void pix_opencv_colorfilt :: floatGMess (float g)
{
  if ( ( (int)g>=0 ) && ( (int)g<=255 ) ) x_color[1] = (int)g;
}

void pix_opencv_colorfilt :: floatBMess (float b)
{
  if ( ( (int)b>=0 ) && ( (int)b<=255 ) ) x_color[2] = (int)b;
}

void pix_opencv_colorfilt :: pickMess (float xcur, float ycur)
{
   if ( ( xcur >= 0. ) && ( xcur <= comp_xsize )
        && ( ycur > 0. ) && ( ycur < comp_ysize ) )
   {
     x_color = m_input.at<cv::Vec3i>(ycur, xcur);

     outlet_float( x_R, x_color[0] );
     outlet_float( x_G, x_color[1] );
     outlet_float( x_B, x_color[2] );
   }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_colorfilt :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_colorfilt::floatToleranceMessCallback,
  		  gensym("tolerance"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_colorfilt::floatRMessCallback,
  		  gensym("R"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_colorfilt::floatGMessCallback,
  		  gensym("G"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_colorfilt::floatBMessCallback,
  		  gensym("B"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_colorfilt::pickMessCallback,
  		  gensym("pick"), A_FLOAT, A_FLOAT, A_NULL);
}

void pix_opencv_colorfilt :: floatToleranceMessCallback(void *data, t_floatarg tolerance)
{
  GetMyClass(data)->floatToleranceMess((float)tolerance);
}

void pix_opencv_colorfilt :: floatRMessCallback(void *data, t_floatarg r)
{
  GetMyClass(data)->floatRMess((float)r);
}

void pix_opencv_colorfilt :: floatGMessCallback(void *data, t_floatarg g)
{
  GetMyClass(data)->floatGMess((float)g);
}

void pix_opencv_colorfilt :: floatBMessCallback(void *data, t_floatarg b)
{
  GetMyClass(data)->floatBMess((float)b);
}

void pix_opencv_colorfilt :: pickMessCallback(void *data, t_floatarg xcur, t_floatarg ycur)
{
  GetMyClass(data)->pickMess((float)xcur,(float)ycur);
}
