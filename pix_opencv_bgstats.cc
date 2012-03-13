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

#include "pix_opencv_bgstats.h"

CPPEXTERN_NEW(pix_opencv_bgstats)

/////////////////////////////////////////////////////////
//
// pix_opencv_bgstats
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_bgstats :: pix_opencv_bgstats()
{ 
  comp_xsize=320;
  comp_ysize=240;

  x_erode = 1;
  x_minarea = CV_BGFG_FGD_MINAREA;
  x_alpha = CV_BGFG_FGD_ALPHA_1;
  x_frames = 0;

  foreground = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
  incoming = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
  rgba = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 4);
  rgb  = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 3);
  gray = cvCreateImage(cvSize(comp_xsize,comp_ysize), IPL_DEPTH_8U, 1);
  x_model = NULL;

}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_bgstats :: ~pix_opencv_bgstats()
{ 
   //Destroy cv_images to clean memory
   cvReleaseImage(&rgba);
   cvReleaseImage(&rgb);
   cvReleaseImage(&gray);
   cvReleaseImage(&foreground);
   cvReleaseImage(&incoming);
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_bgstats :: processRGBAImage(imageStruct &image)
{

  if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!rgba)) {

	this->comp_xsize = image.xsize;
	this->comp_ysize = image.ysize;
        x_frames = 0;

    	//Destroy cv_images to clean memory
	cvReleaseImage(&rgba);
    	cvReleaseImage(&rgb);
    	cvReleaseImage(&gray);
        cvReleaseImage(&foreground);
        cvReleaseImage(&incoming);

	//create the orig image with new size
        rgba = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 4);
        rgb  = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 3);
    	gray = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 1);

        incoming  = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 3);
        foreground  = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 3);

        cvReleaseBGStatModel( &x_model );
        x_model = NULL;

    }
    memcpy( rgba->imageData, image.data, image.xsize*image.ysize*4 );
    cvCvtColor(rgba, incoming, CV_BGRA2BGR);

    if ( x_model == NULL || x_reset_flag )
    {
      x_model = cvCreateFGDStatModel( incoming );
      x_reset_flag = 0;
    }
    else if ( x_frames == 5 )
    {
      // strange model stabilize after a few frames
      //cvReleaseBGStatModel( &x_model );
      //x_model = cvCreateFGDStatModel( incoming, &x_modelparams );
    }
    else
    {
       cvUpdateBGStatModel( incoming, x_model );
    }

    x_frames++;
    cvCvtColor(x_model->foreground, rgba, CV_GRAY2BGRA);
    
    memcpy( image.data, rgba->imageData, image.xsize*image.ysize*4 );
}

void pix_opencv_bgstats :: processRGBImage(imageStruct &image)
{

  if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!rgba)) {

	this->comp_xsize = image.xsize;
	this->comp_ysize = image.ysize;
        x_frames = 0;

    	//Destroy cv_images to clean memory
	cvReleaseImage(&rgba);
    	cvReleaseImage(&rgb);
    	cvReleaseImage(&gray);
        cvReleaseImage(&foreground);
        cvReleaseImage(&incoming);

	//create the orig image with new size
        rgba = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 4);
        rgb  = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 3);
    	gray = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 1);

        incoming  = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 3);
        foreground  = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 3);

        cvReleaseBGStatModel( &x_model );
        x_model = NULL;
    }
    memcpy( rgb->imageData, image.data, image.xsize*image.ysize*3 );
    memcpy( incoming->imageData, image.data, image.xsize*image.ysize*3 );
    
    if ( x_model == NULL || x_reset_flag)
    {
      x_model = cvCreateFGDStatModel( incoming );
      x_reset_flag = 0;
      
    }
    else if ( x_frames == 5 )
    {
      // strange model stabilize after a few frames
      //cvReleaseBGStatModel( &x_model );
      //x_model = cvCreateFGDStatModel( incoming, &x_modelparams );
    }
    else
    {
       cvUpdateBGStatModel( incoming, x_model );
    }

    x_frames++;
    cvCvtColor(x_model->foreground, rgb, CV_GRAY2BGR);
    
    memcpy( image.data, rgb->imageData, image.xsize*image.ysize*3 );

}

void pix_opencv_bgstats :: processYUVImage(imageStruct &image)
{
  post( "pix_opencv_bgstats : yuv format not supported" );
}
    	
void pix_opencv_bgstats :: processGrayImage(imageStruct &image)
{ 
  post( "pix_opencv_bgstats : gray format not supported" );
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_bgstats :: floatMinAreaMess (float minarea)
{
  if ( ( (int)minarea>0 ) && ( x_model != NULL ) )
  {
       x_minarea = minarea;
  }
}

void pix_opencv_bgstats :: floatErodeMess(float erode)
{
  if ( ( (int)erode>0 ) && ( x_model != NULL ) )
  {
       x_erode = (int)erode;
  }
}

void pix_opencv_bgstats :: floatAlphaMess(float alpha)
{
   if ( ( alpha>0.0 ) && ( x_model != NULL ) )
   {
       x_alpha = alpha;
   }
}

void pix_opencv_bgstats :: resetMess ()
{
  if ( x_model != NULL )
  {
	  x_reset_flag = 1;
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_bgstats :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_bgstats::floatMinAreaMessCallback,
  		  gensym("minarea"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_bgstats::floatErodeMessCallback,
  		  gensym("erode"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_bgstats::floatAlphaMessCallback,
  		  gensym("alpha"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_bgstats::resetMessCallback,
  		  gensym("reset"), A_NULL);

}

void pix_opencv_bgstats :: floatMinAreaMessCallback(void *data, t_floatarg minarea)
{
  GetMyClass(data)->floatMinAreaMess((float)minarea);
}

void pix_opencv_bgstats :: floatErodeMessCallback(void *data, t_floatarg erode)
{
  GetMyClass(data)->floatErodeMess((float)erode);
}

void pix_opencv_bgstats :: floatAlphaMessCallback(void *data, t_floatarg alpha)
{
  GetMyClass(data)->floatAlphaMess((float)alpha);
}

void pix_opencv_bgstats :: resetMessCallback(void *data)
{
  GetMyClass(data)->resetMess();
}
