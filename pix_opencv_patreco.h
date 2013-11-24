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
	void 	loadMess(t_symbol *s, int argc, t_atom* argv);
	void 	fixedThreshMess(float arg);
	void 	adaptThreshMess(float arg);
	void 	adaptBlockSizeMess(float arg);
	void 	threshModeMess(float arg);
	void 	patternSizeMess(float arg);
	void 	monitorStageMess(t_float arg);
	void 	ARTpatternMess(t_float arg);
	void 	dilateMess(t_float arg);
	void 	erodeMess(t_float arg);
	void 	clearLibMess(void);

    private:
    
    t_outlet *m_dataout;
	
	cv::Mat m_cameraMatrix, m_distortions;
    
    ARma::PatternDetector *m_detector;
    //~ std::vector<cv::Mat> m_patternLibrary;  				// pattern library
    std::map<int, PatternLib> m_patternLibrary;
	std::vector<ARma::Pattern> m_detectedPattern; 			// detected pattern 
	int m_pattern_size;
	
};
#endif	// for header file
