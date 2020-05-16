#include "pix_opencv_hough_circles.hpp"
#include "pix_opencv_utils.hpp"
#include <stdio.h>

CPPEXTERN_NEW(pix_opencv_hough_circles)

/////////////////////////////////////////////////////////
//
// pix_opencv_hough_circles
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_hough_circles :: pix_opencv_hough_circles()
{ 
  int i;

  comp_xsize=320;
  comp_ysize=240;

  m_dataout = outlet_new(this->x_obj, &s_anything);

  x_threshold = 100;
  x_threshold2 = 10;
  x_maxcircles = 10;
  x_mindist = 30.0;
  x_resolution = 1.0;
  night_mode = 0;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_hough_circles :: ~pix_opencv_hough_circles()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_hough_circles :: processImage(imageStruct &image)
{
  cv::Mat mat = image2mat(image);
  cv::Mat gray = image2mat_gray(image);

  cv::GaussianBlur(gray, gray, cv::Size(9,9), 0, 0);
  std::vector<cv::Vec3f> circles;
  cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT,
               x_resolution, x_mindist, x_threshold, x_threshold2 );

  for( size_t i = 0; i < circles.size(); i++ )
  {
       cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
       int radius = cvRound(circles[i][2]);
       // draw the circle center
       cv::circle( mat, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
       // draw the circle outline
       cv::circle( mat, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
       cv::putText(mat, std::to_string(i), center + cv::Point(5,5), cv::FONT_HERSHEY_COMPLEX, 1, CV_RGB(255,255,255));

       SETFLOAT(&x_list[0], i);
       SETFLOAT(&x_list[1], center.x);
       SETFLOAT(&x_list[2], center.y);
       SETFLOAT(&x_list[3], radius);
       outlet_list( m_dataout, 0, 4, x_list );
  }
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////

void pix_opencv_hough_circles :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_circles::floatNightModeMessCallback,
                  gensym("nightmode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_circles::floatThresholdMessCallback,
                  gensym("threshold"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_circles::floatThreshold2MessCallback,
                  gensym("threshold2"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_circles::floatMinDistMessCallback,
                  gensym("mindist"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_circles::floatResolutionMessCallback,
                  gensym("resolution"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_circles::floatMaxCirclesMessCallback,
                  gensym("maxcircles"), A_FLOAT, A_NULL);
}

void pix_opencv_hough_circles :: floatNightModeMessCallback(void *data, t_floatarg nightmode)
{
  GetMyClass(data)->floatNightModeMess((float)nightmode);
}

void pix_opencv_hough_circles :: floatThresholdMessCallback(void *data, t_floatarg threshold)
{
  GetMyClass(data)->floatThresholdMess((float)threshold);
}

void pix_opencv_hough_circles :: floatThreshold2MessCallback(void *data, t_floatarg threshold)
{
  GetMyClass(data)->floatThreshold2Mess((float)threshold);
}

void pix_opencv_hough_circles :: floatMinDistMessCallback(void *data, t_floatarg mindist)
{
  GetMyClass(data)->floatMinDistMess((float)mindist);
}

void pix_opencv_hough_circles :: floatResolutionMessCallback(void *data, t_floatarg resolution)
{
  GetMyClass(data)->floatResolutionMess((float)resolution);
}

void pix_opencv_hough_circles :: floatMaxCirclesMessCallback(void *data, t_floatarg maxcircles)
{
  GetMyClass(data)->floatMaxCirclesMess((float)maxcircles);
}

void pix_opencv_hough_circles :: floatNightModeMess(float nightmode)
{
  if ((nightmode==0.0)||(nightmode==1.0)) night_mode = (int)nightmode;
}

void pix_opencv_hough_circles :: floatThresholdMess(float threshold)
{
  if (threshold>0.0) x_threshold = (int)threshold;
}

void pix_opencv_hough_circles :: floatThreshold2Mess(float threshold)
{
  if (threshold>0.0) x_threshold2 = (int)threshold;
}

void pix_opencv_hough_circles :: floatMinDistMess(float mindist)
{
  if (mindist>0.0) x_mindist = (int)mindist;
}

void pix_opencv_hough_circles :: floatResolutionMess(float resolution)
{
  if (resolution>0.0) x_resolution = (int)resolution;
}

void pix_opencv_hough_circles :: floatMaxCirclesMess(float maxcircles)
{
  if (maxcircles>0.0) x_maxcircles = (int)maxcircles;
}

