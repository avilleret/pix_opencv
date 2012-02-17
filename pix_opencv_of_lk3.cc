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

#include "pix_opencv_of_lk3.h"
#include <stdio.h>

CPPEXTERN_NEW(pix_opencv_of_lk3)

/////////////////////////////////////////////////////////
//
// pix_opencv_of_lk3
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_of_lk3 :: pix_opencv_of_lk3()
{ 
  m_xsize=320;
  m_ysize=240;

  m_winsize = cvSize(9,9);

  velxArray = NULL;
  velyArray = NULL;
  //~ m_velxout = outlet_new(this->x_obj, 0);
  //~ m_velyout = outlet_new(this->x_obj, 0);

  grey = cvCreateImage( cvSize(m_xsize, m_ysize), 8, 1 );
  prev_grey = cvCreateImage( cvSize(m_xsize, m_ysize), 8, 1 );

    x_velx = cvCreateMat( m_ysize, m_xsize, CV_32FC1);
    x_vely = cvCreateMat( m_ysize, m_xsize, CV_32FC1);
    
    post("pix_opencv_of_lk3 build on %s at %s", __DATE__, __TIME__);

}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_of_lk3 :: ~pix_opencv_of_lk3()
{
  // Destroy cv_images
  cvReleaseImage( &grey );
  cvReleaseImage( &prev_grey );
  
  cvReleaseMat( &x_velx );
  cvReleaseMat( &x_vely );

}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_of_lk3 :: processRGBAImage(imageStruct &image)
{
  error( "pix_opencv_of_lk3 : rgba format not supported" );
}

void pix_opencv_of_lk3 :: processRGBImage(imageStruct &image)
{
  error( "pix_opencv_of_lk3 : rgb	 format not supported" );
}

void pix_opencv_of_lk3 :: processYUVImage(imageStruct &image)
{
  error( "pix_opencv_of_lk3 : yuv format not supported" );
}
    	
void pix_opencv_of_lk3 :: processGrayImage(imageStruct &image)
{ 
	  int px,py;
	  int i, j, length;
	  float outX, outY;

	  if ((this->m_xsize!=image.xsize)&&(this->m_ysize!=image.ysize)) 
	  {
		this->m_xsize=image.xsize;
		this->m_ysize=image.ysize;

		cvReleaseImage( &grey );
		cvReleaseImage( &prev_grey );
		cvReleaseMat( &x_velx );
		cvReleaseMat( &x_vely );

		grey = cvCreateImage( cvSize(m_xsize, m_ysize), 8, 1 );
		prev_grey = cvCreateImage( cvSize(m_xsize, m_ysize), 8, 1 );
		
		x_velx = cvCreateMat( m_ysize, m_xsize, CV_32FC1);
		x_vely = cvCreateMat( m_ysize, m_xsize, CV_32FC1);

	}

	memcpy( grey->imageData, image.data, image.xsize*image.ysize );

	cvCalcOpticalFlowLK( prev_grey, grey,
                       m_winsize,
                       x_velx, x_vely  );
                       
	memcpy( prev_grey->imageData, grey->imageData, image.xsize*image.ysize );
	
	// prepare output
	image.csize = 4;
	image.format = GL_RGBA;
	image.type = GL_UNSIGNED_BYTE;
	image.reallocate(image.xsize*image.ysize*image.csize*sizeof(char));
	
	unsigned char *buffer = image.data;
	char *data = grey->imageData;

	for ( i = 0 ; i < image.xsize*image.ysize ; i++) {
		outX = CV_MAT_ELEM( *x_velx, float, i/m_xsize, i%m_xsize );
		outY = CV_MAT_ELEM( *x_vely, float, i/m_xsize, i%m_xsize );
				
		buffer[0] = CLAMP( outX + 128.f );	// red
		buffer[1] = CLAMP( outY + 128.f );	// green
		buffer[2] = data[i];				// blue
		buffer[3] = 0;						// alpha
		buffer += 4;
	}


}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////

void pix_opencv_of_lk3 :: obj_setupCallback(t_class *classPtr)
{
	CPPEXTERN_MSG2(classPtr, "winsize", winSizeMess, t_float, t_float);
  
  //~ class_addmethod(classPtr, (t_method)&pix_opencv_of_lk3::winSizeMessCallback,
		  //~ gensym("winsize"), A_FLOAT, A_FLOAT, A_NULL);
}

void  pix_opencv_of_lk3 :: winSizeMessCallback(void *data, t_floatarg fwidth, t_floatarg fheight)
{
    GetMyClass(data)->winSizeMess((float)fwidth, (float)fheight);
}

void  pix_opencv_of_lk3 :: winSizeMess(float fwidth, float fheight)
{
  if ((fwidth>=3.0)&&(fwidth<=100.0)) m_winsize.width = (int)fwidth;
  if ((fheight>=3.0)&&(fheight<=100.0)) m_winsize.height = (int)fheight;
}

