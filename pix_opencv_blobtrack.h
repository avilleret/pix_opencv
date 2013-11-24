/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_BLOBTRACK_H_
#define INCLUDE_PIX_OPENCV_BLOBTRACK_H_

#ifndef _EiC
#include "opencv2/video/background_segm.hpp"
#include "opencv2/legacy/blobtrack.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc_c.h>
#endif

#include "blobtrack.h"

#include "Base/GemPixObj.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>
/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
  pix_opencv_blobtrack
    
	advanced blob tracker
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_blobtrack : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_blobtrack, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_blobtrack();
    	
    protected:
    	
   	//////////
   	// Destructor
   	virtual ~pix_opencv_blobtrack();

   	//////////
   	// Do the processing
   	virtual void 	processRGBAImage(imageStruct &image);
   	virtual void 	processRGBImage(imageStruct &image);
	virtual void 	processYUVImage(imageStruct &image);
   	virtual void 	processGrayImage(imageStruct &image);
   	void RunBlobTrackingAuto( IplImage* img );
   		
   	//////////
   	// Messages handling
   	void 	monitorStageMess(int arg);
    void areaThresholdMess(t_float arg);
    void 	fgTrainFramesMess(int arg);
	void 	printParamsMess(void);
	void 	getParamMess(t_symbol*s, int argc, t_atom*argv); // get available params or param value
	void 	setParamMess(t_symbol*s, int argc, t_atom*argv); // set param
	void 	getModuleMess(t_symbol*s, int argc, t_atom*argv); // list available and currently used modules
	void 	setModuleMess(t_symbol*s, int argc, t_atom*argv); // set selected modules

   	// Members
   	std::string m_fg_name, m_bd_name, m_bt_name, m_btgen_name, m_btpp_name, m_bta_name, m_bt_corr;
   	int m_FGTrainFrames;
   	int m_monitoring_stage; // 0 : input image, 1 : FG 3 : input with trackng info
    t_float m_areaThreshold;
   	int x_size, y_size;

   	
   	CvBlobTrackerAuto* m_tracker;
   	CvBlobTrackerAutoParam1 m_param;
   	
	DefModule_FGDetector*			m_FGModule;
    DefModule_BlobDetector*         m_BDModule;
    DefModule_BlobTracker*          m_BTModule;
    DefModule_BlobTrackPostProc*    m_BTPostProcModule;
    DefModule_BlobTrackGen*         m_BTGenModule;
    DefModule_BlobTrackAnalysis*    m_BTAnalysisModule;
    
	
    private:
    
    void setupModules();
	void createModules();
	void releaseModules(void);
	void print_params(CvVSModule* pM, const char* module); 
    t_outlet *m_dataout; // info outlet
	    
};
#endif	// for header file
