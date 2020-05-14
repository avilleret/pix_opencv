#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <Base/GemPixDualObj.h>

/*-----------------------------------------------------------------
CLASS
    pix_opencv_hu_compare
    
    Hu moments comparison used to compare contours

-----------------------------------------------------------------*/

class GEM_EXPORT pix_opencv_hu_compare : public GemPixDualObj
{
    CPPEXTERN_HEADER(pix_opencv_hu_compare, GemPixDualObj)

    public:

      //////////
      // Constructor
      pix_opencv_hu_compare(int,t_atom*);

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_hu_compare();

    //////////
    // Do the processing
    virtual void 	processDualImage(imageStruct &left, imageStruct &right);

    //////////
    // change method used
    void            floatMethodMess(float method);
    void            floatMinSizeMess(float minsize);
    void            clearMess(void);
    void            floatCriteriaMess(float criteria);

    int comp_xsize;
    int comp_ysize;

    t_outlet *m_dataout;
    t_outlet *m_posout;

    int x_method;
    int x_minsize;
    float x_cdistance;

  private:
    
    //////////
    // Static member functions
    static void            floatMethodMessCallback(void *data, float method);
    static void            floatMinSizeMessCallback(void *data, float minsize);
    static void            clearMessCallback(void *data);
    static void            floatCriteriaMessCallback(void *data, float criteria);

    std::vector<cv::Point> x_bcontourr;

    t_atom       rlist[5];
};
