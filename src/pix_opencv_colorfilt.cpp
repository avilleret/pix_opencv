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
  : x_color{128,128,128}
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

  x_tolerance = 50;

  x_canvas = canvas_getcurrent();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_colorfilt :: ~pix_opencv_colorfilt()
{ 
}

void pix_opencv_colorfilt :: drawColor()
{
  int width, height;
  char color[32];

  sprintf( color, "#%.2X%.2X%.2X", x_color[0], x_color[1], x_color[2]);
  width = rtext_width( glist_findrtext( (t_glist*)x_canvas, (t_text *)this->x_obj ) );
  height = rtext_height( glist_findrtext( (t_glist*)x_canvas, (t_text *)this->x_obj ) );
  sys_vgui((char*)".x%x.c delete rectangle %xCOLOR\n", x_canvas, this->x_obj );
  sys_vgui((char*)".x%x.c create rectangle %d %d %d %d -fill %s -tags %xCOLOR\n",
           x_canvas, this->x_obj->te_xpix+width+5, this->x_obj->te_ypix,
           this->x_obj->te_xpix+width+height+5,
           this->x_obj->te_ypix+height, color, this->x_obj );
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_colorfilt :: processImage(imageStruct &image)
{
  bgr = image2mat(image);

  cv::Mat tmp = (bgr - x_color) / 3.;
  cv::Mat mask;

  cv::threshold(tmp, mask, x_tolerance, 255, cv::THRESH_BINARY);

  bgr *= mask;
}

void pix_opencv_colorfilt :: floatToleranceMess (float tolerance)
{
  if ( (int)tolerance>0 ) x_tolerance = (int)tolerance;
}

void pix_opencv_colorfilt :: floatRMess (float r)
{
  if ( ( (int)r>=0 ) && ( (int)r<=255 ) ) x_color[0] = (int)r;
  if (glist_isvisible(x_canvas)) drawColor();
}

void pix_opencv_colorfilt :: floatGMess (float g)
{
  if ( ( (int)g>=0 ) && ( (int)g<=255 ) ) x_color[1] = (int)g;
  if (glist_isvisible(x_canvas)) drawColor();
}

void pix_opencv_colorfilt :: floatBMess (float b)
{
  if ( ( (int)b>=0 ) && ( (int)b<=255 ) ) x_color[2] = (int)b;
  if (glist_isvisible(x_canvas)) drawColor();
}

void pix_opencv_colorfilt :: pickMess (float xcur, float ycur)
{
   if ( ( xcur >= 0. ) && ( xcur <= comp_xsize )
        && ( ycur > 0. ) && ( ycur < comp_ysize ) )
   {
     x_color = bgr.at<cv::Vec3i>(ycur, xcur);

     outlet_float( x_R, x_color[0] );
     outlet_float( x_G, x_color[1] );
     outlet_float( x_B, x_color[2] );

      if (glist_isvisible(x_canvas)) drawColor();
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
