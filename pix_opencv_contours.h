/*-----------------------------------------------------------------
LOG
    GEM - Graphics Environment for Multimedia

    Copyright (c) 1997-1999 Mark Danks. mark@danks.org
    Copyright (c) Günther Geiger. geiger@epy.co.at
    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
    Copyright (c) 2002 James Tittle & Chris Clepper
    For information on usage and redistribution, and for a DISCLAIMER OF ALL
    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_CONTOURS_H_
#define INCLUDE_PIX_OPENCV_CONTOURS_H_

#ifndef _EiC
#include "opencv2/legacy/legacy.hpp"
#endif

#include "Base/GemPixObj.h"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_contours
    
	detects contours and send them out
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_contours : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_contours, GemPixObj)

public:

  //////////
  // Constructor
  pix_opencv_contours();
    
protected:
    
  //////////
  // Destructor
  virtual ~pix_opencv_contours();

  //////////
  // Do the processing
  virtual void 	processImage(imageStruct &image);

  // Messages handling
	void epsilonMess(double arg);
	void areaMess(double arg);
	void contoursMess(double arg);
  void cvblobMess(double arg);
	void convexhullsMess(double arg);
	void convexitydefectsMess(double arg);
	void hierarchyMess(int arg);
	void taboutputMess(float arg);
	void tableMess(t_symbol*x, t_symbol*y, t_symbol*z);
  void repeat_pointMess(float arg);

private:
  
  void outputCount();
  void outputBlobs(imageStruct &image);
  void outputContours(imageStruct &image);
  t_outlet *m_dataout_middle; // contour outlet
  t_outlet *m_dataout_right; // info outlet
  std::vector<std::vector<cv::Point> > m_contours;
  std::vector<std::vector<int> > m_convexhulls;
  std::vector<int> m_area;
  
  unsigned int m_repeat_point;
  double m_epsilon;


  int m_enable_contours, m_enable_hulls, m_enable_defects, m_hierarchy_level, m_mode, m_taboutput, m_enable_cvblob, m_areaThreshold;
  int m_totalPointsCount, m_autoresize;
  
  t_symbol *m_x_arrayname, *m_y_arrayname, *m_z_arrayname;
	    
};
#endif	// for header file
