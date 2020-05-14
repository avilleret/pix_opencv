////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////

#include "pix_opencv_findchessboardcorners.hpp"
#include "pix_opencv_utils.hpp"
#include <stdio.h>

#include <opencv2/calib3d.hpp>

CPPEXTERN_NEW(pix_opencv_findchessboardcorners)

/////////////////////////////////////////////////////////
//
// pix_opencv_findchessboardcorners
// by Antoine Villeret
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

using namespace cv;

pix_opencv_findchessboardcorners :: pix_opencv_findchessboardcorners()
{ 
  //inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("minarea"));
  //inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("maxarea"));
  m_dataout = outlet_new(this->x_obj, 0);
  //m_countout = outlet_new(this->x_obj, 0);
  comp_xsize  = 320;
  comp_ysize  = 240;
  
  pattern_size = cv::Size(6,7);
  win = cv::Size(11, 11);
  zero_zone = cv::Size(-1,-1);
  coord_list = new t_atom[pattern_size.width * pattern_size.height * 2]; // all coordinates are packed in one list [x0 y0 x1 y1 .. xn yn(
  
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_findchessboardcorners :: ~pix_opencv_findchessboardcorners()
{ 
  delete coord_list;
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_findchessboardcorners:: processImage(imageStruct &image)
{
  gray = image2mat_gray(image);
  patternfound = findChessboardCorners(gray, pattern_size, corners,
                                       CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
                                       + CALIB_CB_FAST_CHECK);

  // send out corners screen coordinates if all corners have been found
  if (patternfound) {
    cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
                 TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.1));
    int i=0;
    for (const auto& pt : corners)
    {
      SETFLOAT(&coord_list[i*2], pt.x);
      SETFLOAT(&coord_list[i*2+1], pt.y);
      i++;
    }
    outlet_list( m_dataout, 0, corners.size()*2, coord_list );
  }
  cv::drawChessboardCorners(gray, pattern_size, Mat(corners), patternfound);
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_findchessboardcorners :: patternSizeMess (int xsize, int ysize)
{
  if ( xsize < 3 || ysize < 3 ) {
    error("patternSize should be at least 3x3");
    return;
  }
  pattern_size=cv::Size(xsize,ysize);
  // update corners array & output list size
  delete coord_list;
  coord_list = new t_atom[pattern_size.width * pattern_size.height * 2];
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_findchessboardcorners :: obj_setupCallback(t_class *classPtr)
{
  // TODO add support for message : win, zero_zone, flags and criteria
  class_addmethod(classPtr, (t_method)&pix_opencv_findchessboardcorners::patternSizeMessCallback,
                  gensym("patternSize"), A_FLOAT, A_FLOAT, A_NULL);
}

void pix_opencv_findchessboardcorners :: patternSizeMessCallback(void *data, t_floatarg xsize, t_floatarg ysize)
{
  GetMyClass(data)->patternSizeMess((int)xsize, (int)ysize);
}
