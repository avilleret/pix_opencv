#pragma once
#include <opencv2/imgproc.hpp>
#include <Base/GemPixObj.h>

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
  
  double m_epsilon;


  int m_enable_contours, m_enable_hulls, m_enable_defects, m_hierarchy_level, m_mode, m_taboutput, m_enable_cvblob, m_areaThreshold;
  int m_totalPointsCount, m_autoresize;
  
  t_symbol *m_x_arrayname, *m_y_arrayname, *m_z_arrayname;
	    
};
