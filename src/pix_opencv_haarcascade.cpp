#include "pix_opencv_haarcascade.hpp"
#include "pix_opencv_utils.hpp"
#include <stdio.h>
#include <stdlib.h>

CPPEXTERN_NEW(pix_opencv_haarcascade)

/////////////////////////////////////////////////////////
//
// pix_opencv_haarcascade
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_haarcascade :: pix_opencv_haarcascade()
{ 
  int i;

  m_numout = outlet_new(this->x_obj, 0);
  m_dataout = outlet_new(this->x_obj, 0);

  scale_factor = 1.1;
  min_neighbors = 2;
  mode = 0;
  min_size = 30;
  
  comp_xsize  = 0;
  comp_ysize  = 0;
  x_ftolerance = 5;

  for ( i=0; i<MAX_MARKERS; i++ )
  {
    x_xmark[i] = -1;
    x_ymark[i] = -1;
  }

  if( !cascade.load(cascade_name))
  {
    error("ERROR: Could not load classifier cascade from : %s\n", cascade_name );
  }
  else
  {
    post( "Loaded classifier cascade from %s", cascade_name );
  }
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_haarcascade :: ~pix_opencv_haarcascade()
{ 
}

void pix_opencv_haarcascade :: processImage(imageStruct &image)
{
  double scale = 1;

  this->comp_xsize = image.xsize;
  this->comp_ysize = image.ysize;

  cv::Mat frame = image2mat(image);

  const static cv::Scalar colors[] =
  {
    cv::Scalar(0,0,255),
    cv::Scalar(0,128,255),
    cv::Scalar(0,255,255),
    cv::Scalar(0,255,0),
    cv::Scalar(255,128,0),
    cv::Scalar(255,255,0),
    cv::Scalar(255,0,0),
    cv::Scalar(255,0,255)
  };

  int i, im;

  std::vector<cv::Rect> objects;
  cascade.detectMultiScale(frame, objects, 1.1, 2, 0, cv::Size(30, 30));

  outlet_float(this->m_numout, (float)objects.size());

  for ( im=0; im<MAX_MARKERS; im++ )
  {
    if ( this->x_xmark[im] != -1.0 )
    {
      this->x_found[im]--;
    }
  }

  for(const auto& rect : objects )
  {
    int oi, found;

    cv::Point center;
    int radius;
    center.x = cvRound((rect.x + rect.width*0.5)*scale);
    center.y = cvRound((rect.y + rect.height*0.5)*scale);
    radius = cvRound((rect.width + rect.height)*0.25*scale);

    found = 0;
    oi = -1;
    for ( im=0; im<MAX_MARKERS; im++ )
    {
      // check if the object is already known
      if ( sqrt( pow((double)center.x - this->x_xmark[im], 2 ) + pow((double)center.y - this->x_ymark[im], 2 ) ) <= radius )
      {
        oi=im;
        found=1;
        this->x_found[im] = this->x_ftolerance;
        this->x_xmark[im] = center.x;
        this->x_ymark[im] = center.y;
        break;
      }
    }
    // new object detected
    if ( !found )
    {
      oi = this->mark(center.x, center.y );
    }

    char tindex[4];
    sprintf( tindex, "%d", oi );
    cv::circle( frame, center, radius, colors[oi%8], 3, 8, 0 );
    cv::putText( frame, tindex, center, cv::FONT_HERSHEY_SIMPLEX, 1., CV_RGB(255,255,255));;

    SETFLOAT(&this->rlist[0], oi);
    SETFLOAT(&this->rlist[1], center.x);
    SETFLOAT(&this->rlist[2], center.y);
    SETFLOAT(&this->rlist[3], radius);
    outlet_list( m_dataout, 0, 4, this->rlist );
  }
  // delete lost objects
  for ( im=0; im<MAX_MARKERS; im++ )
  {
    if ( this->x_found[im] < 0 )
    {
      this->x_xmark[im] = -1.0;
      this->x_ymark[im] = -1,0;
      this->x_found[im] = this->x_ftolerance;
    }
  }
  mat2image(frame, image);
}

/////////////////////////////////////////////////////////
// scaleFactorMess
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: scaleFactorMess (float scale_factor)
{
  this->scale_factor = scale_factor;
}

/////////////////////////////////////////////////////////
// minNeighborsMess
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: minNeighborsMess (float min_neighbors)
{
  this->min_neighbors = (int)min_neighbors;
}

/////////////////////////////////////////////////////////
// modeMess
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: modeMess (float mode)
{
  this->mode = !(!(int)mode);
}

/////////////////////////////////////////////////////////
// minSizeMess
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: minSizeMess (float min_size)
{
  this->min_size = (int)min_size;
}

/////////////////////////////////////////////////////////
// fToleranceMess
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: fToleranceMess (float ftolerance)
{
  this->x_ftolerance = (int)ftolerance;
}

/////////////////////////////////////////////////////////
// clearMess
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: clearMess (void)
{
  int i;

  for ( i=0; i<MAX_MARKERS; i++)
  {
    this->x_xmark[i] = -1;
    this->x_ymark[i] = -1;
    this->x_found[i] = this->x_ftolerance;
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_haarcascade :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::scaleFactorMessCallback,
                  gensym("scale_factor"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::minNeighborsMessCallback,
                  gensym("min_neighbors"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::modeMessCallback,
                  gensym("mode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::minSizeMessCallback,
                  gensym("min_size"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::loadCascadeMessCallback,
                  gensym("load"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::fToleranceMessCallback,
                  gensym("ftolerance"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_haarcascade::clearMessCallback,
                  gensym("clear"), A_NULL);
}
void pix_opencv_haarcascade :: scaleFactorMessCallback(void *data, t_floatarg scale_factor)
{
  if (scale_factor>1) GetMyClass(data)->scaleFactorMess((float)scale_factor);
}
void pix_opencv_haarcascade :: minNeighborsMessCallback(void *data, t_floatarg min_neighbors)
{
  if (min_neighbors>=1) GetMyClass(data)->minNeighborsMess((float)min_neighbors);
}
void pix_opencv_haarcascade :: modeMessCallback(void *data, t_floatarg mode)
{
  if ((mode==0)||(mode==1)) GetMyClass(data)->modeMess((float)mode);
}
void pix_opencv_haarcascade :: minSizeMessCallback(void *data, t_floatarg min_size)
{
  if (min_size>1) GetMyClass(data)->minSizeMess((float)min_size);
}
void pix_opencv_haarcascade :: fToleranceMessCallback(void *data, t_floatarg ftolerance)
{
  if (ftolerance>1) GetMyClass(data)->fToleranceMess((float)ftolerance);
}
void pix_opencv_haarcascade :: clearMessCallback(void *data)
{
  GetMyClass(data)->clearMess();
}
void pix_opencv_haarcascade :: loadCascadeMessCallback(void *data, t_symbol* filename)
{
  GetMyClass(data)->loadCascadeMess(filename);
}
void pix_opencv_haarcascade :: loadCascadeMess(t_symbol *filename)
{
  if( !cascade.load( filename->s_name))
  {
    post( "ERROR: Could not load classifier cascade" );
  }
  else    post( "Loaded classifier cascade from %s", filename->s_name );
}
int pix_opencv_haarcascade :: mark(float fx, float fy )
{
  int i;

  // TODO review the whole marker code
  // use C++ container and algorithm to find matches
//  if ( ( fx < 0.0 ) || ( fx > this->rgba->width ) || ( fy < 0 ) || ( fy > this->rgba->height ) )
//  {
//    return -1;
//  }

  for ( i=0; i<MAX_MARKERS; i++)
  {
    if ( this->x_xmark[i] == -1 )
    {
      this->x_xmark[i] = (int)fx;
      this->x_ymark[i] = (int)fy;
      this->x_found[i] = this->x_ftolerance;
      return i;
    }
  }

  post( "pix_opencv_haarcascade : max markers reached" );
  return -1;
}
