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

#if HAVE_BGSUB
#include "pix_opencv_backgroundsubtractor.h"

using namespace cv;
using namespace std;

CPPEXTERN_NEW(pix_opencv_backgroundsubtractor);

/////////////////////////////////////////////////////////
//
// pix_opencv_backgroundsubtractor
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_backgroundsubtractor :: pix_opencv_backgroundsubtractor() : m_forceCPU(false) { 
  
  m_bgsub_algos.push_back("BackgroundSubtractor.GMG");
  m_bgsub_algos.push_back("BackgroundSubtractor.MOG");
  
  m_fgbgGMG = bgsegm::createBackgroundSubtractorGMG();
  m_fgbgMOG = bgsegm::createBackgroundSubtractorMOG();
  
  m_dataout = outlet_new(this->x_obj, 0); 

  m_mog_params = {
      MOGparam{ 
        "history",  
        "(int) Length of the history", 
        /*[](const bgsegm::BackgroundSubtractorMOG& alg) { return float(alg.getHistory()); },
        [](bgsegm::BackgroundSubtractorMOG& alg, float val) { alg.setHistory(int(val)); }
        */
        &bgsegm::BackgroundSubtractorMOG::getHistory, 
        &bgsegm::BackgroundSubtractorMOG::setHistory 
      },
      MOGparam{ 
        "nmixtures" , 
        "(int) Number of Gaussian mixtures", 
        &bgsegm::BackgroundSubtractorMOG::getNMixtures, 
        &bgsegm::BackgroundSubtractorMOG::setNMixtures 
      },
      MOGparam{ 
        "backgroundratio", 
        "(float) Background ratio", 
        &bgsegm::BackgroundSubtractorMOG::getBackgroundRatio, 
        &bgsegm::BackgroundSubtractorMOG::setBackgroundRatio 
      },
      MOGparam{ 
        "noisesigma", 
        "(float) Noise strength (standard deviation of the brightness or each color channel). 0 means some automatic value.", 
        &bgsegm::BackgroundSubtractorMOG::getNoiseSigma, 
        &bgsegm::BackgroundSubtractorMOG::setNoiseSigma 
      }
    };

  m_gmg_params = {
    GMGparam{
      "maxfeatures",
      "(int) : total number of distinct colors to maintain in histogram",
      &bgsegm::BackgroundSubtractorGMG::getMaxFeatures,
      &bgsegm::BackgroundSubtractorGMG::setMaxFeatures
    },
    GMGparam{
      "defaultlearningrate",
      "(float) : learning rate of the algorithm",
      &bgsegm::BackgroundSubtractorGMG::getDefaultLearningRate,
      &bgsegm::BackgroundSubtractorGMG::setDefaultLearningRate
    },
    GMGparam{
      "numframes",
      "(int) : number of frames used to initialize background model",
      &bgsegm::BackgroundSubtractorGMG::getNumFrames,
      &bgsegm::BackgroundSubtractorGMG::setNumFrames
    },
    GMGparam{
      "quantizationlevels",
      "(int) : parameter used for quantization of color-space",
      &bgsegm::BackgroundSubtractorGMG::getQuantizationLevels,
      &bgsegm::BackgroundSubtractorGMG::setQuantizationLevels
    },
    GMGparam{
      "backgroundprior",
      "(float) : prior probability that each individual pixel is a background pixel",
      &bgsegm::BackgroundSubtractorGMG::getBackgroundPrior,
      &bgsegm::BackgroundSubtractorGMG::setBackgroundPrior
    },
    GMGparam{
      "smoothingraius",
      "(int) : kernel radius used for morphological operations",
      &bgsegm::BackgroundSubtractorGMG::getSmoothingRadius,
      &bgsegm::BackgroundSubtractorGMG::setSmoothingRadius
    },
    GMGparam{
      "decisionthreshold",
      "(float) : value of decision threshold",
      &bgsegm::BackgroundSubtractorGMG::getDecisionThreshold,
      &bgsegm::BackgroundSubtractorGMG::setDecisionThreshold
    },
    GMGparam{
      "updatebackgroundmodel",
      "(bool) : status of background model update",
      &bgsegm::BackgroundSubtractorGMG::getUpdateBackgroundModel,
      &bgsegm::BackgroundSubtractorGMG::setUpdateBackgroundModel
      /*
      [](const bgsegm::BackgroundSubtractorGMG& alg) { return float(alg.getUpdateBackgroundModel()); },
      [](bgsegm::BackgroundSubtractorGMG& alg, float val) { alg.setUpdateBackgroundModel(val>0); }        
      */
    },
    GMGparam{
      "minval",
      "(float) : minimum value taken on by pixels in image sequence",
      &bgsegm::BackgroundSubtractorGMG::getMinVal,
      &bgsegm::BackgroundSubtractorGMG::setMinVal
    },
    GMGparam{
      "maxval",
      "(float) : maximum value taken on by pixels in image sequence.",
      &bgsegm::BackgroundSubtractorGMG::getMaxVal,
      &bgsegm::BackgroundSubtractorGMG::setMaxVal
    }
  };
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_backgroundsubtractor :: ~pix_opencv_backgroundsubtractor()
{ 
}

/////////////////////////////////////////////////////////
// StartRendering
//
/////////////////////////////////////////////////////////

void pix_opencv_backgroundsubtractor :: startRendering(){
  m_rendering = true;
}

void pix_opencv_backgroundsubtractor :: stopRendering(){
  m_rendering = false;
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////    	
void pix_opencv_backgroundsubtractor :: processImage(imageStruct &image)
{ 
  Mat imgMat, input;
  if ( image.csize == 1 ){
    imgMat = Mat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
    input = imgMat;
  } else if ( image.csize == 4 ){
    imgMat = Mat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to cv::Mat without copying data
    cvtColor(imgMat, input, CV_RGBA2RGB);
  } else {
    verbose(1,"suport only RGBA or GRAY image");
    return;
  }
  
  if (!m_fgbgMOG.empty()){
    m_fgbgMOG->apply(input, m_fgmask);
  } else if (!m_fgbgGMG.empty()) {
    m_fgbgGMG->apply(input, m_fgmask);
  } else {
    error("Please load a valid algorithm before processing.");
    return;
  }
  
  if ( image.csize == 1 ){ // if grayscale, send out fgmask
    image.data = m_fgmask.data;
  } else { // else, set only alpha channel
    std::vector<cv::Mat> split;
    cv::split(imgMat, split);
    split.pop_back();
    split.push_back(m_fgmask); // add fgmask as alpha channel
    cv::merge(split, imgMat);
  }
}

void pix_opencv_backgroundsubtractor :: paramHelpMess(){
  post("MOG parameters :");
  for(auto& param : m_mog_params)
  {
    post("%s : %s", param.name.c_str(), param.description.c_str());
  }

  post("GMG parameters :");
  for(auto& param : m_gmg_params)
  {
    post("%s : %s", param.name.c_str(), param.description.c_str());
  }
}

void pix_opencv_backgroundsubtractor :: enumParamsMess(){

  t_atom a_prop[2];

  if(m_algoName == "MOG")
  {
    SETFLOAT(a_prop, m_mog_params.size());
    outlet_anything( m_dataout, gensym("params"), 1, a_prop);

    for (auto& p : m_mog_params){
      SETSYMBOL(a_prop, gensym(p.name.c_str()));
      SETFLOAT(a_prop+1, p.get(*m_fgbgMOG));
      outlet_anything(m_dataout, gensym("paramList"), 2, a_prop);
    }
  } else if(m_algoName == "GMG")
  {
    SETFLOAT(a_prop, m_gmg_params.size());
    outlet_anything( m_dataout, gensym("params"), 1, a_prop);

    for (auto& p : m_gmg_params){
      SETSYMBOL(a_prop, gensym(p.name.c_str()));
      SETFLOAT(a_prop+1, p.get(*m_fgbgGMG));
      outlet_anything(m_dataout, gensym("paramList"), 2, a_prop);
    }
  }
}

void pix_opencv_backgroundsubtractor :: setParamMess(t_symbol *s, int argc, t_atom* argv){
  if ( argc < 2 ){
    error("setParam needs 2 args: paramName, value");
    return;
  }
  
  if ( argv[0].a_type != A_SYMBOL ){
    error("1st argument should be paramName (symbol)");
    return;
  }
  t_symbol* paramName = atom_getsymbol(argv);

  if ( argv[1].a_type != A_FLOAT ){
    error("currently support only float parameter values"); // I don't know the signification of output of paramType(), so only support float
  }
  
  size_t i;
  float val = atom_getfloat(argv+1);

  if(m_algoName == "MOG")
  {
    for(auto& p : m_mog_params)
    {
      if(paramName->s_name == p.name)
      {
        p.set(*m_fgbgMOG, val);
        return;
      }
    }  
  } 
  else if ( m_algoName == "GMG" )
  {
    for(auto& p : m_gmg_params)
    {
      if(paramName->s_name == p.name)
      {
        p.set(*m_fgbgGMG, val);
        return;
      }
    } 
  }
  
  error("can't find parameter %s", paramName->s_name);
}

void pix_opencv_backgroundsubtractor :: getParamMess(t_symbol *paramName){

  if(m_algoName == "MOG")
  {
    for(auto& p : m_mog_params)
    {
      if(paramName->s_name == p.name)
      {
        float val = p.get(*m_fgbgMOG);

        t_atom a_val[2];
        SETSYMBOL(a_val, paramName);
        SETFLOAT(a_val+1, val);
        outlet_anything(m_dataout, gensym("param"), 2, a_val);

        return;
      }
    }  
  } 
  else if ( m_algoName == "GMG" )
  {
    for(auto& p : m_gmg_params)
    {
      if(paramName->s_name == p.name)
      {
        float val = p.get(*m_fgbgGMG);

        t_atom a_val[2];
        SETSYMBOL(a_val, paramName);
        SETFLOAT(a_val+1, val);
        outlet_anything(m_dataout, gensym("param"), 2, a_val);  
        return;
      }
    } 
  }
  error("can't find parameter %s", paramName->s_name);
}

void pix_opencv_backgroundsubtractor :: algoMess(t_symbol *s, int argc, t_atom* argv){
  if ( argc == 0 ) {
    t_atom a_prop[2];
    SETFLOAT(a_prop, m_bgsub_algos.size());
    outlet_anything( m_dataout, gensym("algos"), 1, a_prop);
    
    for (size_t i=0; i < m_bgsub_algos.size(); i++){
      SETFLOAT(a_prop,i);
      SETSYMBOL(a_prop+1, gensym(m_bgsub_algos[i].c_str()));
      outlet_anything(m_dataout, gensym("algoList"), 2, a_prop);
    }
    return;
  }
  
  m_algoName = "NONE";
  
  if ( argv[0].a_type == A_FLOAT ){
    int id_max = m_bgsub_algos.size()-1;
    int id = atom_getfloat(argv);
    if ( id > id_max ) id = id_max;

    if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
    {
      error("Failed to create %s Algorithm.", m_bgsub_algos[id].c_str());
    } else {
      verbose(2,"bgsub %d : \"%s\" created.",id, m_bgsub_algos[id].c_str());
      m_algoName = m_bgsub_algos[id].substr(21);
    }
  } else if ( argv[0].a_type == A_SYMBOL ) {
    t_symbol* algoSym = atom_getsymbol(argv);
    string algo = algoSym->s_name;

    if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
    {
      error("Failed to create %s Algorithm.", algo.c_str());
    } else {
      verbose(2,"bgsub : \"%s\" created.",algo.c_str());
      m_algoName = algo.substr(21);
    }
  }
  if ( !m_forceCPU) m_gpuMode=true;
}

void pix_opencv_backgroundsubtractor :: cpuModeMess( int val ){
  m_forceCPU = val > 0;
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_backgroundsubtractor :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG(classPtr,  "algo",       algoMess);
  CPPEXTERN_MSG0(classPtr, "enumParams", enumParamsMess);
  CPPEXTERN_MSG(classPtr,  "setParam",   setParamMess);
  CPPEXTERN_MSG1(classPtr, "getParam",   getParamMess,     t_symbol*);
  CPPEXTERN_MSG0(classPtr, "paramHelp",  paramHelpMess);
  CPPEXTERN_MSG1(classPtr, "cpuMode",    cpuModeMess,      int);
}
#endif /* HAVE_BGSUB */

