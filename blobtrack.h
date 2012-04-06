#include <opencv/cv.h>
#include "opencv2/video/background_segm.hpp"
#include "opencv2/legacy/blobtrack.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc_c.h>

typedef struct DefModule_FGDetector
{
    CvFGDetector* (*create)();
    const char* nickname;
    const char* description;
} DefModule_FGDetector;

typedef struct DefModule_BlobDetector
{
    CvBlobDetector* (*create)();
    const char* nickname;
    const char* description;
} DefModule_BlobDetector;

typedef struct DefModule_BlobTracker
{
    CvBlobTracker* (*create)();
    const char* nickname;
    const char* description;
} DefModule_BlobTracker;

typedef struct DefModule_BlobTrackGen
{
    CvBlobTrackGen* (*create)();
    const char* nickname;
    const char* description;
} DefModule_BlobTrackGen;

typedef struct DefModule_BlobTrackPostProc
{
    CvBlobTrackPostProc* (*create)();
    const char* nickname;
    const char* description;
} DefModule_BlobTrackPostProc;

typedef struct DefModule_BlobTrackAnalysis
{
    CvBlobTrackAnalysis* (*create)();
    const char* nickname;
    const char* description;
} DefModule_BlobTrackAnalysis;
