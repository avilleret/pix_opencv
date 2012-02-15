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
// based on code written by Lluis Gomez i Bigorda ( lluisgomez _at_ hangar _dot_ org )
// camera calibration function by Antoine Villeret helped by Cyrille Henry

#include "pix_opencv_calibration.h"
#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

CPPEXTERN_NEW(pix_opencv_calibration)

/////////////////////////////////////////////////////////
//
// pix_opencv_calibration
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_calibration :: pix_opencv_calibration()
{ 
	find_rgb = NULL;
	find_gray = NULL;
	rgb = NULL;
	gray = NULL;
	tmp = NULL;
	mapx = NULL;
	mapy = NULL;
	
	success_count = 0;
	board_view_nb = 10;
	calibration = 0; 
	patternSize[0] = 6;
	patternSize[1] = 7;
	frame = 0;
	wait_n_frame = 10;
	findChessFlag = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS;
  
	// allocate storage matrix
	image_points			=	cvCreateMat(patternSize[0]*patternSize[1]*board_view_nb, 2, CV_32FC1);
	object_points			=	cvCreateMat(patternSize[0]*patternSize[1]*board_view_nb, 3, CV_32FC1);
	point_counts			=	cvCreateMat(board_view_nb, 1, CV_32SC1);
	intrinsic_matrix 		= 	cvCreateMat(3, 3, CV_32FC1);
	distortion_coeffs 		= 	cvCreateMat(5, 1, CV_32FC1); 
	
	// make an "empty" intrinsinc matrix
	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 8;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 0 ) = 0;
	CV_MAT_ELEM( *intrinsic_matrix, float, 2, 0 ) = 0;
	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 1 ) = 0;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 8;
	CV_MAT_ELEM( *intrinsic_matrix, float, 2, 1 ) = 0;
	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 2 ) = 3;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 2 ) = 3;
	CV_MAT_ELEM( *intrinsic_matrix, float, 2, 2 ) = 1;

	// zeros distortion coeffs
	for ( int i = 0 ; i < 5 ; i++ ) {
	CV_MAT_ELEM( *distortion_coeffs, float, i, 0 ) = 0.0;
	}

	post("pix_opencv_calibration build on %s at %s", __DATE__, __TIME__);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_calibration :: ~pix_opencv_calibration()
{ 
    //Destroy cv_images to clean memory
	if ( find_rgb ) cvReleaseImage(&find_rgb);
	if ( find_gray ) cvReleaseImage(&find_gray);
	if ( gray ) cvReleaseImage(&gray);
	if ( rgb )  cvReleaseImage(&rgb);
	if ( tmp )  cvReleaseImage(&tmp);
	if ( mapx )	cvReleaseImage(&mapx);
	if ( mapy )	cvReleaseImage(&mapy);
	cvReleaseMat(&intrinsic_matrix);
	cvReleaseMat(&distortion_coeffs);
	cvReleaseMat(&image_points);
	cvReleaseMat(&object_points);
	cvReleaseMat(&point_counts);
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: processRGBAImage(imageStruct &image)
{ 
	if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!rgb)) { 
	
		// printf("process rgba image\n");
		this->comp_xsize = image.xsize;
		this->comp_ysize = image.ysize;
		if ( calibration ) error ( "image size changed, calibration was cancelled");
		calibration = 0;

		if ( find_rgb ) cvReleaseImage(&find_rgb);
		if ( find_gray ) cvReleaseImage(&find_gray);
		if ( gray )  cvReleaseImage(&gray); // TODO : cette ligne crash qd on passe du gray a couleur apres ou pendant calibration
		if ( rgb )   cvReleaseImage(&rgb);
		if ( tmp )   cvReleaseImage(&tmp);
		if ( mapx )  cvReleaseImage(&mapx);
		if ( mapy )  cvReleaseImage(&mapy);

		// used in findCorners
		find_rgb = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 4);
		find_gray = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 1);

		//create the images with new size
		rgb = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 4);
		tmp  = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 4);
		mapx = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_32F, 1);
		mapy = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_32F, 1);

		// create new map
		cvInitUndistortMap(this->intrinsic_matrix, this->distortion_coeffs, this->mapx, this->mapy);
    }
    
    // no need to copy a lot of memory, just point to it...
    rgb->imageData = (char*) image.data;

	// this will loop until we got enought views (x->board_view_nb) with all corners visible
	if ( success_count < board_view_nb && calibration != 0 ) {
		findCorners( rgb );
		image.data = (unsigned char*) rgb->imageData; 
	 }
	else if ( success_count >= board_view_nb && calibration != 0 ) {
		computeCalibration( rgb );
		image.data = (unsigned char*) rgb->imageData;
	 }
	else if ( this->calibration == 0 ) { 
		cvRemap(rgb,tmp,mapx,mapy);
		image.data = (unsigned char*) tmp->imageData;
	}
}

