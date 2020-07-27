#pragma once
#include "opencv2/opencv.hpp"
#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_warpperspective

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_warpperspective : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_warpperspective, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_warpperspective();

  protected:

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    /////////
    // Setup
    void 	mapMatrixMess (int argc, t_atom *argv);
    void	srcMatrixMess (int argc, t_atom *argv);
    void	dstMatrixMess (int argc, t_atom *argv);
    void    invertMess    (int argc, t_atom *argv);

    void	findhomography();

    // to detect changes in the image size
    int 	comp_xsize;
    int		comp_ysize;

    cv::Mat mapMatrix; // 3x3 transformation matrix
    cv::Mat srcMatrix;
    cv::Mat dstMatrix; // nX2 points coordinates matrices, n>4

    std::array<t_atom,9> mapMatrixList; // array to send out transformation Matrix
    cv::InterpolationFlags flags;
    int findmethod;

  private:
    
    t_outlet 	*m_dataout;
    //////////
    // Static member functions
    static void  mapMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
    static void  srcMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
    static void  dstMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
    static void  invertMessCallback(void *data, t_symbol *s, int argc, t_atom *argv);
};
