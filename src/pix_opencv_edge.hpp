#pragma once
#include <opencv2/core.hpp>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_edge
    
    Edge detection

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_edge : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_edge, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_edge();

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_edge();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    //////////
    // Set the new edge threshold
    void floatThreshMess(float edge_thresh);
    // The new edge threshold
    int edge_thresh;

  private:
    
    //////////
    // Static member functions
    static void 	floatTreshMessCallback(void *data, t_floatarg edge_thresh);
};
