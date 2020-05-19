#include "pix_opencv_hough_lines.hpp"
#include "pix_opencv_utils.hpp"

#include <stdio.h>

CPPEXTERN_NEW(pix_opencv_hough_lines)

/////////////////////////////////////////////////////////
//
// pix_opencv_hough_lines
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_hough_lines :: pix_opencv_hough_lines()
{ 
  int i;

  comp_xsize=320;
  comp_ysize=240;

  m_dataout = outlet_new(this->x_obj, &s_anything);

  x_mode = cv::HOUGH_PROBABILISTIC;
  x_threshold = 50;
  x_maxlines = 10;
  x_minlength = 30.0;
  x_gap = 10.0;
  x_aresolution = 10.0;
  x_dresolution = 30.0;
  night_mode = 0;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_hough_lines :: ~pix_opencv_hough_lines()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_hough_lines :: processImage(imageStruct &image)
{
  cv::Mat mat = image2mat(image);
  cv::Mat gray = image2mat_gray(image);
  cv::Canny( gray, gray, 50, 200, 3 );

  if( night_mode )
    mat = cv::Mat::zeros(mat.rows, mat.cols, mat.type());

  std::vector<cv::Vec4f> lines;
  if( x_mode )
  {
    cv::HoughLinesP(gray, lines, 1, CV_PI/180, x_threshold, x_minlength, x_gap);
    int i = 0;
    for(const auto& l : lines)
    {
      cv::Point a(l[0], l[1]);
      cv::Point b(l[2], l[3]);
      cv::line(mat, a, b, cv::Scalar(0,0,255), 3, cv::LINE_AA);
      cv::putText( mat, std::to_string(i++), (a+b)/2, cv::FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255,255,255));

      SETFLOAT(&x_list[0], i);
      SETFLOAT(&x_list[1], l[0]);
      SETFLOAT(&x_list[2], l[1]);
      SETFLOAT(&x_list[3], l[2]);
      SETFLOAT(&x_list[4], l[3]);
      outlet_list( m_dataout, 0, 5, x_list );
    }
  }
  else
  {
    cv::HoughLines(gray, lines, 1, CV_PI/180, x_threshold);
    for( size_t i = 0; i < lines.size(); i++ )
    {
        float rho = lines[i][0], theta = lines[i][1];
        cv::Point pt1, pt2;
        double a = cos(theta), b = sin(theta);
        double x0 = a*rho, y0 = b*rho;
        pt1.x = cvRound(x0 + 1000*(-b));
        pt1.y = cvRound(y0 + 1000*(a));
        pt2.x = cvRound(x0 - 1000*(-b));
        pt2.y = cvRound(y0 - 1000*(a));
        cv::line( mat, pt1, pt2, cv::Scalar(0,0,255), 3, cv::LINE_AA);
        pt1.x = (pt1.x+pt2.x)/2;
        pt1.y = (pt1.y+pt2.y)/2;
        cv::putText(mat, std::to_string(i), pt1, cv::FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255,255,255));

        SETFLOAT(&x_list[0], i);
        SETFLOAT(&x_list[1], pt1.x);
        SETFLOAT(&x_list[2], pt1.y);
        SETFLOAT(&x_list[3], pt2.x);
        SETFLOAT(&x_list[4], pt2.y);
        outlet_list( m_dataout, 0, 5, x_list );
    }
  }
  mat2image(mat, image);
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////

void pix_opencv_hough_lines :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatNightModeMessCallback,
                  gensym("nightmode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatModeMessCallback,
                  gensym("mode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatThresholdMessCallback,
                  gensym("threshold"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatMinLengthMessCallback,
                  gensym("minlength"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatGapMessCallback,
                  gensym("gap"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatAResolutionMessCallback,
                  gensym("aresolution"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatDResolutionMessCallback,
                  gensym("dresolution"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hough_lines::floatMaxLinesMessCallback,
                  gensym("maxlines"), A_FLOAT, A_NULL);
}

void pix_opencv_hough_lines :: floatNightModeMessCallback(void *data, t_floatarg nightmode)
{
  GetMyClass(data)->floatNightModeMess((float)nightmode);
}

void pix_opencv_hough_lines :: floatModeMessCallback(void *data, t_floatarg mode)
{
  GetMyClass(data)->floatModeMess((float)mode);
}

void pix_opencv_hough_lines :: floatThresholdMessCallback(void *data, t_floatarg threshold)
{
  GetMyClass(data)->floatThresholdMess((float)threshold);
}

void pix_opencv_hough_lines :: floatMinLengthMessCallback(void *data, t_floatarg minlength)
{
  GetMyClass(data)->floatMinLengthMess((float)minlength);
}

void pix_opencv_hough_lines :: floatGapMessCallback(void *data, t_floatarg gap)
{
  GetMyClass(data)->floatGapMess((float)gap);
}

void pix_opencv_hough_lines :: floatAResolutionMessCallback(void *data, t_floatarg aresolution)
{
  GetMyClass(data)->floatAResolutionMess((float)aresolution);
}

void pix_opencv_hough_lines :: floatDResolutionMessCallback(void *data, t_floatarg dresolution)
{
  GetMyClass(data)->floatDResolutionMess((float)dresolution);
}

void pix_opencv_hough_lines :: floatMaxLinesMessCallback(void *data, t_floatarg maxlines)
{
  GetMyClass(data)->floatMaxLinesMess((float)maxlines);
}

void pix_opencv_hough_lines :: floatNightModeMess(float nightmode)
{
  if ((nightmode==0.0)||(nightmode==1.0)) night_mode = (int)nightmode;
}

void pix_opencv_hough_lines :: floatModeMess(float mode)
{
  x_mode = mode > 0;
}

void pix_opencv_hough_lines :: floatThresholdMess(float threshold)
{
  if (threshold>0.0) x_threshold = (int)threshold;
}

void pix_opencv_hough_lines :: floatMinLengthMess(float minlength)
{
  if (minlength>0.0) x_minlength = (double)minlength;
}

void pix_opencv_hough_lines :: floatGapMess(float gap)
{
  if (gap>0.0) x_gap = (double)gap;
}

void pix_opencv_hough_lines :: floatAResolutionMess(float aresolution)
{
  if (aresolution>0.0) x_aresolution = (double)aresolution;
}

void pix_opencv_hough_lines :: floatDResolutionMess(float dresolution)
{
  if (dresolution>0.0) x_dresolution = (double)dresolution;
}

void pix_opencv_hough_lines :: floatMaxLinesMess(float maxlines)
{
  if (maxlines>0.0) x_maxlines = (int)maxlines;
}

