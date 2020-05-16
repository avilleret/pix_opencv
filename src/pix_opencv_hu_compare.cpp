#include "pix_opencv_hu_compare.hpp"
#include "pix_opencv_utils.hpp"

CPPEXTERN_NEW_WITH_GIMME(pix_opencv_hu_compare)

/////////////////////////////////////////////////////////
//
// pix_opencv_hu_compare
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_hu_compare :: pix_opencv_hu_compare(int argc, t_atom*argv)
{ 
    m_dataout = outlet_new(this->x_obj, &s_anything);
    m_posout = outlet_new(this->x_obj, &s_anything);

    comp_xsize=320;
    comp_ysize=240;

    x_method = cv::CONTOURS_MATCH_I1;
    x_minsize = 10*10;
    x_cdistance = 0.1;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_hu_compare :: ~pix_opencv_hu_compare()
{ 
}

/////////////////////////////////////////////////////////
// processDualImage
//
/////////////////////////////////////////////////////////
void pix_opencv_hu_compare :: processDualImage(imageStruct &left, imageStruct &right)
{
 double dist = 100.0, ndist;
 int i = 0;                   // Indicator of cycles.
 cv::Rect rect;
 int    size;

  if ((left.xsize!=right.xsize) || (left.ysize!=right.ysize) )
  {
    post( "pix_opencv_hu_compare : left and right image are not of the same size" );
    return;
  }

  cv::Mat left_mat = image2mat_gray(left);
  cv::Mat right_mat = image2mat_gray(right);

  std::vector<std::vector<cv::Point>> right_contours, left_contours;
  std::vector<cv::Vec4i> right_hierarchy, left_hierarchy;
  // calculate the biggest contour
  try {
    cv::findContours(right_mat, right_contours, right_hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
    cv::findContours(left_mat,  left_contours,  left_hierarchy,  cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
      // cvFindContours( grayr, x_storage, &right_contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0) );
  }
  catch(...) {
      post( "pix_opencv_hu_compare : error calculating contours" );
      return;
  }

  int max_size = (comp_xsize-2)*(comp_ysize-2);
  for(const auto& contour : right_contours)
  {
    rect = cv::boundingRect(contour);
    int area = rect.area();
    if(area > size && area < max_size)
    {
      x_bcontourr = contour;
      size = rect.width*rect.height;
    }
  }

  cv::Mat left_copy = left_mat.clone();

  for(const auto& contour : left_contours)
  {
    rect = cv::boundingRect(contour);
    int area = rect.area();
    if(area > size && area < max_size)
    {
      ndist = cv::matchShapes(x_bcontourr, contour, x_method, 0);
      ndist = std::max(ndist, dist);
      cv::Scalar color(128,128,128);
      if( ndist < x_cdistance )
      {
        color = cv::Scalar(255,255,255);
        SETFLOAT(&rlist[0], i++);
        SETFLOAT(&rlist[1], rect.x);
        SETFLOAT(&rlist[2], rect.y);
        SETFLOAT(&rlist[3], rect.width);
        SETFLOAT(&rlist[4], rect.height);
        outlet_list( m_posout, 0, 5, rlist );
      }
      cv::rectangle(left_copy, rect, color, 2);
      cv::drawContours(left_copy, contour, 0, color, 1);
    }
  }

  if ( dist < 100.00 ) outlet_float( m_dataout, dist );

  mat2image(left_copy, left);
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_hu_compare :: obj_setupCallback(t_class *classPtr)
{ 
  class_addmethod(classPtr, (t_method)&pix_opencv_hu_compare::floatMethodMessCallback,
		  gensym("method"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hu_compare::floatMinSizeMessCallback,
		  gensym("minsize"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hu_compare::clearMessCallback,
		  gensym("clear"), A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_hu_compare::floatCriteriaMessCallback,
		  gensym("criteria"), A_FLOAT, A_NULL);
}

void pix_opencv_hu_compare :: floatMethodMessCallback(void *data, t_floatarg method)
{
  GetMyClass(data)->floatMethodMess((float)method);
}

void pix_opencv_hu_compare :: floatMinSizeMessCallback(void *data, t_floatarg minsize)
{
  GetMyClass(data)->floatMinSizeMess((float)minsize);
}

void pix_opencv_hu_compare :: clearMessCallback(void *data)
{
  GetMyClass(data)->clearMess();
}

void pix_opencv_hu_compare :: floatCriteriaMessCallback(void *data, t_floatarg criteria)
{
  GetMyClass(data)->floatCriteriaMess((float)criteria);
}

void  pix_opencv_hu_compare :: floatMethodMess(float method)
{
    if ((int)method==cv::CONTOURS_MATCH_I1)
    {
       post( "pix_opencv_hu_compare : method set to CV_CONTOURS_MATCH_I1" );
       x_method = (int)method;
    }
    if ((int)method==cv::CONTOURS_MATCH_I2)
    {
       post( "pix_opencv_hu_compare : method set to CV_CONTOURS_MATCH_I2" );
       x_method = (int)method;
    }
    if ((int)method==cv::CONTOURS_MATCH_I3)
    {
       post( "pix_opencv_hu_compare : method set to CV_CONTOURS_MATCH_I3" );
       x_method = (int)method;
    }
}

void  pix_opencv_hu_compare :: floatMinSizeMess(float minsize)
{
   if ( (int)minsize > 0 )
   {
     x_minsize = (int)minsize;
   }
}

void  pix_opencv_hu_compare :: clearMess(void)
{
  x_bcontourr = {};
}

void  pix_opencv_hu_compare :: floatCriteriaMess(float criteria)
{
   if ( criteria > 0.0 )
   {
      x_cdistance = criteria;
   }
}