void pix_opencv_calibration :: processRGBImage(imageStruct &image) {
	error( "pix_opencv_calibration : rgb format not supported");
}

void pix_opencv_calibration :: processYUVImage(imageStruct &image) {
	error( "pix_opencv_calibration : yuv format not supported" );
}
    	
void pix_opencv_calibration :: processGrayImage(imageStruct &image)
{ 
	if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!gray)) { 
		
		// printf("process gray image\n");
		this->comp_xsize = image.xsize;
		this->comp_ysize = image.ysize;
		if ( calibration ) error ( "image size changed, calibration was cancelled");
		calibration = 0;

		if ( find_rgb ) cvReleaseImage(&find_rgb);
		if ( find_gray ) cvReleaseImage(&find_gray);
		if ( gray )  cvReleaseImage(&gray);
		if ( rgb )   cvReleaseImage(&rgb);
		if ( tmp )   cvReleaseImage(&tmp);
		if ( mapx )  cvReleaseImage(&mapx);
		if ( mapy )  cvReleaseImage(&mapy);
		
		// used in findCorners
		find_rgb = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 4);
		find_gray = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 1);
		
		//create the images with new size
		gray = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 1);
		tmp = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_8U, 1);
		mapx = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_32F, 1);
		mapy = cvCreateImage(cvSize(this->comp_xsize,this->comp_ysize), IPL_DEPTH_32F, 1);

		// create new map
		cvInitUndistortMap(this->intrinsic_matrix, this->distortion_coeffs, this->mapx, this->mapy);
    }
    
    // no need to copy a lot of memory, just point to it...
    gray->imageData = (char*) image.data;
    
	// this will loop until we got enought views (x->board_view_nb) with all corners visible
	if ( success_count < board_view_nb && calibration != 0 ) {
		findCorners( gray );
		image.data = (unsigned char*) gray->imageData;
	 }
	else if ( success_count >= board_view_nb && calibration != 0 ) {
		computeCalibration( gray );
		image.data = (unsigned char*) gray->imageData;
	 }
	else if ( this->calibration == 0 ) { 
		cvRemap(gray,tmp,mapx,mapy);
		image.data = (unsigned char*) tmp->imageData;
	}
}

