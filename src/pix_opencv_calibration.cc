#include "pix_opencv_calibration.h"
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>

using namespace cv;

CPPEXTERN_NEW(pix_opencv_calibration)

pix_opencv_calibration :: pix_opencv_calibration()
{ 
	m_dataout = outlet_new(this->x_obj, 0);
	
	success_count = 0;
	board_view_nb = 10;
	m_calibration = 0; 
	patternSize[0] = 6;
	patternSize[1] = 7;
	frame = 0;
	wait_n_frame = 10;
	findChessFlag = CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE + CALIB_CB_FAST_CHECK;
  
	// allocate storage matrix
	m_image_points			=	cv::Mat(patternSize[0]*patternSize[1]*board_view_nb, 2, CV_32FC1);
	m_object_points			=	cv::Mat(patternSize[0]*patternSize[1]*board_view_nb, 3, CV_32FC1);
	m_point_counts			=	cv::Mat(board_view_nb, 1, CV_32SC1);
	m_intrinsic_matrix 		= 	cv::Mat(3, 3, CV_32FC1);
	m_distortion_coeffs 	= 	cv::Mat(5, 1, CV_32FC1); 
	
	pix_opencv_calibration :: resetCorrectionMatrix();
}

pix_opencv_calibration :: ~pix_opencv_calibration()
{ 

}

// FIXME : merge all process*Image into one method
void pix_opencv_calibration :: processRGBAImage(imageStruct &image)
{ 
	if ((m_comp_xsize!=image.xsize)||(m_comp_ysize!=image.ysize)) { 
	
		m_comp_xsize = image.xsize;
		m_comp_ysize = image.ysize;
		if ( m_calibration ) error ( "image size changed, calibration was cancelled");
		m_calibration = 0;

		// used in findCorners
		find_rgb = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC4);
		find_gray = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC1);

		//create the images with new size
		rgb = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC4, image.data);
		tmp  = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC4);
		m_mapx = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_32FC1);
		m_mapy = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_32FC1);

		// create new map
		cv::initUndistortRectifyMap(m_intrinsic_matrix, m_distortion_coeffs, cv::Mat(), cv::Mat(), cv::Size(), CV_32FC1, m_mapx, m_mapy);
    }
    
	// this will loop until we got enought views (x->board_view_nb) with all corners visible
	if ( success_count < board_view_nb && m_calibration != 0 ) {
		findCorners( rgb );
	 }
	else if ( success_count >= board_view_nb && m_calibration != 0 ) {
		computeCalibration( rgb );
	 }
	else if ( m_calibration == 0 ) { 
		cv::remap(rgb,tmp,m_mapx,m_mapy, INTER_NEAREST);
		image.data = (unsigned char*) gray.data;
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
	if ((m_comp_xsize!=image.xsize)||(m_comp_ysize!=image.ysize)) { 
		
		m_comp_xsize = image.xsize;
		m_comp_ysize = image.ysize;
		if ( m_calibration ) { 
			error ( "image size changed, calibration was cancelled");
			m_calibration = 0;
			
			t_atom data_out;
			SETFLOAT(&data_out, m_calibration);
			outlet_anything( this->m_dataout, gensym("calibration"), 1, &data_out);
		}
		
		// used in findCorners
		find_rgb = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC4);
		find_gray = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC1);
		
		//create the images with new size
		gray = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC1, image.data);
		tmp = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_8UC1);
		m_mapx = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_32FC1);
		m_mapy = cv::Mat(cv::Size(m_comp_xsize,m_comp_ysize), CV_32FC1);

		// create new map
		cv::Size size;
		cv::initUndistortRectifyMap(m_intrinsic_matrix, m_distortion_coeffs, cv::Mat(), cv::Mat(), cv::Size(), CV_32FC1, m_mapx, m_mapy);
    }
    
	// this will loop until we got enought views (x->board_view_nb) with all corners visible
	if ( success_count < board_view_nb && m_calibration != 0 ) {
		findCorners( gray );
	 }
	else if ( success_count >= board_view_nb && m_calibration != 0 ) {
		computeCalibration( gray );
	 }
	else if ( m_calibration == 0 ) { 
		cv::remap(gray,tmp,m_mapx,m_mapy,INTER_NEAREST);
		image.data = (unsigned char*) tmp.data;
	}
}

