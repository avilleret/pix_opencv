#ifndef _ARMA_DETECTOR_H_
#define _ARMA_DETECTOR_H_
#include <opencv/cv.h>
#include "pattern.hpp"

using namespace std;
using namespace cv;


namespace ARma
{
	
class PatternDetector
{
public:

    //constructor
	PatternDetector();

	//distractor
	~PatternDetector(){};

enum THRES_MODE {
		FIXED_THRESHOLD,
		ADAPTIVE_THRESHOLD, 
	}; 

//detect patterns in the input frame
void detect(Mat& frame, const Mat& cameraMatrix, const Mat& distortions, vector<Mat>& library, vector<Pattern>& foundPatterns);

	int 	m_patternCount;								// patternCount
	float 	m_fixed_threshold;							// fixed threshold
	float	m_adapt_threshold;							// adaptive threshold
	int		m_adapt_block_size;							// block size for adaptive threshold
	int 	m_threshold_mode;							// 0 : no thresholding (input image should be already binarised, 1 : fixed threshold method, 2 : adaptive threshold method
	int 	m_pattern_size;								// size of pattern for recognition ( 64 is good, smaller is faster...)
	int 	m_monitor_stage;							// monitored stage, 1 : binarisation, 2 : normalised ROI...
	float	m_confidence_threshold;						// threshold above which we are sure to identify the pattern

private:
	//~ int mode, normSize, block_size;
	//~ double confThreshold, thresh1, thresh2;
	struct patInfo{
		int index;
		int ori;
		double maxCor;
	};
	Mat binImage, grayImage, normROI, patMask, patMaskInt;
	Point2f norm2DPts[4];


	void convertAndBinarize(const Mat& src, Mat& dst1, Mat& dst2, int thresh_mode = 1);
	void normalizePattern(const Mat& src, const Point2f roiPoints[], Rect& rec, Mat& dst);
	int identifyPattern(const Mat& src, std::vector<cv::Mat>& loadedPatterns, patInfo& out);

};


};
#endif
