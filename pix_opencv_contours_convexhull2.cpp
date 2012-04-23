////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
// based on code written by Lluis Gomez i Bigorda ( lluisgomez _at_ hangar _dot_ org ) (pix_opencv)
// Template for pix_opencv class

#include "pix_opencv_contours_convexhull2.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>


CPPEXTERN_NEW(pix_opencv_contours_convexhull2)

/////////////////////////////////////////////////////////
//
// pix_opencv_contours_convexhull2
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_contours_convexhull2 :: pix_opencv_contours_convexhull2() : m_area_threshold(30)
{ 
	m_dataout = outlet_new(this->x_obj, 0);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_contours_convexhull2 :: ~pix_opencv_contours_convexhull2()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull2 :: processRGBAImage(imageStruct &image)
{ 
	error( "pix_opencv_contours_convexhull2 : rgba format not supported" );
}

void pix_opencv_contours_convexhull2 :: processRGBImage(imageStruct &image) {
	error( "pix_opencv_contours_convexhull2 : rgb format not supported");
}

void pix_opencv_contours_convexhull2 :: processYUVImage(imageStruct &image) {
	error( "pix_opencv_contours_convexhull2 : yuv format not supported" );
}
    	
void pix_opencv_contours_convexhull2 :: processGrayImage(imageStruct &image)
{ 
	if ( image.xsize < 0 || image.ysize < 0 ) return;

	cv::Mat imgMat2( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
	
	cv::Mat imgMat = imgMat2.clone();
		
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Point> one_contour;		

	contours.clear();
	m_contours.clear();

	cv::findContours(imgMat, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
		
	for( size_t i = 0; i < contours.size(); i++ ) {
		if ( cv::contourArea(contours[i], false) > m_area_threshold ){
			one_contour.clear();
			cv::approxPolyDP(contours[i], one_contour, m_epsilon, true);
			m_contours.push_back(one_contour);
		}
	}
	//~ cv::drawContours(imgMat2, m_contours, -1, cv::Scalar(128,255,255), 3);

	int i = 0;
	for( std::vector<std::vector<cv::Point> >::iterator it = m_contours.begin(); it != m_contours.end(); ++it ) {
		if (!it->empty()) {
			int size = 2+it->size()*2;
			t_atom*ap = new t_atom[size];
			SETFLOAT(ap, static_cast<t_float>(it->size()));
			SETFLOAT(ap+1, 2.0);
			
			t_atom* pt=ap+2;
			int offset = 2;
			
			for ( std::vector<cv::Point>::iterator ite=it->begin(); ite!=it->end(); ++ite){
				SETFLOAT(ap+offset,(float) (*ite).x/image.xsize);
				SETFLOAT(ap+offset+1,(float) (*ite).y/image.ysize);
				offset+=2;
			}
			
			outlet_anything(m_dataout, gensym("contours"), size, ap);
			if(ap)delete[]ap;ap=NULL;
		}
	}	
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull2 :: obj_setupCallback(t_class *classPtr)
{
	CPPEXTERN_MSG1(classPtr, "epsilon",	epsilonMess, 		double);		    		  	  
	CPPEXTERN_MSG1(classPtr, "area",	areaMess, 			double);		    		  	  
}

/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull2 :: epsilonMess(double arg)
{
	m_epsilon = arg > 0 ? arg : 3.;
	t_atom data_out;
	SETFLOAT(&data_out, (float) m_epsilon);
	outlet_anything( m_dataout, gensym("epsilon"), 1, &data_out);
}
void pix_opencv_contours_convexhull2 :: areaMess(double arg)
{
	m_area_threshold = arg > 0 ? arg : 30.;
	t_atom data_out;
	SETFLOAT(&data_out, (float) m_area_threshold);
	outlet_anything( m_dataout, gensym("area"), 1, &data_out);
}
