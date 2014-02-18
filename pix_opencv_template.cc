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

#include "pix_opencv_template.h"
#include <stdio.h>
#include <opencv/highgui.h>
#include <RTE/MessageCallbacks.h>


CPPEXTERN_NEW(pix_opencv_template)

/////////////////////////////////////////////////////////
//
// pix_opencv_template
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_template :: pix_opencv_template()
{ 
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_template :: ~pix_opencv_template()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
///////////////////////////////////////////////////////// 	
void pix_opencv_template :: processImage(imageStruct &image)
{ 
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_template :: obj_setupCallback(t_class *classPtr)
{
}
