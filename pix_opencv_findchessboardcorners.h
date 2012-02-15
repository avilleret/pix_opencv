/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Contours Bounding Rectangle detection

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/
// findchessboardcorners integration by Antoine Villeret - 2011

#ifndef INCLUDE_PIX_OPENCV_FINDCHESSBOARDCORNERS_H_
#define INCLUDE_PIX_OPENCV_FINDCHESSBOARDCORNERS_H_

#ifndef _EiC
#include "cv.h"
#endif

#include "Base/GemPixObj.h"

#define MAX_MARKERS 500

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_findchessboardcorners
    
    Chessboard corners detection

KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_findchessboardcorners : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_findchessboardcorners, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_findchessboardcorners();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_findchessboardcorners();

		//////////
		// Do the processing
		virtual void 	processRGBAImage(imageStruct &image);
		virtual void 	processRGBImage(imageStruct &image);
		virtual void 	processYUVImage(imageStruct &image);
		virtual void 	processGrayImage(imageStruct &image);
		
		/////////
		// Setup
		void 	patternSizeMess (int xsize, int ysize);


		// to detect changes in the image size
		int 	comp_xsize;
		int		comp_ysize;

        
        CvSize pattern_size; // pattern size (inner corners count in 2D)
        CvPoint2D32f *corners; // array to store corners coordinates
        int cornerCount; // number of corners found
        int flags; // flags for cvFindChessboardCorners
        CvSize win; // half of the search window size for cvFindCornerSubPix
        CvSize zero_zone; // for cvFindCornerSubPix
        CvTermCriteria criteria; // for cvFindCornerSubPix
        t_atom *coord_list;


    private:
    
		t_outlet 	*m_dataout;
    	//////////
    	// Static member functions
		static void patternSizeMessCallback(void *data, t_floatarg xsize, t_floatarg ysize);


		/////////
		// IplImage needed
    	IplImage 	*rgb, *tmp_color, *gray, *tmp_gray;
	
};

#endif	// for header file
