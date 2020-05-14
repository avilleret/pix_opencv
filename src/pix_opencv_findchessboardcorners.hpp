#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

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
class GEM_EXPORT pix_opencv_findchessboardcorners : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_findchessboardcorners, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_findchessboardcorners();

  protected:

    void process();

    //////////
    // Destructor
    virtual ~pix_opencv_findchessboardcorners();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    /////////
    // Setup
    void 	patternSizeMess (int xsize, int ysize);

    // to detect changes in the image size
    int 	comp_xsize;
    int		comp_ysize;

    cv::Size pattern_size; // pattern size (inner corners count in 2D)
    std::vector<cv::Point2f> corners; // array to store corners coordinates
    int cornerCount; // number of corners found
    cv::Size win; // half of the search window size for cvFindCornerSubPix
    cv::Size zero_zone; // for cvFindCornerSubPix
    t_atom *coord_list;

  private:
    t_outlet *m_dataout;
    //////////
    // Static member functions
    static void patternSizeMessCallback(void *data, t_floatarg xsize, t_floatarg ysize);

    cv::Mat gray;
    bool patternfound;
};
