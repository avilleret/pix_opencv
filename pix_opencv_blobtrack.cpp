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
//

#include "pix_opencv_blobtrack.h"
#include <stdio.h>
#include <opencv/highgui.h>
#include <RTE/MessageCallbacks.h>

//~ #include "cameraparams.h"



CPPEXTERN_NEW(pix_opencv_blobtrack)

/////////////////////////////////////////////////////////
//
// pix_opencv_blobtrack
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_blobtrack :: pix_opencv_blobtrack()
{ 
	m_dataout = outlet_new(this->x_obj, 0);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_blobtrack :: ~pix_opencv_blobtrack()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: processRGBAImage(imageStruct &image)
{ 
	error( "pix_opencv_blobtrack : rgba format not supported" );
}

void pix_opencv_blobtrack :: processRGBImage(imageStruct &image) {
	error( "pix_opencv_blobtrack : rgb format not supported");
}

void pix_opencv_blobtrack :: processYUVImage(imageStruct &image) {
	error( "pix_opencv_blobtrack : yuv format not supported" );
}
    	
void pix_opencv_blobtrack :: processGrayImage(imageStruct &image)
{ 
	Mat imgMat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: obj_setupCallback(t_class *classPtr)
{
  		    		    		  	  
}
