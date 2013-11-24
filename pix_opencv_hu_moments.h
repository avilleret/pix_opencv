/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Hu moments calculator object

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) G�nther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_HU_MOMENTS_H_
#define INCLUDE_PIX_OPENCV_HU_MOMENTS_H_

#ifndef _EiC
#include "opencv2/legacy/legacy.hpp"
#endif

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_hu_moments
    
    Hu moments calculator object

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_hu_moments : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_hu_moments, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_hu_moments();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_hu_moments();

    	//////////
    	// Do the processing
    	virtual void 	processRGBAImage(imageStruct &image);
    	virtual void 	processRGBImage(imageStruct &image);
	virtual void 	processYUVImage(imageStruct &image);
    	virtual void 	processGrayImage(imageStruct &image); 
    	
	//////////
    	// Set the new edge threshold
    	void	    	floatBinaryMess(float binary);

	// to detect changes in the image size
	int 		comp_xsize;
	int		comp_ysize;

        int       x_binary;
        t_outlet  *m_dataout;
        t_atom    rlist[7];

    private:
    
    	//////////
    	// Static member functions
    	static void 	floatBinaryMessCallback(void *data, t_floatarg binary);

	/////////
	// IplImage needed
    	IplImage 	*rgba, *rgb, *gray;
        CvMoments x_moments;
        CvHuMoments x_humoments;
};

#endif	// for header file
