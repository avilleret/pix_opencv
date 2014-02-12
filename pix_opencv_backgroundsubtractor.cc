////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) Günther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
// based on code written by Lluis Gomez i Bigorda ( lluisgomez _at_ hangar _dot_ org ) (pix_opencv)
// Template for pix_opencv class

#include "pix_opencv_backgroundsubtractor.h"

using namespace cv;
using namespace std;

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_opencv_backgroundsubtractor, t_floatarg, A_DEFFLOAT, t_floatarg, A_DEFFLOAT);

/////////////////////////////////////////////////////////
//
// pix_opencv_backgroundsubtractor
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_backgroundsubtractor :: pix_opencv_backgroundsubtractor(t_floatarg threshold, t_floatarg initFrames)
    : m_threshold(threshold), m_initFrames(initFrames)
{ 
  initModule_video();
  setUseOptimized(true);
  setNumThreads(8);
  
  vector<string> algorithms;
  Algorithm::getList(algorithms);
  string model = "BackgroundSubtractor.";

  for (size_t i=0; i < algorithms.size(); i++){
    if ( model.compare(0,model.length(),algorithms[i],0,model.length()) == 0 ){ // && npos >= model.length() ){
        m_bgsub_algos.push_back(algorithms[i]);
    }
  }
  
  if ( m_bgsub_algos.size() == 0){
    throw(GemException("Can't find any background subtractor algorithm, seriously dead..."));
  }
  
  post("Found %d backgroundsubtraction algorithms : ", m_bgsub_algos.size());
  for (size_t i=0; i < m_bgsub_algos.size(); i++){
    post("%d : %s", i, m_bgsub_algos[i].c_str());
  }  

  m_fgbg = Algorithm::create<BackgroundSubtractorGMG>(m_bgsub_algos[0]);
  if (m_fgbg.empty())
  {
    throw(GemException("Failed to create BackgroundSubtractor.GMG Algorithm."));
  }
  
  //~if ( m_initFrames == 0. ) m_initFrames = 20;
  //~if ( m_threshold == 0. ) m_threshold = 0.7;
  
  //~m_fgbg->set("initializationFrames", m_initFrames);
  //~m_fgbg->set("decisionThreshold", m_threshold);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_backgroundsubtractor :: ~pix_opencv_backgroundsubtractor()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////    	
void pix_opencv_backgroundsubtractor :: processImage(imageStruct &image)
{ 
  Mat imgMat;
  if ( image.csize == 1 ){
    imgMat = Mat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
  } else if ( image.csize == 4 ){
    imgMat = Mat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
  } else {
    verbose(1,"suport only RGBA or GRAY image");
    return;
  }
  
  (*m_fgbg)(imgMat, m_fgmask);
  
  if ( image.csize == 1 ){ // if grayscale, send out fgmask
    image.data = m_fgmask.data;
  } else { // else, set only alpha channel
    std::vector<cv::Mat> split;
    cv::split(imgMat, split);
    split.pop_back(); // delete alpha channel
    split.push_back(m_fgmask); // add fgmask as alpha channel
    cv::merge(split, imgMat);
  }
}

void pix_opencv_backgroundsubtractor :: initFramesMess(double val){
  m_fgbg->set("initializationFrames", val);
}


void pix_opencv_backgroundsubtractor :: thresholdMess(double val){
  m_fgbg->set("decisionThreshold", val);
}

void pix_opencv_backgroundsubtractor :: algoMess(t_symbol *s, int argc, t_atom* argv){
  printf("call algoMess\n");
  if ( argc == 0 ) {
    error("algo need symbol or float argument.");
    return;
  }
  
  if ( argv[0].a_type == A_FLOAT ){
    int id_max = m_bgsub_algos.size()-1;
    int id = atom_getfloat(argv);
    if ( id > id_max ) id = id_max;
    printf("choose id : %d\n",id);
    if ( !m_fgbg.empty() ) m_fgbg.release();
    m_fgbg = Algorithm::create<BackgroundSubtractorGMG>(m_bgsub_algos[id]);

    //~m_fgbg->setAlgorithm(m_bgsub_algos[id],m_fgbg);
    if (m_fgbg.empty())
    {
      error("Failed to create %s Algorithm.", m_bgsub_algos[id].c_str());
    } else {
      post("bgsub %d : \"%s\" created.",id, m_bgsub_algos[id].c_str());
    }
  } else if ( argv[0].a_type == A_SYMBOL ) {
    t_symbol* algoSym = atom_getsymbol(argv);
    string algo = algoSym->s_name;
    m_fgbg = Algorithm::create<BackgroundSubtractorGMG>(algo);
    if (m_fgbg.empty())
    {
      error("Failed to create BackgroundSubtractor.GMG Algorithm.");
    } else {
      post("bgsub : \"%s\" created.", algo.c_str());
    }
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_backgroundsubtractor :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "initFrames",  initFramesMess,     double);
  CPPEXTERN_MSG1(classPtr, "threshold",   thresholdMess,     double);
  CPPEXTERN_MSG(classPtr, "algo",   algoMess);
}
