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

#include "pix_opencv_contours_convexhull.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>


CPPEXTERN_NEW(pix_opencv_contours_convexhull)

/////////////////////////////////////////////////////////
//
// pix_opencv_contours_convexhull
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_contours_convexhull :: pix_opencv_contours_convexhull() : m_area_threshold(30)
{ 
	//~ m_contourout = outlet_new(this->x_obj, 0);
	m_dataout = outlet_new(this->x_obj, 0);
	
	//~ post("build on %s at %s", __DATE__, __TIME__);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_contours_convexhull :: ~pix_opencv_contours_convexhull()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull :: processRGBAImage(imageStruct &image)
{ 
	error( "pix_opencv_contours_convexhull : rgba format not supported" );
}

void pix_opencv_contours_convexhull :: processRGBImage(imageStruct &image) {
	error( "pix_opencv_contours_convexhull : rgb format not supported");
}

void pix_opencv_contours_convexhull :: processYUVImage(imageStruct &image) {
	error( "pix_opencv_contours_convexhull : yuv format not supported" );
}
    	
void pix_opencv_contours_convexhull :: processGrayImage(imageStruct &image)
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

	t_atom*info;
	info = new t_atom[(int) m_contours.size()*14+2];
	// info : 6x(contour_nb) matrix
	// info for each contour : area, rotrect corner (8 float), angle
	int count(0);
	SETFLOAT(info+1, 14.);
	int info_offset(2);

	
	for( std::vector<std::vector<cv::Point> >::iterator it = m_contours.begin(); it != m_contours.end(); ++it ) {
		if (!it->empty() && it->size() > 2) {
			int size = 2+it->size()*2;
			t_atom*ap = new t_atom[size];
			SETFLOAT(ap, static_cast<t_float>(it->size()));
			SETFLOAT(ap+1, 2.0);
			
			int offset(2);
			
			for ( std::vector<cv::Point>::iterator ite=it->begin(); ite!=it->end(); ++ite){
				SETFLOAT(ap+offset,(float) (*ite).x/image.xsize);
				SETFLOAT(ap+offset+1,(float) (*ite).y/image.ysize);
				offset+=2;
			}
			outlet_anything(m_dataout, gensym("contour"), size, ap);
			if(ap)delete[]ap;ap=NULL;	
			
			SETFLOAT(info+info_offset, (float) cv::contourArea(*it));
			
			
			cv::RotatedRect rot_rect = cv::minAreaRect(*it);
			cv::Point2f corners[4];
			rot_rect.points(corners);
			for (int j=0;j<4;j++) {
				SETFLOAT(info+info_offset+j*2+1, corners[j].x/image.xsize);
				SETFLOAT(info+info_offset+j*2+2, corners[j].y/image.ysize);
			}

			SETFLOAT(info+info_offset+9, rot_rect.center.x/image.xsize);
			SETFLOAT(info+info_offset+10, rot_rect.center.y/image.ysize);
			SETFLOAT(info+info_offset+11, rot_rect.size.width/image.xsize);
			SETFLOAT(info+info_offset+12, rot_rect.size.height/image.xsize);
			SETFLOAT(info+info_offset+13, rot_rect.angle);
			
			info_offset+=14;
			count++;
		}
	}
	SETFLOAT(info, (float) count);
	if (count) outlet_anything(m_dataout, gensym("info"), count*14+2, info);
	
	if (info) delete info;
	info = NULL;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull :: obj_setupCallback(t_class *classPtr)
{
	CPPEXTERN_MSG1(classPtr, "epsilon",	epsilonMess, 		double);		    		  	  
	CPPEXTERN_MSG1(classPtr, "area",	areaMess, 			double);		    		  	  
}

/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull :: epsilonMess(double arg)
{
	m_epsilon = arg > 0 ? arg : 3.;
	t_atom data_out;
	SETFLOAT(&data_out, (float) m_epsilon);
	outlet_anything( m_dataout, gensym("epsilon"), 1, &data_out);
}
void pix_opencv_contours_convexhull :: areaMess(double arg)
{
	m_area_threshold = arg > 0 ? arg : 30.;
	t_atom data_out;
	SETFLOAT(&data_out, (float) m_area_threshold);
	outlet_anything( m_dataout, gensym("area"), 1, &data_out);
}
