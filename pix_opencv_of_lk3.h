/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Optical Flow block Matching algorithm

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_pix_opencv_of_lk3_H_
#define INCLUDE_pix_opencv_of_lk3_H_

#ifndef _EiC
#include "cv.h"
#endif

#include "Base/GemPixObj.h"
#include "RTE/MessageCallbacks.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_of_lk3
    
    Optical Flow block Matching algorithm

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_of_lk3 : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_of_lk3, GemPixObj)

    public:

	//////////
	// Constructor
    	pix_opencv_of_lk3();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_of_lk3();

    	//////////
    	// Do the processing
    	virtual void 	processRGBAImage(imageStruct &image);
    	virtual void 	processRGBImage(imageStruct &image);
		virtual void 	processYUVImage(imageStruct &image);
    	virtual void 	processGrayImage(imageStruct &image); 

        void winSizeMess(float width, float height);

        int m_xsize, m_ysize;

        CvSize m_winsize	;
		
		t_symbol *velxArray, *velyArray;
		t_word *velxVec, *velyVec;
		int velxVecSize, velyVecSize;

    private:
       
    	//////////
    	// Static member functions
        static void  winSizeMessCallback(void *data, float width, float height);        

	// Internal Open CV data
        IplImage *grey, *prev_grey;
        CvMat *x_velx, *x_vely;

};

#endif	// for header file