/////////////////////////////////////////////////////////
// findCorners
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: findCorners ( IplImage *image )
{
	int					board_point_nb = this->patternSize[0]*this->patternSize[1];
	CvPoint2D32f		*corners = new CvPoint2D32f[board_point_nb];
	int					corner_count;
	int					step;
	CvSize				patternSize, image_size;
	CvMat				in_cv;
	
	patternSize = cvSize( this->patternSize[0], this->patternSize[1] );
	image_size = cvSize( image->width, image->height );
	// cvGetImage	(&in_cv,	&in_image);	// create an IplImage from a CvMat
	cvGetMat ( image, &in_cv ); // create a CvMat from IplImage

	// find chessboard corners (gray or RGBA image...)
	int found = cvFindChessboardCorners(image, 
										patternSize, 
										corners, 
										&corner_count, 
										findChessFlag);
	if (image->nChannels == 4) {
		cvCvtColor( image , find_gray , CV_RGBA2GRAY); // convert color to gray
	} else {
		cvCopy(image, find_gray) ;
	}
	
	// get subpixel accuracy on those corners (grayscale image only)
	cvFindCornerSubPix(find_gray, 
					   corners, 
					   corner_count, 
					   cvSize(11,11), 
					   cvSize(-1,-1), 
					   cvTermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1));

	cvCvtColor( find_gray , find_rgb , CV_GRAY2RGBA); // convert gray to color

	// draw chessboard corner (color image only)
	cvDrawChessboardCorners(find_rgb, patternSize, corners, corner_count, found);

	this->frame++;
	if ( this->frame % this->wait_n_frame == 0 ) {
	// update arrays

		if( corner_count == board_point_nb ) {
			step = this->success_count*board_point_nb;
			for( int i=step, j=0; j<board_point_nb; ++i,++j ) {
				CV_MAT_ELEM(*this->image_points, float,i,0) = corners[j].x; 
				CV_MAT_ELEM(*this->image_points, float,i,1) = corners[j].y; 
				CV_MAT_ELEM(*this->object_points,float,i,0) = j/this->patternSize[0]; 
				CV_MAT_ELEM(*this->object_points,float,i,1) = j%this->patternSize[0]; 
				CV_MAT_ELEM(*this->object_points,float,i,2) = 0.0f;
			}
			CV_MAT_ELEM(*this->point_counts, int,this->success_count,0) = board_point_nb; 
			this->success_count++;

			cvNot( find_rgb , find_rgb );
		}
		post("take : %d/%d\n", success_count, board_view_nb);
	}

	// convert color to gray
	if (image->nChannels == 1) {
		cvCvtColor( find_rgb , image, CV_RGBA2GRAY); // convert color to gray
	} else {
		cvCopy(find_rgb, image);
	}	

}
/////////////////////////////////////////////////////////
// computeCalibration
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: computeCalibration ( IplImage *image )
{	
	//CALIBRATE THE CAMERA! 
	cvCalibrateCamera2(this->object_points,
					   this->image_points,
					   this->point_counts,
					   cvSize( image->width , image->height ),
					   this->intrinsic_matrix,
					   this->distortion_coeffs,
					   NULL, 
					   NULL,
					   0);
	cvReleaseImage(&mapx);
	cvReleaseImage(&mapy);
	this->mapx = cvCreateImage( cvSize( image->width, image->height ), IPL_DEPTH_32F, 1 );
	this->mapy = cvCreateImage( cvSize( image->width, image->height ), IPL_DEPTH_32F, 1 );

	cvInitUndistortMap(this->intrinsic_matrix, this->distortion_coeffs, this->mapx, this->mapy);
	calibration = 0;
}
/////////////////////////////////////////////////////////
// LoadMess
//
/////////////////////////////////////////////////////////

void pix_opencv_calibration :: loadIntraMess (t_symbol *filename)
{
	if ( filename == NULL ) { error("NULL pointer passed to function loadIntra"); return;}
	this->intrinsic_matrix = (CvMat*)cvLoad(filename->s_name, 0, 0, 0);
  
	if (intrinsic_matrix == NULL) {
		intrinsic_matrix = 	cvCreateMat(3, 3, CV_32FC1);
		post("can't open file %s", filename->s_name); 
		CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 8;
		CV_MAT_ELEM( *intrinsic_matrix, float, 1, 0 ) = 0;
		CV_MAT_ELEM( *intrinsic_matrix, float, 2, 0 ) = 0;
		CV_MAT_ELEM( *intrinsic_matrix, float, 0, 1 ) = 0;
		CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 8;
		CV_MAT_ELEM( *intrinsic_matrix, float, 2, 1 ) = 0;
		CV_MAT_ELEM( *intrinsic_matrix, float, 0, 2 ) = 3;
		CV_MAT_ELEM( *intrinsic_matrix, float, 1, 2 ) = 3;
		CV_MAT_ELEM( *intrinsic_matrix, float, 2, 2 ) = 1;
	}
	else post("load transformation matrix from %s",filename->s_name);

	// reinitialise size to force reinitialisation of mapx and mapy on next frame
	this->comp_xsize = 0;  
}

void pix_opencv_calibration :: loadDistMess (t_symbol *filename)
{
	if ( filename == NULL ) { error("NULL pointer passed to function loadDist"); return;}
	distortion_coeffs = (CvMat*)cvLoad(filename->s_name);
	if (distortion_coeffs == NULL){
		distortion_coeffs = cvCreateMat(5, 1, CV_32FC1); 
		post("can't open file %s", filename->s_name); 
		// zeros distortion coeffs
		for ( int i = 0 ; i < 5 ; i++ ) {
		CV_MAT_ELEM( *distortion_coeffs, float, i, 0 ) = 0.0;
		}
	}
	else post("load distortion coefficients from %s",filename->s_name);

	// reinitialise size to force reinitialisation of mapx and mapy on next frame
	this->comp_xsize = 0;  
}

void pix_opencv_calibration :: writeIntraMess (t_symbol *filename)
{
	// printf("write intrinsic matrix\n");
	cvSave(filename->s_name,intrinsic_matrix);
}

void pix_opencv_calibration :: writeDistMess (t_symbol *filename)
{
	// printf("write distorsion coeffs\n");
	cvSave(filename->s_name,distortion_coeffs);
}

