#pragma once

#include <opencv2/core.hpp>

#include "Base/GemPixDualObj.h"

/*-----------------------------------------------------------------
CLASS
    pix_opencv_pgh_compare
    
    PGH histogram comparison used to compare contours

-----------------------------------------------------------------*/

class GEM_EXPORT pix_opencv_pgh_compare : public GemPixDualObj
{
    CPPEXTERN_HEADER(pix_opencv_pgh_compare, GemPixDualObj)

    public:

	    //////////
    	// Constructor
    	pix_opencv_pgh_compare(int,t_atom*);
    	
    protected:
    	
    	//////////
    	// Do the processing
      void 	processDualImage(imageStruct &left, imageStruct &right) override;

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

        std::vector<cv::Point> x_bcontourr;

    private:
    
    	//////////
    	// Static member functions
        static void            floatMinSizeMessCallback(void *data, float minsize);
        static void            clearMessCallback(void *data);
        static void            floatCriteriaMessCallback(void *data, float criteria);

        t_atom       rlist[5];
};
