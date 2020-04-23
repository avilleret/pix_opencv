#include "pix_opencv_edge.hpp"
#include "pix_opencv_utils.hpp"
#include <opencv2/imgproc.hpp>

CPPEXTERN_NEW(pix_opencv_edge)

/////////////////////////////////////////////////////////
//
// pix_opencv_edge
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_edge :: pix_opencv_edge()
{ 
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("ft1"));
  edge_thresh = 50;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_edge :: ~pix_opencv_edge()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_edge :: processImage(imageStruct &image)
{
    auto gray = image2mat_gray(image);
    cv::Mat edge;
    cv::blur(gray, edge, cv::Size(3,3));

    // Run the edge detector on grayscale
    cv::Canny(gray, edge, edge_thresh, edge_thresh*3, 3);
  
    auto orig = image2mat(image);
    cv::Mat result = cv::Mat::zeros(orig.rows, orig.cols, orig.type());
    orig.copyTo(result, edge);

    mat2image(result, image);
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_edge :: floatThreshMess (float edge_thresh)
{
  this->edge_thresh = (int)edge_thresh;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_edge :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_edge::floatTreshMessCallback,
  		  gensym("ft1"), A_FLOAT, A_NULL);
}
void pix_opencv_edge :: floatTreshMessCallback(void *data, t_floatarg edge_thresh)
{
  GetMyClass(data)->floatThreshMess((float)edge_thresh);
}
