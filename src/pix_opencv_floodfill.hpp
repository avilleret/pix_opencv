#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "Base/GemPixObj.h"

#define MAX_COMPONENTS 10

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_floodfill
    
    Color blob tracker

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_floodfill : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_floodfill, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_floodfill();

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_floodfill();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    void  colorMess(float color);
    void  fillcolorMess(float index, float r, float g, float b);
    void  connectivityMess(float connectivity);
    void  markMess(float px, float py);
    void  deleteMess(float index);
    void  clearMess(void);
    void  updiffMess(float updiff);
    void  lodiffMess(float lodiff);

    int comp_xsize;
    int comp_ysize;

    t_outlet *m_dataout;
    t_atom x_list[5];

    int x_up;
    int x_lo;
    int x_connectivity;
    int x_color;


  private:
    
    //////////
    // Static member functions
    static void  colorMessCallback(void *data, float color);
    static void  fillcolorMessCallback(void *data, float index, float r, float g, float b);
    static void  connectivityMessCallback(void *data, float connectivity);
    static void  markMessCallback(void *data, float px, float py);
    static void  deleteMessCallback(void *data, float index);
    static void  clearMessCallback(void *data);
    static void  updiffMessCallback(void *data, float updiff);
    static void  lodiffMessCallback(void *data, float lodiff);

    // Internal Open CV data
    // tracked components
    int x_xcomp[MAX_COMPONENTS];
    int x_ycomp[MAX_COMPONENTS];

    // fill color
    int x_r[MAX_COMPONENTS];
    int x_g[MAX_COMPONENTS];
    int x_b[MAX_COMPONENTS];
};