/////////////////////////////////////////////////////////
// findCorners
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: findCorners ( cv::Mat& image )
{
	int					board_point_nb = this->patternSize[0]*this->patternSize[1];
	std::vector<cv::Point2f> corners;
	int					step;
	Size				patternSize, image_size;

	patternSize = cv::Size( this->patternSize[0], this->patternSize[1] );
	image_size = image.size();

	// find chessboard corners (gray or RGBA image...)
	int found = cv::findChessboardCorners(image, 
										patternSize, 
										corners,  
										findChessFlag);

	if (image.channels() == 4) {
		find_rgb = image.clone();
		cv::cvtColor( image , find_gray , COLOR_RGBA2GRAY); // convert color to gray
	} else {
		find_gray = image.clone();
	}
	
	// get subpixel accuracy on those corners (grayscale image only)
	cv::cornerSubPix(find_gray, 
					   corners, 
					   Size(11,11), 
					   Size(-1,-1), 
					   TermCriteria(TermCriteria::EPS+TermCriteria::MAX_ITER, 30, 0.1));


	// draw chessboard corner (color image only)
	if (image.channels() == 4) cv::drawChessboardCorners(find_rgb, patternSize, cv::Mat(corners), found);
	else 
	{
		cvtColor( find_gray , find_rgb , COLOR_RGBA2GRAY); // convert gray to color
		drawChessboardCorners(find_rgb, patternSize, cv::Mat(corners), found);
	}
	
	this->frame++;
	if ( this->frame % this->wait_n_frame == 0 ) { 
	// update arrays

		if( corners.size() == board_point_nb ) {
			step = this->success_count*board_point_nb;
			for( int i=step, j=0; j<board_point_nb; ++i,++j ) {
				m_image_points.at<float>(i,0) = corners[j].x; 
				m_image_points.at<float>(i,1) = corners[j].y; 
				m_object_points.at<float>(i,0) = j/this->patternSize[0]; 
				m_object_points.at<float>(i,1) = j%this->patternSize[0]; 
				m_object_points.at<float>(i,2) = 0.0f;
			}
			m_point_counts.at<int>(success_count,0) = board_point_nb; 
			success_count++;

			cv::bitwise_not( find_rgb , find_rgb );
		}
		
		t_atom data_out;
		SETFLOAT(&data_out, success_count);
		outlet_anything( this->m_dataout, gensym("take"), 1, &data_out);

	}

	// convert color to gray
	if (image.channels() == 1) {
		cvtColor( find_rgb , image, COLOR_RGBA2GRAY); // convert color to gray
	} else {
		image = find_rgb.clone();
	}	

}
/////////////////////////////////////////////////////////
// computeCalibration
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: computeCalibration ( cv::Mat& image )
{	
	cv::calibrateCamera(m_object_points,
					   m_image_points,
					   image.size(),
					   m_intrinsic_matrix,
					   m_distortion_coeffs,
					   cv::Mat(),
					   cv::Mat(),
					   cv::Mat(),
					   cv::Mat(), 
					   cv::Mat(),
					   0);
	m_mapx = cv::Mat( image.size(), CV_32FC1 );
	m_mapy = cv::Mat( image.size(), CV_32FC1 );

	cv::Size size;
	cv::initUndistortRectifyMap(m_intrinsic_matrix, m_distortion_coeffs, cv::Mat(), cv::Mat(), size, CV_32FC1, m_mapx, m_mapy);
	
	t_atom intra_out[9];
	for ( int i = 0 ; i < 9 ; i++ ){
		SETFLOAT(&intra_out[i], m_intrinsic_matrix.at<float>(i%3, i/3));
	}		
	outlet_anything( this->m_dataout, gensym("intrinsic_matrix"), 9, intra_out);
	
	t_atom dist_out[5];
	for ( int i = 0 ; i < 5 ; i++ ){
		SETFLOAT(&dist_out[i], m_distortion_coeffs.at<float>(i, 0));
	}		
	outlet_anything( this->m_dataout, gensym("distortion_coeffs"), 5, dist_out);
	
	m_calibration = 0;
	
	t_atom data_out;
	SETFLOAT(&data_out, m_calibration);
	outlet_anything( this->m_dataout, gensym("calibration"), 1, &data_out);
}

