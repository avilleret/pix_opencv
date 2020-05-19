#pragma once
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_hu_moments
    
    Hu moments calculator object

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_hu_moments : public GemPixObj
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
    virtual void 	processImage(imageStruct &image);

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
};
