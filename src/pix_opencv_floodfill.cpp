#include "pix_opencv_floodfill.hpp"
#include "pix_opencv_utils.hpp"
#include <stdio.h>

CPPEXTERN_NEW(pix_opencv_floodfill)

/////////////////////////////////////////////////////////
//
// pix_opencv_floodfill
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_floodfill :: pix_opencv_floodfill()
{ 
  int i;

  comp_xsize=320;
  comp_ysize=240;

  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("lo_diff"));
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("up_diff"));
  m_dataout = outlet_new(this->x_obj, &s_anything);

  x_lo = 20;
  x_up = 20;
  x_connectivity = 4;
  x_color = 1;

  for ( i=0; i<MAX_COMPONENTS; i++)
  {
       x_xcomp[i] = -1;
       x_ycomp[i] = -1;
       x_r[i] = rand() & 255;
       x_g[i] = rand() & 255;
       x_b[i] = rand() & 255;
  }
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_floodfill :: ~pix_opencv_floodfill()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_floodfill :: processImage(imageStruct &image)
{
  int i, k;
  int im;
  int marked;
  int flags = x_connectivity + ( 255 << 8 ) + cv::FLOODFILL_FIXED_RANGE;

  auto grey = image2mat_gray(image);
  auto rgb = image2mat_bgr(image);
  // cvCvtColor(rgba, rgb, CV_BGRA2BGR);
  cv::Rect rect;
  // mark recognized components
  for ( i=0; i<MAX_COMPONENTS; i++ )
  {
     if ( x_xcomp[i] != -1 )
     {
       if ( x_color )
       {
          cv::Point seed(x_xcomp[i],x_ycomp[i]);
          cv::Scalar color = CV_RGB( x_r[i], x_g[i], x_b[i] );
          cv::floodFill( rgb, seed, color, &rect, CV_RGB( x_lo, x_lo, x_lo ),
                       CV_RGB( x_up, x_up, x_up ), flags);
       }
       else
       {
          cv::Point seed(x_xcomp[i],x_ycomp[i]);
          double brightness = (x_r[i]*2 + x_g[i]*7 + x_b[i] + 5)/10.0;
          cv::floodFill( grey, seed, brightness,  &rect,
                         x_lo, x_up, flags);
       }
       SETFLOAT(&x_list[0], i);
       SETFLOAT(&x_list[1], rect.x);
       SETFLOAT(&x_list[2], rect.y);
       SETFLOAT(&x_list[3], rect.width);
       SETFLOAT(&x_list[4], rect.height);
       outlet_list( m_dataout, 0, 5, x_list );
     }
  }

  if ( !x_color )
  {  
    mat2image(grey, image);
  }
  else
  {
     mat2image(rgb, image);
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////

void pix_opencv_floodfill :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::colorMessCallback,
		  gensym("color"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::fillcolorMessCallback,
		  gensym("fillcolor"), A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::connectivityMessCallback,
		  gensym("connectivity"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::markMessCallback,
		  gensym("mark"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::deleteMessCallback,
		  gensym("delete"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::clearMessCallback,
		  gensym("clear"), A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::updiffMessCallback,
		  gensym("up_diff"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_floodfill::lodiffMessCallback,
		  gensym("lo_diff"), A_FLOAT, A_NULL);
}

void  pix_opencv_floodfill :: colorMessCallback(void *data, t_floatarg color)
{
    GetMyClass(data)->colorMess((float)color);
}

void  pix_opencv_floodfill :: fillcolorMessCallback(void *data, t_floatarg index, t_floatarg r, t_floatarg g, t_floatarg b)
{
    GetMyClass(data)->fillcolorMess(index, r, g, b);
}

void  pix_opencv_floodfill :: connectivityMessCallback(void *data, t_floatarg connectivity)
{
    GetMyClass(data)->connectivityMess(connectivity);
}

void  pix_opencv_floodfill :: markMessCallback(void *data, t_floatarg px, t_floatarg py)
{
    GetMyClass(data)->markMess(px, py);
}

void  pix_opencv_floodfill :: deleteMessCallback(void *data, t_floatarg index)
{
    GetMyClass(data)->deleteMess((float)index);
}

void  pix_opencv_floodfill :: clearMessCallback(void *data)
{
    GetMyClass(data)->clearMess();
}

void  pix_opencv_floodfill :: updiffMessCallback(void *data, t_floatarg updiff)
{
    GetMyClass(data)->updiffMess((float)updiff);
}

void  pix_opencv_floodfill :: lodiffMessCallback(void *data, t_floatarg lodiff)
{
    GetMyClass(data)->lodiffMess((float)lodiff);
}

void  pix_opencv_floodfill :: colorMess(float color)
{
    if ( ( (int)color == 0 ) || ( (int)color == 1 ) )
    {
       x_color = (int)color;
    }
}

void  pix_opencv_floodfill :: fillcolorMess(float index, float r, float g, float b)
{
    if ( ( (int)index <= 0 ) || ( (int)index > MAX_COMPONENTS ) )
    {
       post( "pix_opencv_floodfill : wrong color index : %d", (int)index );
       return;
    }

    if ( ( (int)r >= 0 ) || ( (int)r <= 255 ) )
    {
       x_r[(int)index-1] = (int)r;
    }

    if ( ( (int)g >= 0 ) || ( (int)g <= 255 ) )
    {
       x_g[(int)index-1] = (int)g;
    }

    if ( ( (int)b >= 0 ) || ( (int)b <= 255 ) )
    {
       x_b[(int)index-1] = (int)b;
    }
}

void  pix_opencv_floodfill :: connectivityMess(float connectivity)
{
    if ( ( connectivity != 4.0 ) && ( connectivity != 8.0 ) )
    {
       return;
    }

    x_connectivity = (int)connectivity;
}

void  pix_opencv_floodfill :: markMess(float fpx, float fpy)
{
  int i;
  int inserted;
  int px, py;

    if ( ( fpx < 0.0 ) || ( fpx > comp_xsize ) || ( fpy < 0.0 ) || ( fpy > comp_ysize ) )
    {
      return;
    }

    px = (int)fpx;
    //py = comp_ysize-(int)fpy;
    py = (int)fpy;
    inserted = 0;
    for ( i=0; i<MAX_COMPONENTS; i++)
    {
       if ( x_xcomp[i] == -1 )
       {
         x_xcomp[i] = px;
         x_ycomp[i] = py;
         // post( "pix_opencv_floodfill : inserted point (%d,%d)", px, py );
         inserted = 1;
         break;
       }
    }
    if ( !inserted )
    {
       post( "pix_opencv_floodfill : max markers reached" );
    }
}

void  pix_opencv_floodfill :: deleteMess(float index)
{
  int i;

    if ( ( index < 1.0 ) || ( index > MAX_COMPONENTS ) )
    {
       return;
    }

    x_xcomp[(int)index-1] = -1;
    x_ycomp[(int)index-1] = -1;

}

void  pix_opencv_floodfill :: clearMess(void)
{
  int i;

    for ( i=0; i<MAX_COMPONENTS; i++)
    {
      x_xcomp[i] = -1;
      x_ycomp[i] = -1;
    }

}

void  pix_opencv_floodfill :: updiffMess(float updiff)
{
  if ( ( (int)updiff >= 0 ) && ( (int)updiff <= 255 ) )
  {
     x_up = (int)updiff;
  }
}

void  pix_opencv_floodfill :: lodiffMess(float lodiff)
{
  if ( ( (int)lodiff >= 0 ) && ( (int)lodiff <= 255 ) )
  {
     x_lo = (int)lodiff;
  }
}