void pix_opencv_calibration :: loadMess (t_symbol *filename)
{
  if ( filename == NULL ) { error("NULL pointer passed to function loadDist"); return;}

  if ( filename->s_name[0] == 0 ) {
    error("no filename passed to loadDist message");
    return;
  }

  FileStorage fs(filename->s_name, FileStorage::READ);

  fs["camera_matrix"] >> m_intrinsic_matrix;
  fs["dist_coeff"] >> m_distortion_coeffs;
	
	if (m_distortion_coeffs.empty()) { 
		m_distortion_coeffs = 	cv::Mat(5, 1, CV_32FC1);
		error("can't open file %s", filename->s_name);
		resetCorrectionMatrix();
	}
	else if( m_distortion_coeffs.rows != 5 || m_distortion_coeffs.cols != 1 || CV_MAT_TYPE(m_distortion_coeffs.type()) != CV_32FC1 ) {
		error("%s is not a valid distortions coeffs file", filename->s_name);
		m_distortion_coeffs = cv::Mat(3, 3, CV_32FC1);
		resetCorrectionMatrix();
	}
	else post("load distortion coefficients from %s",filename->s_name);

	t_atom dist_out[5];
	for ( int i = 0 ; i < 5 ; i++ ){
		SETFLOAT(&dist_out[i], m_distortion_coeffs.at<float>(i, 0));
	}		
	outlet_anything( this->m_dataout, gensym("distortion_coeffs"), 5, dist_out);

	if (m_intrinsic_matrix.empty()) {
		m_intrinsic_matrix = 	cv::Mat(3, 3, CV_32FC1);
		error("can't open file %s", filename->s_name);
		resetCorrectionMatrix();
	}
	else if ( m_intrinsic_matrix.rows != 3 || m_intrinsic_matrix.cols != 3 || CV_MAT_TYPE(m_intrinsic_matrix.type()) != CV_32FC1 ) {
		error("%s is not a valid intrinsic matrix", filename->s_name);
		m_intrinsic_matrix = 	cv::Mat(3, 3, CV_32FC1);
		resetCorrectionMatrix();
	}
	else post("load transformation matrix from %s",filename->s_name);
	
	t_atom intra_out[9];
	for ( int i = 0 ; i < 9 ; i++ ){
		SETFLOAT(&intra_out[i], m_intrinsic_matrix.at<float>(i%3, i/3));
	}		
	outlet_anything( this->m_dataout, gensym("intrinsic_matrix"), 9, intra_out);
	
	// reinitialise size to force reinitialisation of mapx and mapy on next frame
	m_comp_xsize = 0;  
}

void pix_opencv_calibration :: writeMess (t_symbol *filename)
{
	if(!filename)
	{
		error("NULL symbol pointer");
		return;
	}
	FileStorage fs(filename->s_name, FileStorage::WRITE);

	fs << "camera_matrix" << m_intrinsic_matrix;
	fs << "dist_coeff" << m_distortion_coeffs;
}

void pix_opencv_calibration :: floatCalibrationhMess (float calib_flag) 
{
	m_calibration=calib_flag;
	if ( m_calibration == 1 ) {
		this->success_count = 0;
		this->frame = 0;
	}
	t_atom data_out;
	SETFLOAT(&data_out, m_calibration);
	outlet_anything( this->m_dataout, gensym("calibration"), 1, &data_out);
}

void pix_opencv_calibration :: patternSizeMess (float xsize, float ysize)
{
	if (m_calibration) {error("you can't change pattern size during calibration"); return;}
	if ( xsize < 3 || ysize < 3 ) {error("patternSize should be at least 3x3"); return;}
	this->patternSize[0]=xsize;
	this->patternSize[1]=ysize;
	
	// reallocate matrix
	m_image_points	=	cv::Mat(patternSize[0]*patternSize[1]*board_view_nb, 2, CV_32FC1);
	m_object_points	=	cv::Mat(patternSize[0]*patternSize[1]*board_view_nb, 3, CV_32FC1);
	m_point_counts	=	cv::Mat(board_view_nb, 1, CV_32SC1);	
}

void pix_opencv_calibration :: viewMess (int view)
{     
	if ( m_calibration == 1 ) {error("you can't change view number during calibration !"); return;}
	board_view_nb=view<2?2:view;
	if (view < 2) error("view should be greater or equal to 2");

	// reallocate matrix
	m_image_points = cv::Mat(patternSize[0]*patternSize[1]*board_view_nb, 2, CV_32FC1);
	m_object_points  = cv::Mat(patternSize[0]*patternSize[1]*board_view_nb, 3, CV_32FC1);
	m_point_counts   = cv::Mat(board_view_nb, 1, CV_32SC1);
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
	findChessFlag = CALIB_CB_ADAPTIVE_THRESH * adaptThres + CALIB_CB_NORMALIZE_IMAGE * normalize + CALIB_CB_FILTER_QUADS * filter;
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_calibration :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::loadMessCallback,
  		  gensym("load"), A_SYMBOL, A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_calibration::writeMessCallback,
  		  gensym("write"), A_SYMBOL, A_NULL);
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
    class_addmethod(classPtr, (t_method)&pix_opencv_calibration::resetMessCallback,
  		  gensym("reset"), A_NULL);    
  		    		  	  
}
void pix_opencv_calibration :: loadMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->loadMess(filename);
}
void pix_opencv_calibration :: writeMessCallback(void *data, t_symbol* filename)
{
	    GetMyClass(data)->writeMess(filename);
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

void pix_opencv_calibration :: resetMessCallback(void *data)
{
		GetMyClass(data)->resetCorrectionMatrix();
}

void pix_opencv_calibration ::  resetCorrectionMatrix()
{
	m_intrinsic_matrix.zeros(3,3,CV_32FC1);
	m_distortion_coeffs.zeros(3,3,CV_32FC1);
	
	// reinitialise size to force reinitialisation of mapx and mapy on next frame
	m_comp_xsize = 0;
}