void pix_opencv_calibration :: floatCalibrationhMess (float calib_flag) 
{
	this->calibration=calib_flag;
	if ( this->calibration == 1 ) {
		this->success_count = 0;
		this->frame = 0;
	}
	post("calibration : %d", this->calibration);
}

void pix_opencv_calibration :: patternSizeMess (float xsize, float ysize)
{
	if (calibration) error("you can't change pattern size during calibration"); return;
	if ( xsize < 3 || ysize < 3 ) error("patternSize should be at least 3x3"); return;
	this->patternSize[0]=xsize;
	this->patternSize[1]=ysize;
}

void pix_opencv_calibration :: viewMess (int view)
{     
	if ( calibration == 1 ) {error("you can't change view number during calibration !"); return;}
	board_view_nb=view<2?2:view;
	if (view < 2) error("view should be greater or equal to 2");

	cvReleaseMat(&image_points);
	cvReleaseMat(&object_points);
	cvReleaseMat(&point_counts);
	image_points	=	cvCreateMat(patternSize[0]*patternSize[1]*board_view_nb, 2, CV_32FC1);
	object_points	=	cvCreateMat(patternSize[0]*patternSize[1]*board_view_nb, 3, CV_32FC1);
	point_counts	=	cvCreateMat(board_view_nb, 1, CV_32SC1);

}

void pix_opencv_calibration :: waitMess (int wait)
{
	wait_n_frame=wait<1?1:wait;
	if (wait < 1) error("wait should be greater or equal to 1, you can't calibrate more often than each frame !");
}

void pix_opencv_calibration :: findChessFlagMess(int adaptThres, int normalize, int filter)
{
	adaptThres=adaptThres<=0?0:adaptThres>=1?1:adaptThres;
	normalize=normalize<=0?0:normalize>=1?1:normalize;
	filter=filter<=0?0:filter>=1?1:filter;
	findChessFlag = CV_CALIB_CB_ADAPTIVE_THRESH * adaptThres + CV_CALIB_CB_NORMALIZE_IMAGE * normalize + CV_CALIB_CB_FILTER_QUADS * filter;
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::loadIntraMessCallback,
  		  gensym("loadIntra"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::loadDistMessCallback,
  		  gensym("loadDist"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::writeIntraMessCallback,
  		  gensym("writeIntra"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::writeDistMessCallback,
  		  gensym("writeDist"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::floatCalibrationMessCallback,
  		  gensym("calibration"), A_FLOAT, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::patternSizeMessCallback,
  		  gensym("patternSize"), A_FLOAT, A_FLOAT, A_NULL);	 
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::viewMessCallback,
  		  gensym("view"), A_FLOAT, A_NULL);  
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::waitMessCallback,
  		  gensym("wait"), A_FLOAT, A_NULL); 
    class_addmethod(classPtr, (t_method)&pix_opencv_calibration::findChessFlagMessCallback,
  		  gensym("findChessFlag"), A_FLOAT, A_FLOAT, A_FLOAT, A_NULL);    
  		    		  	  
}
void pix_opencv_calibration :: loadIntraMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->loadIntraMess(filename);
}
void pix_opencv_calibration :: loadDistMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->loadDistMess(filename);
}
void pix_opencv_calibration :: writeIntraMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->writeIntraMess(filename);
}
void pix_opencv_calibration :: writeDistMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->writeDistMess(filename);
}
void pix_opencv_calibration :: floatCalibrationMessCallback(void *data, t_floatarg calib_flag)
{
		GetMyClass(data)->floatCalibrationhMess((float)calib_flag);
}
void pix_opencv_calibration :: patternSizeMessCallback(void *data, t_floatarg xsize, t_floatarg ysize)
{
		GetMyClass(data)->patternSizeMess((float)xsize, (float)ysize);
}
void pix_opencv_calibration :: viewMessCallback(void *data, t_floatarg view)
{
		GetMyClass(data)->viewMess((int)view);
}
void pix_opencv_calibration :: waitMessCallback(void *data, t_floatarg wait)
{
		GetMyClass(data)->waitMess((int)wait);
}
void pix_opencv_calibration :: findChessFlagMessCallback(void *data, t_floatarg adaptThres, t_floatarg normalize, t_floatarg filter)
{
		GetMyClass(data)->findChessFlagMess((int) adaptThres, (int) normalize, (int) filter);
}

