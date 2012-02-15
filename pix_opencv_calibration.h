/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Threshold filter

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_pix_opencv_calibration_H_
#define INCLUDE_pix_opencv_calibration_H_

#ifndef _EiC
#include "cv.h"
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
class GEM_EXTERN pix_opencv_calibration : public GemPixObj
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
	
	// to detect changes in the image size
	int 	comp_xsize;
	int		comp_ysize;
	
	int	success_count, /* number of images on wich we correctly found corners */
		board_view_nb, /* number of views to take */
		calibration, /* calibration flag */
		patternSize[2], /* size of the calibration chessboard */
		frame, /* number of frames analysed for chessboard corner */
		wait_n_frame, /* number of frames to wait between two take */
		findChessFlag; // flag for cvFindChessboardCorners


    private:
    
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
	/////////
	// CvMat needed
	CvMat 	*image_points, 
			*object_points, 
			*point_counts, 
			*intrinsic_matrix, 
			*distortion_coeffs;

	/////////
	// IplImage needed
    IplImage 	*find_rgb, *find_gray, *rgb, *gray, *tmp, *mapx, *mapy;
};

#endif	// for header file
