#include "pix_opencv_camshift.hpp"
#include "pix_opencv_utils.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

CPPEXTERN_NEW(pix_opencv_camshift)

/////////////////////////////////////////////////////////
//
// pix_opencv_camshift
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_camshift :: pix_opencv_camshift()
{ 
  m_dataout = outlet_new(this->x_obj, &s_anything);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_camshift :: ~pix_opencv_camshift()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_camshift :: processImage(imageStruct &image)
{
  cv::Mat hsv = image2mat_hsv(image);

  if(hsv.size() != m_last_size)
  {
    m_last_size = hsv.size();
    x_track = 0;
    x_init = 0;
  }

  if ( x_track  )
  {
    cv::inRange(hsv, cv::Scalar(0,x_smin,std::min(x_vmin,x_vmax),0),
                cv::Scalar(180,256,std::max(x_vmin,x_vmax),0), m_mask);
    cv::Mat backproj;

    // extract hue (1st channel) from hsv (3 channels)
    std::vector<cv::Mat> mat_vec;
    cv::split(hsv, mat_vec);
    auto& hue = mat_vec[0];

    int hsize = 16;
    float hranges[] = {0,180};
    const float* phranges = hranges;

    if ( x_init )
    {
      unsigned char max_val = 255;
      x_init = 0;
      cv::Mat roi(hue, m_selection), maskroi(m_mask, m_selection);

      cv::calcHist(&roi, 1, 0, maskroi, m_hist, 1, &hsize, &phranges);
      cv::normalize(m_hist, m_hist, 0, max_val, cv::NORM_MINMAX);

      //cv::minMaxLoc(hist, nullptr, &max_val, nullptr, nullptr);

      m_trackwindow = m_selection;
    }

    cv::calcBackProject(&hue, 1, 0, m_hist, backproj, &phranges);

    backproj &= m_mask;
    cv::RotatedRect trackBox = cv::CamShift(backproj, m_trackwindow,
                        cv::TermCriteria( cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1 ));
    if( m_trackwindow.area() <= 1 )
    {
        int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5)/6;
        m_trackwindow = cv::Rect(m_trackwindow.x - r, m_trackwindow.y - r,
                           m_trackwindow.x + r, m_trackwindow.y + r) &
                      cv::Rect(0, 0, cols, rows);
    }
    if( x_backproject )
      mat2image(backproj, image);

    cv::Mat mat = image2mat(image);

    cv::ellipse(mat, trackBox, cv::Scalar(0,0,255), 3, cv::LINE_AA);

    SETFLOAT(&x_list[0], trackBox.center.x);
    SETFLOAT(&x_list[1], trackBox.center.y);
    SETFLOAT(&x_list[2], trackBox.size.width);
    SETFLOAT(&x_list[3], trackBox.size.height);
    SETFLOAT(&x_list[4], trackBox.angle);
    outlet_list(m_dataout, nullptr, 5, x_list);
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////

void pix_opencv_camshift :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::backProjectMessCallback,
		  gensym("backproject"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::vMinMessCallback,
		  gensym("vmin"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::vMaxMessCallback,
		  gensym("vmax"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::sMinMessCallback,
		  gensym("smin"), A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::trackMessCallback,
		  gensym("track"), A_FLOAT, A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::rWidthMessCallback,
		  gensym("rwidth"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_camshift::rHeightMessCallback,
		  gensym("rheight"), A_NULL);
}

void  pix_opencv_camshift :: backProjectMessCallback(void *data, t_floatarg backproject)
{
    GetMyClass(data)->backProjectMess((float)backproject);
}

void  pix_opencv_camshift :: vMinMessCallback(void *data, t_floatarg vmin)
{
    GetMyClass(data)->vMinMess((float)vmin);
}

void  pix_opencv_camshift :: vMaxMessCallback(void *data, t_floatarg vmax)
{
    GetMyClass(data)->vMaxMess((float)vmax);
}

void  pix_opencv_camshift :: sMinMessCallback(void *data, t_floatarg smin)
{
    GetMyClass(data)->sMinMess((float)smin);
}

void  pix_opencv_camshift :: trackMessCallback(void *data, t_floatarg px, t_floatarg py)
{
    GetMyClass(data)->trackMess((float)px, (float)py);
}

void  pix_opencv_camshift :: rWidthMessCallback(void *data, t_floatarg rwidth)
{
    GetMyClass(data)->rWidthMess((float)rwidth);
}

void  pix_opencv_camshift :: rHeightMessCallback(void *data, t_floatarg rheight)
{
    GetMyClass(data)->rHeightMess((float)rheight);
}

void  pix_opencv_camshift :: backProjectMess(float backproject)
{
   if ( ( (int)backproject==0 ) || ( (int)backproject==1 ) ) x_backproject = (int)backproject;
}

void  pix_opencv_camshift :: vMinMess(float vmin)
{
   if ( ( (int)vmin>=0 ) || ( (int)vmin<256 ) ) x_vmin = (int)vmin;
}

void  pix_opencv_camshift :: vMaxMess(float vmax)
{
   if ( ( (int)vmax>=0 ) || ( (int)vmax<256 ) ) x_vmax = (int)vmax;
}

void  pix_opencv_camshift :: sMinMess(float smin)
{
   if ( ( (int)smin>=0 ) || ( (int)smin<256 ) ) x_smin = (int)smin;
}

void  pix_opencv_camshift :: trackMess(float px, float py)
{
  int rx, ry, w, h;

  if ( ( px<0.0 ) || ( px>m_last_size.width ) || ( py<0.0 ) || ( py>m_last_size.height ) ) return;

  //py = comp_ysize - py;
  m_origin = cv::Point((int)px,(int)py);
  rx = ( (int)px-(x_rwidth/2) < 0 )? 0:(int)px-(x_rwidth/2);
  ry = ( (int)py-(x_rheight/2) < 0 )? 0:(int)py-(x_rheight/2);
  w = (rx+x_rwidth>m_last_size.width ) ? ( m_last_size.width - rx ):x_rwidth;
  h = (ry+x_rheight>m_last_size.height ) ? ( m_last_size.height - ry ):x_rheight;
  m_selection = cv::Rect(rx,ry,w,h);
  post( "pix_opencv_camshift : track point (%f,%f) region (%d %d %d %d)", px, py, rx, ry, w, h );
  x_track = 1;
  x_init = 1;
}

void  pix_opencv_camshift :: rWidthMess(float rwidth)
{
  if ( (int)rwidth>=0 ) x_rwidth = (int)rwidth;
  // refresh selection zone
  trackMess( (float)m_origin.x, (float)m_origin.y );
}

void  pix_opencv_camshift :: rHeightMess(float rheight)
{
  if ( (int)rheight>=0 ) x_rheight = (int)rheight;
  // refresh selection zone
  trackMess( (float)m_origin.x, (float)m_origin.y );
}

