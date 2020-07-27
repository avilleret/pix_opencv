////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-1998 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "pix_opencv_pgh_compare.hpp"
#include "pix_opencv_utils.hpp"

#include <opencv2/imgproc.hpp>

CPPEXTERN_NEW_WITH_GIMME(pix_opencv_pgh_compare)

/////////////////////////////////////////////////////////
//
// pix_opencv_pgh_compare
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_pgh_compare :: pix_opencv_pgh_compare(int argc, t_atom*argv)
{ 
    m_dataout = outlet_new(this->x_obj, &s_anything);
    m_posout = outlet_new(this->x_obj, &s_anything);

    comp_xsize=320;
    comp_ysize=240;

    x_minsize = 10*10;
    x_cdistance = 0.05;
}

/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_opencv_pgh_compare :: processDualImage(imageStruct &left, imageStruct &right)
{
  double dist = 100.0, ndist;
  int i = 0;                   // Indicator of cycles.
  int dims[] = {8, 8};
  float range[] = {-180, 180, -100, 100};
  float *ranges[] = {&range[0], &range[2]};

  if ((left.xsize!=right.xsize) || (left.ysize!=right.ysize) )
  {
    post( "pix_opencv_pgh_compare : left and right image are not of the same size" );
    return;
  }

  auto leftmat = image2mat(left);
  auto grayr = image2mat_gray(right);
  std::vector<std::vector<cv::Point>> contours;

  // calculate the biggest contour
  try {
    cv::findContours(grayr, contours, {}, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
  }
  catch(...) {
      post( "pix_opencv_pgh_compare : error calculating contours" );
      return;
  }

  int size = 0;
  for(const auto& c : contours)
  {
    auto rect = cv::boundingRect(c);
    if(rect.area() > size && rect.area() < (left.xsize-2)*(left.ysize-2))
    {
      x_bcontourr = c;
      size = rect.width*rect.height;
    }
  }

  auto lgray = image2mat_gray(left);

  std::vector<std::vector<cv::Point>> lcontours;

  cv::findContours(lgray, lcontours, {}, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  i=0;
  for(const auto c : lcontours)
  {
    auto rect = cv::boundingRect(c);
    if ( rect.area() > x_minsize && rect.area() < (comp_xsize-2)*(comp_ysize-2))
    {
      double ndist = cv::matchShapes(x_bcontourr, c, cv::CONTOURS_MATCH_I1, 0.);
      if ( ndist < dist ) dist = ndist;
      if ( ndist < x_cdistance )
      {
        cv::rectangle( leftmat, cv::Point(rect.x,rect.y), cv::Point(rect.x+rect.width,rect.y+rect.height), CV_RGB(255,255,255), 2, 8 , 0 );
        cv::drawContours( leftmat, lcontours, i, cv::Scalar(255,255,255));
        SETFLOAT(&rlist[0], i++);
        SETFLOAT(&rlist[1], rect.x);
        SETFLOAT(&rlist[2], rect.y);
        SETFLOAT(&rlist[3], rect.width);
        SETFLOAT(&rlist[4], rect.height);
        outlet_list( m_posout, 0, 5, rlist );
      }
      else
      {
        cv::rectangle(leftmat, cv::Point(rect.x,rect.y), cv::Point(rect.x+rect.width,rect.y+rect.height), CV_RGB(128,128,128), 2, 8 , 0 );
        cv::drawContours(leftmat, lcontours, -1, cv::Scalar(128,128,128));
      }
    }
  }

  if ( dist < 100.00 ) outlet_float( m_dataout, dist );
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_pgh_compare :: obj_setupCallback(t_class *classPtr)
{ 
  class_addmethod(classPtr, (t_method)&pix_opencv_pgh_compare::floatMinSizeMessCallback,
		  gensym("minsize"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_pgh_compare::clearMessCallback,
		  gensym("clear"), A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_pgh_compare::floatCriteriaMessCallback,
		  gensym("criteria"), A_FLOAT, A_NULL);
}

void pix_opencv_pgh_compare :: floatMinSizeMessCallback(void *data, t_floatarg minsize)
{
  GetMyClass(data)->floatMinSizeMess((float)minsize);
}

void pix_opencv_pgh_compare :: clearMessCallback(void *data)
{
  GetMyClass(data)->clearMess();
}

void pix_opencv_pgh_compare :: floatCriteriaMessCallback(void *data, t_floatarg criteria)
{
  GetMyClass(data)->floatCriteriaMess((float)criteria);
}

void  pix_opencv_pgh_compare :: floatMinSizeMess(float minsize)
{
   if ( (int)minsize > 0 )
   {
     x_minsize = (int)minsize;
   }
}

void  pix_opencv_pgh_compare :: clearMess(void)
{
  x_bcontourr = {};
}

void  pix_opencv_pgh_compare :: floatCriteriaMess(float criteria)
{
   if ( criteria > 0.0 )
   {
      x_cdistance = criteria;
   }
}
