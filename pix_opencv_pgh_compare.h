/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    PGH histogram comparison used to compare contours

    Copyright (c) 1997-1998 Mark Danks. mark@danks.org
    Copyright (c) G¸nther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f¸r::uml‰ute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_PGH_COMPARE_H_
#define INCLUDE_PIX_OPENCV_PGH_COMPARE_H_

#ifndef _EiC
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/legacy/compat.hpp"

#endif

#include "Base/GemPixDualObj.h"

/*-----------------------------------------------------------------
CLASS
    pix_opencv_pgh_compare
    
    PGH histogram comparison used to compare contours

-----------------------------------------------------------------*/

class GEM_EXTERN pix_opencv_pgh_compare : public GemPixDualObj
{
    CPPEXTERN_HEADER(pix_opencv_pgh_compare, GemPixDualObj)

    public:

	    //////////
    	// Constructor
    	pix_opencv_pgh_compare(int,t_atom*);
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_pgh_compare();

    	//////////
    	// Do the processing
    	virtual void 	processRGBA_RGBA(imageStruct &left, imageStruct &right);
    	virtual void 	processRGB_RGB(imageStruct &left, imageStruct &right);
    	virtual void 	processYUV_YUV(imageStruct &left, imageStruct &right);
      	virtual void 	processGray_Gray(imageStruct &left, imageStruct &right);
        
    	//////////
    	// change method used
        void            floatMinSizeMess(float minsize);
        void            clearMess(void);
        void            floatCriteriaMess(float criteria);

        int comp_xsize;
        int comp_ysize;

        t_outlet *m_dataout;
        t_outlet *m_posout;

        int x_minsize;
        float x_cdistance;

    private:
    
    	//////////
    	// Static member functions
        static void            floatMinSizeMessCallback(void *data, float minsize);
        static void            clearMessCallback(void *data);
        static void            floatCriteriaMessCallback(void *data, float criteria);

        IplImage *rgba, *rgb, *gray;
        IplImage *rgbar, *rgbr, *grayr;

        CvMemStorage *x_storage;
        CvSeq        *x_bcontourr;

        t_atom       rlist[5];

};

#endif	// for header file
