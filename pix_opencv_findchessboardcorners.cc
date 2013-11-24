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

#include "pix_opencv_findchessboardcorners.h"
#include <stdio.h>

CPPEXTERN_NEW(pix_opencv_findchessboardcorners)

/////////////////////////////////////////////////////////
//
// pix_opencv_findchessboardcorners
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_findchessboardcorners :: pix_opencv_findchessboardcorners()
{ 
  //inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("minarea"));
  //inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("maxarea"));
  m_dataout = outlet_new(this->x_obj, 0);
  //m_countout = outlet_new(this->x_obj, 0);
  comp_xsize  = 320;
  comp_ysize  = 240;
  gray = NULL;
  tmp_color = NULL;
  tmp_gray = NULL;
  rgb = NULL;
  
  pattern_size = cvSize(6,7);
  corners = new CvPoint2D32f[pattern_size.width * pattern_size.height];
  cornerCount = 0;
  win = cvSize(11, 11); 
  zero_zone = cvSize(-1,-1);
  flags = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS;
  criteria = cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1);
  coord_list = new t_atom[pattern_size.width * pattern_size.height * 2]; // all coordinates are packed in one list [x0 y0 x1 y1 .. xn yn(
  
  post("pix_opencv_findchessboardcorners by Antoine Villeret");
  post("build on %s at %s", __DATE__, __TIME__);

}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_findchessboardcorners :: ~pix_opencv_findchessboardcorners()
{ 
    	//Destroy cv_images to clean memory
    	if (gray) cvReleaseImage(&gray);
    	if (rgb) cvReleaseImage(&rgb);
    	if (tmp_color) cvReleaseImage(&tmp_color);
		if (tmp_gray) cvReleaseImage(&tmp_gray);
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_findchessboardcorners :: processRGBAImage(imageStruct &image)
{
  if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!rgb)) 
  {

	this->comp_xsize = image.xsize;
	this->comp_ysize = image.ysize;

    //Destroy cv_images to clean memory
    if(gray) cvReleaseImage(&gray);
    if(rgb) cvReleaseImage(&rgb);
    if(tmp_color) cvReleaseImage(&tmp_color);

	// Create images with new sizes
	rgb = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 4);
	tmp_gray = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 1);
    }
    
    // no need to copy a lot of memory, just point to it...
    rgb->imageData = (char*) image.data;    
    cvCvtColor( rgb, tmp_gray, CV_RGBA2GRAY);
    
	//~ printf("find corner...*/\n");
	int found = cvFindChessboardCorners( tmp_gray, pattern_size, corners, &cornerCount, flags);
	//~ printf(" found : %d\n",found);
	
	
	if ( found ) {
		//~ printf("find corner sub pix...\n");
		cvFindCornerSubPix( tmp_gray, corners, cornerCount, win, zero_zone, criteria);
	}
	
	// Draw corners on the image
	//cvCvtColor( gray, tmp_color, CV_GRAY2RGBA);
	cvDrawChessboardCorners( rgb, pattern_size, corners, cornerCount, found);
	//cvCvtColor( tmp_color, gray, CV_RGBA2GRAY);
	
    // send out corners screen coordinates if all corners have been found
    if (found) {
		int i;
		CvPoint2D32f *pt;
		pt = (CvPoint2D32f *) corners;
		for ( i=0; i<pattern_size.width * pattern_size.height ; i++ )
		{
			 SETFLOAT(&coord_list[i*2], pt->x);
			 SETFLOAT(&coord_list[i*2+1], pt->y);
			 pt++;
		}
		outlet_list( m_dataout, 0, pattern_size.width * pattern_size.height*2, coord_list );
	}
    //~ printf("process RGBA image done\n");
}

void pix_opencv_findchessboardcorners :: processRGBImage(imageStruct &image)
{
	// TODO
    error("cant't support RGB image for now");
}

void pix_opencv_findchessboardcorners :: processYUVImage(imageStruct &image)
{
	// TODO
	error( "pix_opencv_findchessboardcorners : yuv format not supported" );
}
    	
void pix_opencv_findchessboardcorners :: processGrayImage(imageStruct &image)
{ 
	//~ printf("process gray image\n");
  if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!gray)) 
  {

	this->comp_xsize = image.xsize;
	this->comp_ysize = image.ysize;

    //Destroy cv_images to clean memory
    if(gray) cvReleaseImage(&gray);
    if(rgb) cvReleaseImage(&rgb);
    if(tmp_color) cvReleaseImage(&tmp_color);
    if(tmp_gray) cvReleaseImage(&tmp_gray);

	// Create images with new sizes
	gray = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 1);
	tmp_color = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 4);
    }
    
    // no need to copy a lot of memory, just point to it...
    gray->imageData = (char*) image.data;    
    
	//~ printf("find corner...*/\n");
	int found = cvFindChessboardCorners( gray, pattern_size, corners, &cornerCount, flags);
	//~ printf(" found : %d\n",found);
	
	
	if ( found ) {
		//~ printf("find corner sub pix...\n");
		cvFindCornerSubPix( gray, corners, cornerCount, win, zero_zone, criteria);
	}
	
	// Draw corners on the image
	cvCvtColor( gray, tmp_color, CV_GRAY2RGBA);
	cvDrawChessboardCorners( tmp_color, pattern_size, corners, cornerCount, found);
	cvCvtColor( tmp_color, gray, CV_RGBA2GRAY);
	
    // send out corners screen coordinates if all corners have been found
    if (found) {
		int i;
		CvPoint2D32f *pt;
		pt = (CvPoint2D32f *) corners;
		for ( i=0; i<pattern_size.width * pattern_size.height ; i++ )
		{
			 SETFLOAT(&coord_list[i*2], pt->x);
			 SETFLOAT(&coord_list[i*2+1], pt->y);
			 pt++;
		}
		outlet_list( m_dataout, 0, pattern_size.width * pattern_size.height*2, coord_list );
	}
    //~ printf("process gray image done\n");
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_findchessboardcorners :: patternSizeMess (int xsize, int ysize)
{
	if ( xsize < 3 || ysize < 3 ) {
		error("patternSize should be at least 3x3"); 
		return;
	}
	pattern_size=cvSize(xsize,ysize);
	// update corners array & output list size
	delete coord_list;
	coord_list = new t_atom[pattern_size.width * pattern_size.height * 2];
	delete corners;
	corners = new CvPoint2D32f[pattern_size.width * pattern_size.height];
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_findchessboardcorners :: obj_setupCallback(t_class *classPtr)
{
	// TODO add support for message : win, zero_zone, flags and criteria
	  class_addmethod(classPtr, (t_method)&pix_opencv_findchessboardcorners::patternSizeMessCallback,
  		  gensym("patternSize"), A_FLOAT, A_FLOAT, A_NULL);	 
}

void pix_opencv_findchessboardcorners :: patternSizeMessCallback(void *data, t_floatarg xsize, t_floatarg ysize)
{
		GetMyClass(data)->patternSizeMess((int)xsize, (int)ysize);
}
