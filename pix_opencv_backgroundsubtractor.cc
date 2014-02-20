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

CPPEXTERN_NEW(pix_opencv_backgroundsubtractor);

/////////////////////////////////////////////////////////
//
// pix_opencv_backgroundsubtractor
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_backgroundsubtractor :: pix_opencv_backgroundsubtractor()
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
    throw(GemException("Can't find any background subtractor algorithm."));
  }
  
  if ( m_bgsub_algos[0] == "BackgroundSubtractor.GMG"){
    m_fgbgGMG = Algorithm::create<BackgroundSubtractorGMG>(m_bgsub_algos[0]);
  } else {
    m_fgbgMOG = Algorithm::create<BackgroundSubtractor>(m_bgsub_algos[0]);
  }
  if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
  {
    throw(GemException("Failed to create BackgroundSubtractor Algorithm."));
  }
  
  m_dataout = outlet_new(this->x_obj, 0); 
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
    (*m_fgbgMOG)(input, m_fgmask);
  } else if (!m_fgbgGMG.empty()) {
    (*m_fgbgGMG)(input, m_fgmask);
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
  vector<string> paramList;
  if (!m_fgbgMOG.empty()){
    m_fgbgMOG->getParams(paramList);
    post("%s parameters help :",m_fgbgMOG->name().c_str());
    for (size_t i=0; i < paramList.size(); i++){
      post("%s : %s", paramList[i].c_str(), m_fgbgMOG->paramHelp(paramList[i]).c_str());
    }
  } else if (!m_fgbgGMG.empty()) {
    m_fgbgGMG->getParams(paramList);
    post("%s parameters help :",m_fgbgGMG->name().c_str());
    for (size_t i=0; i < paramList.size(); i++){
      post("%s : %s", paramList[i].c_str(), m_fgbgGMG->paramHelp(paramList[i]).c_str());
    }
  } else {
    error("Please load a valid algorithm before requesting paramHelp.");
    return;
  }
}

void pix_opencv_backgroundsubtractor :: enumParamsMess(){
  vector<string> paramList;
  if (!m_fgbgMOG.empty()){
    m_fgbgMOG->getParams(paramList);
  } else if ( !m_fgbgGMG.empty() ){
    m_fgbgGMG->getParams(paramList);
  } else {
    error("please choose an algo before enumerating parameters.");
    return;
  }

  t_atom a_prop[2];
  SETFLOAT(a_prop, paramList.size());
  outlet_anything( m_dataout, gensym("params"), 1, a_prop);
  
  for (size_t i=0; i < paramList.size(); i++){
    SETSYMBOL(a_prop, gensym(paramList[i].c_str()));
    SETFLOAT(a_prop+1,0.);
    outlet_anything(m_dataout, gensym("paramList"), 2, a_prop);
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
  
  vector<string> paramList;
   if (!m_fgbgMOG.empty()){
    m_fgbgMOG->getParams(paramList);
  } else if ( !m_fgbgGMG.empty() ){
    m_fgbgGMG->getParams(paramList);
  } else {
    error("please choose an algo before enumerating parameters.");
    return;
  }
  size_t i;
  for (i=0; i < paramList.size(); i++){
    if ( paramList[i] == paramName->s_name ) break;
  }
  if ( i == paramList.size()){
    error("can't find parameter %s", paramName->s_name);
    return;
  }

  float val = atom_getfloat(argv+1);
  try {
    if (!m_fgbgMOG.empty()){
      m_fgbgMOG->set(paramList[i], val);
    } else if ( !m_fgbgGMG.empty() ) {
      m_fgbgGMG->set(paramList[i], val);
    }
  } catch (...) {
    error("can't set parameter %s value",paramList[i].c_str());
    return;
  }
}

void pix_opencv_backgroundsubtractor :: getParamMess(t_symbol *paramName){
  vector<string> paramList;
  if (!m_fgbgMOG.empty()){
    m_fgbgMOG->getParams(paramList);
  } else if ( !m_fgbgGMG.empty() ){
    m_fgbgGMG->getParams(paramList);
  } else {
    error("please choose an algo before enumerating parameters.");
    return;
  }
  size_t i;
  for (i=0; i < paramList.size(); i++){
    if ( paramList[i] == paramName->s_name ) break;
  }
  if ( i == paramList.size()){
    error("can't find parameter %s", paramName->s_name);
    return;
  }
  double val=0;
  try {
    if (!m_fgbgMOG.empty()){
      val = m_fgbgMOG->get<double>(paramList[i]);
    } else if ( !m_fgbgGMG.empty() ){
      val = m_fgbgGMG->get<double>(paramList[i]);
    }    
  } catch (...) {
    error("can't get parameter %s value",paramList[i].c_str());
    return;
  }
  
  t_atom a_val[2];
  SETSYMBOL(a_val, paramName);
  SETFLOAT(a_val+1, val);
  outlet_anything(m_dataout, gensym("param"), 2, a_val);
  
  
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
  
  if ( !m_fgbgMOG.empty() ) m_fgbgMOG.release();
  if ( !m_fgbgGMG.empty() ) m_fgbgGMG.release();
  
  if ( argv[0].a_type == A_FLOAT ){
    int id_max = m_bgsub_algos.size()-1;
    int id = atom_getfloat(argv);
    if ( id > id_max ) id = id_max;
    if ( m_bgsub_algos[id] == "BackgroundSubtractor.GMG"){
      m_fgbgGMG = Algorithm::create<BackgroundSubtractorGMG>(m_bgsub_algos[id]);
    } else {
      m_fgbgMOG = Algorithm::create<BackgroundSubtractor>(m_bgsub_algos[id]);
    }
    if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
    {
      error("Failed to create %s Algorithm.", m_bgsub_algos[id].c_str());
    } else {
      verbose(2,"bgsub %d : \"%s\" created.",id, m_bgsub_algos[id].c_str());
    }
  } else if ( argv[0].a_type == A_SYMBOL ) {
    t_symbol* algoSym = atom_getsymbol(argv);
    string algo = algoSym->s_name;
    if ( algo == "BackgroundSubtractor.GMG"){
      m_fgbgGMG = Algorithm::create<BackgroundSubtractorGMG>(algo);
    } else {
      m_fgbgMOG = Algorithm::create<BackgroundSubtractor>(algo);
    }
    if (m_fgbgMOG.empty() && m_fgbgGMG.empty())
    {
      error("Failed to create %s Algorithm.", algo.c_str());
    } else {
      verbose(2,"bgsub : \"%s\" created.",algo.c_str());
    }
  }
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
}
