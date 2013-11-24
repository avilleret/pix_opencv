/*
* Blob.hpp
*
*
* A blob is a homogenous patch represented by a polygonal contour.
* Typically a blob tracker uses the contour to figure out the blob's
* persistence and "upgrades" it with ids and other temporal
* information.
*
*/

#ifndef BLOB_H
#define BLOB_H

#include <vector>

class Blob {

public: 

  std::vector <cv::Point>    pts;     // the contour of the blob
  int                 nPts;    // number of pts; 
  int					id;
  float               area;
  float               length;
  float               angle;
  float				maccel;  //distance traveled since last frame
  float				age;     //how long the blob has been at war
  float				sitting; //how long hes been sitting in the same place
  float				downTime;
  float				lastTimeTimeWasChecked;
  cv::Rect      boundingRect;
  cv::Rect      angleBoundingRect;
  cv::Point2f   centroid, lastCentroid, D;
  bool				simulated;
  bool                hole;
  cv::Scalar					color;				

  //----------------------------------------
  Blob() {
    area 		= 0.0f;
    length 		= 0.0f;
    hole 		= false;
    nPts        = 0;
    simulated	= false;
    age			= 0.0f;
    sitting		= 0.0f;
    color		= 0xFFFFFF;

  //freakishly long variable name (ala Apple)
  //~lastTimeTimeWasChecked = ofGetElapsedTimeMillis(); //get current time as of creation
  }
};
#endif


