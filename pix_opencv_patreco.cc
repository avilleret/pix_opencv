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
// also based on Georgios Evangelidis' work  ( georgios _dot_ evangelidis __at_ inria _dot_ fr ) (ARma)
// ported for Gem by Antoine Villeret - 2012

#define PAT_SIZE 64

#include "pix_opencv_patreco.h"
#include <stdio.h>
#include <opencv/highgui.h>
//~ #include "cameraparams.h"



CPPEXTERN_NEW(pix_opencv_patreco)

/////////////////////////////////////////////////////////
//
// pix_opencv_patreco
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_patreco :: pix_opencv_patreco()
{ 
	m_dataout = outlet_new(this->x_obj, 0);
	
	m_patternCount = 0;
	
	int norm_pattern_size = PAT_SIZE;
	double fixed_thresh = 40;
	double adapt_thresh = 5;//non-used with FIXED_THRESHOLD mode
	int adapt_block_size = 45;//non-used with FIXED_THRESHOLD mode
	double confidenceThreshold = 0.35;
	int mode = 2;//1:FIXED_THRESHOLD, 2: ADAPTIVE_THRESHOLD
	
	m_detector = new ARma::PatternDetector( fixed_thresh, adapt_thresh, adapt_block_size, confidenceThreshold, norm_pattern_size, mode);

	post("pix_opencv_patreco build on %s at %s", __DATE__, __TIME__);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_patreco :: ~pix_opencv_patreco()
{ 
	delete m_detector;
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_patreco :: processRGBAImage(imageStruct &image)
{ 
	error( "pix_opencv_patreco : rgba format not supported" );
}

void pix_opencv_patreco :: processRGBImage(imageStruct &image) {
	error( "pix_opencv_patreco : rgb format not supported");
}

void pix_opencv_patreco :: processYUVImage(imageStruct &image) {
	error( "pix_opencv_patreco : yuv format not supported" );
}
    	
void pix_opencv_patreco :: processGrayImage(imageStruct &image)
{ 
	Mat imgMat( image.xsize, image.ysize, CV_8UC1, image.data, image.csize*image.xsize);
    m_detector->detect(imgMat, m_cameraMatrix, m_distortions, m_patternLibrary, m_detectedPattern);
}

/////////////////////////////////////////////////////////
// LoadMess
//
/////////////////////////////////////////////////////////

void pix_opencv_patreco :: loadIntraMess (t_symbol *filename)
{
	CvMat* intrinsic_matrix;

	if ( filename->s_name[0] == 0 ) {
		error("no filename passed to loadIntra message");
		return;
	}
	if ( filename == NULL ) { error("%s is not a valid matrix", filename->s_name); return;}
	intrinsic_matrix = (CvMat*)cvLoad(filename->s_name, 0, 0, 0);// TODO crash when passing non-XML file
  
	if (intrinsic_matrix == NULL) {
		intrinsic_matrix = 	cvCreateMat(3, 3, CV_32FC1);
		error("can't open file %s", filename->s_name);
		//~ resetCorrectionMatrix();
	}
	else if ( intrinsic_matrix->rows != 3 || intrinsic_matrix->cols != 3 || CV_MAT_TYPE(intrinsic_matrix->type) != CV_32FC1 ) {
		error("%s is not a valid intrinsic matrix", filename->s_name);
		cvReleaseMat(&intrinsic_matrix);
		intrinsic_matrix = 	cvCreateMat(3, 3, CV_32FC1);
		//~ resetCorrectionMatrix();
	}
	else post("load transformation matrix from %s",filename->s_name);
	
	m_cameraMatrix = cvarrToMat(intrinsic_matrix);

	t_atom intra_out[9];
	for ( int i = 0 ; i < 9 ; i++ ){
		SETFLOAT(&intra_out[i], CV_MAT_ELEM( *intrinsic_matrix, float, i%3, i/3));
	}		
	outlet_anything( this->m_dataout, gensym("intrinsic_matrix"), 9, intra_out);
}

void pix_opencv_patreco :: loadDistMess (t_symbol *filename)
{
	CvMat* distortion_coeffs;
		
	if ( filename->s_name[0] == 0 ) {
		error("no filename passed to loadDist message");
		return;
	}
	if ( filename == NULL ) { error("NULL pointer passed to function loadDist"); return;}
	distortion_coeffs = (CvMat*)cvLoad(filename->s_name); // TODO crash when passing non-XML file
	
	if (distortion_coeffs == NULL) { 
		distortion_coeffs = 	cvCreateMat(5, 1, CV_32FC1);
		error("can't open file %s", filename->s_name);
		//~ resetCorrectionMatrix();
	}
	else if( distortion_coeffs->rows != 5 || distortion_coeffs->cols != 1 || CV_MAT_TYPE(distortion_coeffs->type) != CV_32FC1 ) {
		error("%s is not a valid distortions coeffs file", filename->s_name);
		cvReleaseMat(&distortion_coeffs);
		distortion_coeffs = 	cvCreateMat(3, 3, CV_32FC1);
		//~ resetCorrectionMatrix();
	}
	else post("load distortion coefficients from %s",filename->s_name);
	
	m_distortions = cvarrToMat(distortion_coeffs);

	t_atom dist_out[5];
	for ( int i = 0 ; i < 5 ; i++ ){
		SETFLOAT(&dist_out[i], CV_MAT_ELEM( *distortion_coeffs, float, i, 0));
	}		
	outlet_anything( m_dataout, gensym("distortion_coeffs"), 5, dist_out);
	 
}

void pix_opencv_patreco :: loadMess (t_symbol *filename)
{
	if ( filename->s_name[0] == 0 ) {
		error("no filename passed to loadIntra message");
		return;
	}
	if ( filename == NULL ) { error("%s is not a valid matrix", filename->s_name); return;}
	
	Mat img = imread(filename->s_name,0);
	
	if(img.cols!=img.rows){
		error("%s is not a square pattern", filename->s_name);
		return;
	}

	int msize = PAT_SIZE; 

	cv::Mat src(msize, msize, CV_8UC1);
	Point2f center((msize-1)/2.0f,(msize-1)/2.0f);
	Mat rot_mat(2,3,CV_32F);
	
	cv::resize(img, src, Size(msize,msize));
	Mat subImg = src(cv::Range(msize/4,3*msize/4), cv::Range(msize/4,3*msize/4));
	m_patternLibrary.push_back(subImg);

	rot_mat = getRotationMatrix2D( center, 90, 1.0);

	for (int i=1; i<4; i++){
		Mat dst= Mat(msize, msize, CV_8UC1);
		rot_mat = getRotationMatrix2D( center, -i*90, 1.0);
		cv::warpAffine( src, dst , rot_mat, Size(msize,msize));
		Mat subImg = dst(cv::Range(msize/4,3*msize/4), cv::Range(msize/4,3*msize/4)); // AV crop 25% on each side -> specific to AR tag ?
		m_patternLibrary.push_back(subImg);	
	}

	m_patternCount++;	
	t_atom data_out;
	SETFLOAT(&data_out, m_patternCount);
	outlet_anything( m_dataout, gensym("patternCount"), 1, &data_out);
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_patreco :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_patreco::loadIntraMessCallback,
  		  gensym("loadIntra"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_patreco::loadDistMessCallback,
  		  gensym("loadDist"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_patreco::loadMessCallback,
  		  gensym("load"), A_SYMBOL, A_NULL);
  		  
  // TODO add a clear method to empty the pattern library
  		    		  	  
}
void pix_opencv_patreco :: loadIntraMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->loadIntraMess(filename);
}
void pix_opencv_patreco :: loadDistMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->loadDistMess(filename);
}
void pix_opencv_patreco :: loadMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->loadMess(filename);
}
