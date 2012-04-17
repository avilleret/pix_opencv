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
pix_opencv_contours_convexhull2 :: pix_opencv_contours_convexhull2()
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
	Mat imgMat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
	
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Point>> one_contour;
	cv::findContours(imgMat, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	
	m_contours.clear();
	
	for( size_t i = 0; i < contours.size(); i++ ) {
		if ( cv::contourArea(contours[i], false) > m_area_threshold ){
			approxPolyDP(Mat(contours0[k]), one_contour, 3, true);
			m_contours.push_back(one_contour);
		}
	}
	
	for( size_t i = 0; i < m_contours.size(); i++ ) {
	}
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_contours_convexhull2 :: obj_setupCallback(t_class *classPtr)
{
  		    		    		  	  
}
