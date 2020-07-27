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
// based on code written by Lluis Gomez i Bigorda ( lluisgomez _at_ hangar _dot_ org ) (pix_opencv)
// pix_opencv_opticalflow compute optical flow, several algorithms are available in one object
// by Antoine Villeret - 2012

#include "pix_opencv_opticalflow.hpp"
#include "pix_opencv_utils.hpp"

CPPEXTERN_NEW(pix_opencv_opticalflow)

/////////////////////////////////////////////////////////
//
// pix_opencv_opticalflow
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_opticalflow :: pix_opencv_opticalflow() : m_gain(1.)
{ 
	m_dataout_middle = outlet_new(this->x_obj, 0);
	m_dataout_right = outlet_new(this->x_obj, 0);
  
	//~ post("build on %s at %s", __DATE__, __TIME__);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_opticalflow :: ~pix_opencv_opticalflow()
{ 
}

/////////////////////////////////////////////////////////
// render
//
/////////////////////////////////////////////////////////
void pix_opencv_opticalflow :: processImage(imageStruct &image)
{ 
  if ( image.xsize <= 0 || image.ysize <= 0 ) return;

  m_curr = image2mat_gray(image);
  
  if (m_prev.size() != m_curr.size()){
      m_prev = m_curr.clone();
  }
  //~cv::calcOpticalFlowSF(m_curr, m_prev, m_flow, 3, 2, 4, 4.1, 25.5, 18, 55.0, 25.5, 0.35, 18, 55.0, 25.5, 10);
  
  cv::calcOpticalFlowFarneback(m_curr, m_prev, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
  m_prev = m_curr.clone(); // copy data

  cv::Size size = m_curr.size();
  // here is the idiom: check the arrays for continuity and,
  // if this is the case,
  // treat the arrays as 1D vectors
  if( m_curr.isContinuous() && m_prev.isContinuous() && m_flow.isContinuous() )
  {
      size.width *= size.height;
      size.height = 1;
  }
  
  float gain=m_gain;
  if ( m_normalize ){
    float maxrad=1;
  
    for (int y = 0; y < flow.rows; ++y)
    {
        for (int x = 0; x < flow.cols; ++x)
        {
            cv::Point2f u = flow.at<cv::Point2f>(x,y);

            if (!isFlowCorrect(u))
                continue;
            float rad = sqrt(u.x * u.x + u.y * u.y);
            maxrad = maxrad>rad?maxrad:rad;
        }
    }
    gain=1/maxrad;
  }

  for( int i = 0; i < size.height; i++ )
  {
      // when the arrays are continuous,
      // the outer loop is executed only once
      const float* ptrFlow = flow.ptr<float>(i);
      unsigned char* data=image.data+i*image.csize*image.xsize;

      
      for( int j = 0; j < 2*size.width; j+=2 )
      {
        float fx = ptrFlow[j];
        float fy = ptrFlow[j+1];

        cv::Vec3b pix;
          
        pix = computeColor(fx*gain, fy*gain);
  
        for ( int k = 0; k < 3; k++ ){
          data[k]=pix[k];
        }
        //~printf("pix %d : %d\t%d\t%d\n",j+i*size.width, pix[0],pix[1], pix[2]);
        //~m_colorcode.computeColor(fx, fy, data);
        data+=4;
      }
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_opticalflow :: obj_setupCallback(t_class *classPtr)
{
	CPPEXTERN_MSG1(classPtr, "gain",	gainMess, 		double);		    		  	  
	CPPEXTERN_MSG1(classPtr, "normalize",	normalizeMess, 		double);		    		  	  
}

/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_opticalflow :: gainMess(double arg)
{
	m_gain = arg > 0 ? arg : 3.;
}

void pix_opencv_opticalflow :: normalizeMess(double arg)
{
	m_normalize = arg > 0;
}



///////////////////////////
// static function for color coding
///////////////////////////
using namespace cv;

static cv::Vec3b computeColor(float fx, float fy)
{
static bool first = true;

    // relative lengths of color transitions:
    // these are chosen based on perceptual similarity
    // (e.g. one can distinguish more shades between red and yellow
    //  than between yellow and green)
    const int RY = 15;
    const int YG = 6;
    const int GC = 4;
    const int CB = 11;
    const int BM = 13;
    const int MR = 6;
    const int NCOLS = RY + YG + GC + CB + BM + MR;
    static Vec3i colorWheel[NCOLS];

    if (first)
    {
        int k = 0;

        for (int i = 0; i < RY; ++i, ++k)
            colorWheel[k] = Vec3i(255, 255 * i / RY, 0);

        for (int i = 0; i < YG; ++i, ++k)
            colorWheel[k] = Vec3i(255 - 255 * i / YG, 255, 0);

        for (int i = 0; i < GC; ++i, ++k)
            colorWheel[k] = Vec3i(0, 255, 255 * i / GC);

        for (int i = 0; i < CB; ++i, ++k)
            colorWheel[k] = Vec3i(0, 255 - 255 * i / CB, 255);

        for (int i = 0; i < BM; ++i, ++k)
            colorWheel[k] = Vec3i(255 * i / BM, 0, 255);

        for (int i = 0; i < MR; ++i, ++k)
            colorWheel[k] = Vec3i(255, 0, 255 - 255 * i / MR);

        first = false;
    }

    const float rad = sqrt(fx * fx + fy * fy);
    const float a = atan2(-fy, -fx) / (float) CV_PI;

    const float fk = (a + 1.0f) / 2.0f * (NCOLS - 1);
    const int k0 = static_cast<int>(fk);
    const int k1 = (k0 + 1) % NCOLS;
    const float f = fk - k0;

    Vec3b pix;

    for (int b = 0; b < 3; b++)
    {
        const float col0 = colorWheel[k0][b] / 255.0f;
        const float col1 = colorWheel[k1][b] / 255.0f;

        float col = (1 - f) * col0 + f * col1;

        if (rad <= 1)
            col = 1 - rad * (1 - col); // increase saturation with radius
        else
            col *= .75; // out of range

        pix[2 - b] = static_cast<uchar>(255.0 * col);
    }

    return pix;
}

inline bool isFlowCorrect(cv::Point2f u)
{
    return !cvIsNaN(u.x) && !cvIsNaN(u.y) && fabs(u.x) < 1e9 && fabs(u.y) < 1e9;
}
