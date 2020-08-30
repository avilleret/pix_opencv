#include "pix_opencv_distrans.hpp"
#include "pix_opencv_utils.hpp"
#include <opencv2/core/fast_math.hpp>

CPPEXTERN_NEW(pix_opencv_distrans)

/////////////////////////////////////////////////////////
//
// pix_opencv_distrans
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_distrans :: pix_opencv_distrans()
{ 
  int i;

  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("ft1"));

  edge_thresh = 25;
  mask_size = cv::DIST_MASK_PRECISE;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_distrans :: ~pix_opencv_distrans()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_distrans :: processImage(imageStruct &image)
{
  cv::Mat gray = image2mat_gray(image);

  static const cv::Scalar colors[] =
  {
      cv::Scalar(0,0,0),
      cv::Scalar(255,0,0),
      cv::Scalar(255,128,0),
      cv::Scalar(255,255,0),
      cv::Scalar(0,255,0),
      cv::Scalar(0,128,255),
      cv::Scalar(0,255,255),
      cv::Scalar(0,0,255),
      cv::Scalar(255,0,255)
  };

  int maskSize = voronoiType >= 0 ? cv::DIST_MASK_5 : mask_size;
  int distType = voronoiType >= 0 ? cv::DIST_L2 : dist_type;
  cv::Mat edge = gray >= edge_thresh, dist, labels, dist8u;
  if( voronoiType < 0 )
      distanceTransform( edge, dist, distType, maskSize );
  else
      distanceTransform( edge, dist, labels, distType, maskSize, voronoiType );
  if( voronoiType < 0 )
  {
      // begin "painting" the distance transform result
      dist *= 5000;
      pow(dist, 0.5, dist);
      cv::Mat dist32s, dist8u1, dist8u2;
      dist.convertTo(dist32s, CV_32S, 1, 0.5);
      dist32s &= cv::Scalar::all(255);
      dist32s.convertTo(dist8u1, CV_8U, 1, 0);
      dist32s *= -1;
      dist32s += cv::Scalar::all(255);
      dist32s.convertTo(dist8u2, CV_8U);
      cv::Mat planes[] = {dist8u1, dist8u2, dist8u2};
      cv::merge(planes, 3, dist8u);
  }
  else
  {
      dist8u.create(labels.size(), CV_8UC3);
      for( int i = 0; i < labels.rows; i++ )
      {
          const int* ll = (const int*)labels.ptr(i);
          const float* dd = (const float*)dist.ptr(i);
          uchar* d = (uchar*)dist8u.ptr(i);
          for( int j = 0; j < labels.cols; j++ )
          {
              int idx = ll[j] == 0 || dd[j] == 0 ? 0 : (ll[j]-1)%8 + 1;
              float scale = 1.f/(1 + dd[j]*dd[j]*0.0004f);
              int b = cvRound(colors[idx][0]*scale);
              int g = cvRound(colors[idx][1]*scale);
              int r = cvRound(colors[idx][2]*scale);
              d[j*3] = (uchar)b;
              d[j*3+1] = (uchar)g;
              d[j*3+2] = (uchar)r;
          }
      }
  }
  mat2image(dist8u, image);
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_distrans :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_distrans::thresholdMessCallback,
  		  gensym("ft1"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_distrans::voronoiMessCallback,
		  gensym("voronoi"), A_DEFFLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_distrans::maskMessCallback,
		  gensym("mask"), A_DEFFLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_distrans::typeMessCallback,
      gensym("type"), A_DEFFLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_distrans::distMessCallback,
      gensym("dist"), A_DEFFLOAT, A_NULL);
}
void pix_opencv_distrans :: thresholdMessCallback(void *data, t_floatarg pos)
{
  if (pos>=0) GetMyClass(data)->edge_thresh = (int)pos;
}
void pix_opencv_distrans :: voronoiMessCallback(void *data, t_floatarg voronoi)
{
  GetMyClass(data)->build_voronoi=!(!(int)voronoi);
}
void pix_opencv_distrans :: maskMessCallback(void *data, t_floatarg f)
{
  int i = static_cast<int>(f);
  int& mask_size = GetMyClass(data)->mask_size;
  switch(i)
  {
    case 0:
      mask_size = cv::DIST_MASK_PRECISE;
      break;
    case 3:
      mask_size = cv::DIST_MASK_3;
      break;
    case 5:
      mask_size = cv::DIST_MASK_5;
      break;
    default:
      ;
  }
}

void pix_opencv_distrans :: distMessCallback(void *data, t_floatarg f)
{
  int type = static_cast<int>(f);
  switch(type)
  {
    case 0:
      GetMyClass(data)->dist_type = cv::DIST_C;
      break;
    case 1:
      GetMyClass(data)->dist_type = cv::DIST_L1;
      break;
    case 2:
      GetMyClass(data)->dist_type = cv::DIST_L2;
      break;
  }
}

void pix_opencv_distrans :: typeMessCallback(void *data, t_floatarg f)
{
  if(f>0)
    GetMyClass(data)->voronoiType = cv::DIST_LABEL_PIXEL;
  else
    GetMyClass(data)->voronoiType = cv::DIST_LABEL_CCOMP;
}
