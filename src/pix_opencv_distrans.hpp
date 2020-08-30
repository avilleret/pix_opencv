#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_distrans
    
    Distrans algorithm

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_distrans : public GemPixObj
{
  CPPEXTERN_HEADER(pix_opencv_distrans, GemPixObj)

public:

  //////////
  // Constructor
  pix_opencv_distrans();

protected:

  //////////
  // Destructor
  virtual ~pix_opencv_distrans();

  //////////
  // Do the processing
  virtual void 	processImage(imageStruct &image);

  // Some varibales to control mophology voronoi
  int 		edge_thresh;
  int 		mask_size;
  int     build_voronoi;
  int     voronoiType = cv::DIST_LABEL_CCOMP;
  int     dist_type;

private:

  //////////
  // Static member functions
  static void 	thresholdMessCallback(void *data, t_floatarg pos);
  static void 	voronoiMessCallback(void *data, t_floatarg voronoi);
  static void 	maskMessCallback(void *data, t_floatarg f);
  static void 	distMessCallback(void *data, t_floatarg f);
  static void   typeMessCallback(void *data, t_floatarg f);
};
