#pragma once
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>
#include <Base/GemPixObj.h>

#include <string>
#include <stdio.h>

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS

   pix_opencv_knear : OCR like pattern recognition
   based on basic OCR with Open CV tutorial
   by damiles : http://blog.damiles.com/?p=93

KEYWORDS
    pix
    
DESCRIPTION

-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_knear : public GemPixObj
{
    CPPEXTERN_HEADER(pix_opencv_knear, GemPixObj)

    public:

      //////////
      // Constructor
      pix_opencv_knear(t_symbol *path, t_floatarg nsamples);

  protected:

    //////////
    // Destructor
    virtual ~pix_opencv_knear();

    //////////
    // Do the processing
    virtual void 	processImage(imageStruct &image);

    void findX(const cv::Mat& imgSrc,int& min, int& max);
    void findY(const cv::Mat& imgSrc,int& min, int& max);
    cv::Rect findBB(const cv::Mat& imgSrc);
    cv::Mat preprocessing(const cv::Mat& imgSrc,int new_width, int new_height);
    void load_patterns(void);

    // to detect changes in the image size
    int 		comp_xsize;
    int		comp_ysize;
    

  private:
    
    //////////
    // Static member functions
    static void 	bangMessCallback(void *data);
    static void 	loadMessCallback(void *data, t_symbol *path, t_floatarg nsamples);

    // internal data
    t_outlet        *m_dataout;
    int             x_classify;

    // open cv classifier data
    char            *x_filepath;
    int             x_nsamples;
    int             x_rsamples;
    cv::Mat         trainData;
    cv::Mat         trainClasses;
    cv::Mat         x_nearest;
    cv::Mat         x_dist;
    int             x_pwidth;
    int             x_pheight;

    std::string localPath;

    cv::Ptr<cv::ml::KNearest> x_knn{};
};
