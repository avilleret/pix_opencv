#include "pix_opencv_laplace.hpp"
#include "pix_opencv_utils.hpp"

CPPEXTERN_NEW(pix_opencv_laplace)

/////////////////////////////////////////////////////////
//
// pix_opencv_laplace
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_laplace :: pix_opencv_laplace()
{ 
  int i;

  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("ft1"));

  aperture_size = 3;
  comp_xsize  = 0;
  comp_ysize  = 0;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_laplace :: ~pix_opencv_laplace()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_laplace :: processImage(imageStruct &image)
{
  cv::Mat mat = image2mat(image);

  if(m_laplacian.size() != mat.size() || m_laplacian.channels() != mat.channels())
  {
    m_laplacian = cv::Mat(mat.size(), CV_MAKETYPE(CV_16S,mat.channels()));
  }

  if(m_out.size() != mat.size()
     || m_out.type() != mat.type()
     || m_out.channels() != mat.channels())
  {
    m_out = cv::Mat(mat.size(), mat.type());
  }

  cv::Laplacian(mat, m_laplacian, CV_16S, aperture_size);
  cv::convertScaleAbs(m_laplacian, m_out, 1, 0);

  mat2image(m_out, image);
}

/////////////////////////////////////////////////////////
// floatApertureMess
//
/////////////////////////////////////////////////////////
void pix_opencv_laplace :: floatApertureMess (float aperture_size)
{
  if ((aperture_size==1)||(aperture_size==3)||(aperture_size==5)||(aperture_size==7)) this->aperture_size = (int)aperture_size;
  else post("aperture size out of range ...  must be 1,3,5 or 7");
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_laplace :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_laplace::floatApertureMessCallback,
  		  gensym("ft1"), A_FLOAT, A_NULL);
}
void pix_opencv_laplace :: floatApertureMessCallback(void *data, t_floatarg aperture_size)
{
  GetMyClass(data)->floatApertureMess((float)aperture_size);
}
