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

#ifndef INCLUDE_pix_opencv_patreco_H_
#define INCLUDE_pix_opencv_patreco_H_

#ifndef _EiC
#include "cv.h"
#endif

// ARma lib
#include "pattern.hpp"
#include "patterndetector.hpp"

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_patreco
    
	square pattern detector
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_patreco : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_patreco, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_patreco();
    	
    protected:
    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_patreco();

   	//////////
   	// Do the processing
   	virtual void 	processRGBAImage(imageStruct &image);
   	virtual void 	processRGBImage(imageStruct &image);
	virtual void 	processYUVImage(imageStruct &image);
   	virtual void 	processGrayImage(imageStruct &image);
   	   	    	
	//////////
   	// Set parameters
	void 	loadIntraMess(t_symbol *filename);
	void 	loadDistMess(t_symbol *filename);
	void 	loadMess(t_symbol *filename);
	void 	fixedThreshMess(float arg);
	void 	adaptThreshMess(float arg);
	void 	adaptBlockSizeMess(float arg);
	void 	threshModeMess(float arg);
	void 	patternSizeMess(float arg);
	void 	monitorStageMess(t_float arg);
	

    private:
    
    t_outlet *m_dataout;
    
   	//////////
   	// Static member functions
	static void	loadIntraMessCallback(void *data, t_symbol* filename);
	static void	loadDistMessCallback(void *data, t_symbol* filename);
	static void	loadMessCallback(void *data, t_symbol* filename);
	static void	fixedThreshMessCallback(void *data, t_floatarg arg);
	static void	adaptThreshMessCallback(void *data, t_floatarg arg);
	static void	adaptBlockSizeMessCallback(void *data, t_floatarg arg);
	static void	threshModeMessCallback(void *data, t_floatarg arg);
	static void	patternSizeMessCallback(void *data, t_floatarg arg);
	static void	monitorStageMessCallback(void *data, t_floatarg arg);
	
	cv::Mat m_cameraMatrix, m_distortions;
    
    ARma::PatternDetector *m_detector;
    std::vector<cv::Mat> m_patternLibrary;  			// pattern library
	std::vector<ARma::Pattern> m_detectedPattern; 		// detected pattern 
	int m_patternCount;									// patternCount
	//~ float 	m_fixed_threshold;							// fixed threshold
	//~ float	m_adapt_threshold;							// adaptive threshold
	//~ int		m_adapt_block_size;							// block size for adaptive threshold
	//~ int 	m_threshold_mode;							// 0 : no thresholding (input image should be already binarised, 1 : fixed threshold method, 2 : adaptive threshold method
	//~ int 	m_pattern_size;								// size of pattern for recognition ( 64 is good, smaller is faster...)
	//~ int 	m_monitor_stage;							// monitored stage, 1 : binarisation, 2 : normalised ROI...
};
#endif	// for header file
