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
// pix_opencv_warpperspective integration by Antoine Villeret - 2011

#ifndef INCLUDE_PIX_OPENCV_WARPPERSPECTIVE_H_
#define INCLUDE_PIX_OPENCV_WARPPERSPECTIVE_H_

#ifndef _EiC
#include "cv.h"
#endif

#include "Base/GemPixObj.h"

#define MAX_MARKERS 500

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_warpperspective
   
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXTERN pix_opencv_warpperspective : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_warpperspective, GemPixObj)

    public:

	    //////////
	    // Constructor
    	pix_opencv_warpperspective();
    	
    protected:
    	
    	//////////
    	// Destructor
    	virtual ~pix_opencv_warpperspective();

		//////////
		// Do the processing
		virtual void 	processRGBAImage(imageStruct &image);
		virtual void 	processRGBImage(imageStruct &image);
		virtual void 	processYUVImage(imageStruct &image);
		virtual void 	processGrayImage(imageStruct &image);
		
		/////////
		// Setup
		void 	mapMatrixMess (int argc, t_atom *argv);
		void	srcMatrixMess (int argc, t_atom *argv);
		void	dstMatrixMess (int argc, t_atom *argv);
		void	findhomography();

		// to detect changes in the image size
		int 	comp_xsize;
		int		comp_ysize;

        CvMat *mapMatrix; // 3x3 transformation matrix
		CvMat *srcMatrix, *dstMatrix; // nX2 points coordinates matrices, n>4
		t_atom *mapMatrixList; // array to send out transformation Matrix
        int flags;
        int findmethod;


    private:
    
		t_outlet 	*m_dataout;
    	//////////
    	// Static member functions
		static void  mapMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
		static void  srcMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
		static void  dstMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);

		/////////
		// IplImage needed
    	IplImage 	*rgb, *tmp, *gray;
	
};

#endif	// for header file
