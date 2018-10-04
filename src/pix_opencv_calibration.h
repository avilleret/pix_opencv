/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Threshold filter

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) G�nther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_pix_opencv_calibration_H_
#define INCLUDE_pix_opencv_calibration_H_

#ifndef _EiC
#include "opencv2/core/core_c.h"
#include "opencv2/imgproc/imgproc_c.h"
#endif

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_calibration
    
    Threshold filter

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_calibration : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_calibration, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_calibration();
    	
    protected:
    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_calibration();

   	//////////
   	// Do the processing
   	virtual void 	processRGBAImage(imageStruct &image);
   	virtual void 	processRGBImage(imageStruct &image);
	virtual void 	processYUVImage(imageStruct &image);
   	virtual void 	processGrayImage(imageStruct &image);
   	
   	void findCorners ( IplImage *image );
    void computeCalibration ( IplImage *image );
   	    	
	//////////
   	// Set the new edge threshold
	void 	loadIntraMess(t_symbol *filename);
	void 	loadDistMess(t_symbol *filename);
	void 	writeIntraMess(t_symbol *filename);
	void 	writeDistMess(t_symbol *filename);
	void 	floatCalibrationhMess (float calib_flag);
	void 	patternSizeMess (float xsize, float ysize);
	void 	viewMess (int view);
	void 	waitMess (int wait);
	void 	findChessFlagMess(int adaptThres, int normalize, int filter);
	void 	resetCorrectionMatrix();
	
	int	success_count, /* number of images on wich we correctly found corners */
		board_view_nb, /* number of views to take */
		calibration, /* calibration flag */
		patternSize[2], /* size of the calibration chessboard */
		frame, /* number of frames analysed for chessboard corner */
		wait_n_frame, /* number of frames to wait between two take */
		findChessFlag; // flag for cvFindChessboardCorners


    private:
    
    t_outlet *m_dataout;
    
   	//////////
   	// Static member functions
	static void	loadIntraMessCallback(void *data, t_symbol* filename);
	static void	loadDistMessCallback(void *data, t_symbol* filename);
	static void	writeIntraMessCallback(void *data, t_symbol* filename);
	static void	writeDistMessCallback(void *data, t_symbol* filename);
	static void floatCalibrationMessCallback(void *data, t_floatarg calib_flag);
   	static void patternSizeMessCallback(void *data, t_floatarg xsize, t_floatarg ysize);
   	static void viewMessCallback(void *data, t_floatarg view);
    static void waitMessCallback(void *data, t_floatarg wait); 
    static void findChessFlagMessCallback(void *data, t_floatarg adaptThres, t_floatarg normalize, t_floatarg filter);
    static void resetMessCallback(void *data);
	/////////

	// CvMat needed
	cv::Mat	m_image_points;
  cv::Mat m_object_points;
	cv::Mat m_point_counts;
	cv::Mat m_intrinsic_matrix;
	cv::Mat m_distortion_coeffs;

  cv::Mat find_rgb;
  cv::Mat find_gray;
  cv::Mat rgb;
  cv::Mat gray;
  cv::Mat tmp;
  cv::Mat m_mapx;
  cv::Mat m_mapy;

  // to detect changes in the image size
  int   m_comp_xsize;
  int   m_comp_ysize;
};

#endif	// for header file
