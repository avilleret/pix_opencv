#pragma once

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_calibration

    camera calibration and correction
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
   	
   	void findCorners ( cv::Mat& image );
    void computeCalibration ( cv::Mat& image );
   	    	
	//////////
   	// Set the new edge threshold
	void 	loadMess(t_symbol *filename);
	void 	writeMess(t_symbol *filename);
	void 	floatCalibrationhMess (float calib_flag);
	void 	patternSizeMess (float xsize, float ysize);
	void 	viewMess (int view);
	void 	waitMess (int wait);
	void 	findChessFlagMess(int adaptThres, int normalize, int filter);
	void 	resetCorrectionMatrix();
	
  int	success_count; /* number of images on wich we correctly found corners */
  int board_view_nb; /* number of views to take */
  int	m_calibration; /* calibration flag */
  int	patternSize[2]; /* size of the calibration chessboard */
  int	frame; /* number of frames analysed for chessboard corner */
  int	wait_n_frame; /* number of frames to wait between two take */
  int	findChessFlag; // flag for cvFindChessboardCorners

    private:
    
    t_outlet *m_dataout;
    
   	//////////
   	// Static member functions
	static void	loadMessCallback(void *data, t_symbol* filename);
	static void	writeMessCallback(void *data, t_symbol* filename);
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
