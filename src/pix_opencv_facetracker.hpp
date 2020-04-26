#pragma once
#include <opencv2/opencv.hpp>
#include <FaceTracker/Tracker.h>
#include <Base/GemPixObj.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
    pix_opencv_facetracker
    
  FaceTracker implementation
  * see https://github.com/kylemcdonald/FaceTracker
	
KEYWORDS
    pix
    
DESCRIPTION
   
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_facetracker : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_facetracker, GemPixObj)

  public:

    //////////
    // Constructor
    pix_opencv_facetracker(int,t_atom*);
    
  protected:
    
    //////////
    // Destructor
    virtual ~pix_opencv_facetracker();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);
    
    void resetMess(void);
    void showMess(int);
    void tableMess(t_symbol*xarray, t_symbol*yarray);
    void clampMess(float arg);
    void toleranceMess(float arg);
    void filterMess(float arg);

  private:
    
    enum Feature {
        LEFT_EYEBROW, RIGHT_EYEBROW,
        LEFT_EYE, RIGHT_EYE,
        LEFT_JAW, RIGHT_JAW, JAW,
        OUTER_MOUTH, INNER_MOUTH,
        NOSE_BRIDGE, NOSE_BASE,
        FACE_OUTLINE, ALL_FEATURES
    };
    
    void Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi);
    void OutputMesh(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi);
    std::vector<int> getFeatureIndices(int feature);
  
    bool m_fcheck; 
    double m_scale; 
    int m_fpd; 
    bool m_show;
    int m_nIter; 
    double m_clamp, m_fTol; 
    bool m_failed;
    int m_fps;
    int64 m_t1,m_t0;
    int m_fnum;
    bool m_taboutput;  // enable table output
    bool m_autoresize; // flag to resize table according to mesh size

    //set other tracking parameters
    std::vector<int> m_wSize1;
    std::vector<int> m_wSize2;
    
    t_outlet *m_dataout; // info outlet
    FACETRACKER::Tracker m_tracker;
    cv::Mat m_tri, m_con;
    
    t_symbol* m_arraysname[13];
             
};
