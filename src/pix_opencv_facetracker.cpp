#include "pix_opencv_facetracker.hpp"
#include "pix_opencv_utils.hpp"

using namespace FACETRACKER;

CPPEXTERN_NEW_WITH_GIMME(pix_opencv_facetracker)

static std::vector<int> consecutive(int start, int end) {
	int n = end - start;
	std::vector<int> result(n);
	for(int i = 0; i < n; i++) {
		result[i] = start + i;
	}
	return result;
}

std::vector<int> pix_opencv_facetracker::getFeatureIndices(int feature) {
	switch(feature) {
		case LEFT_JAW: return consecutive(0, 9);
		case RIGHT_JAW: return consecutive(8, 17);
		case JAW: return consecutive(0, 17);
		case LEFT_EYEBROW: return consecutive(17, 22);
		case RIGHT_EYEBROW: return consecutive(22, 27);
		case LEFT_EYE: return consecutive(36, 42);
		case RIGHT_EYE: return consecutive(42, 48);
		case OUTER_MOUTH: return consecutive(48, 60);
		case INNER_MOUTH: {
			static int innerMouth[] = {48,60,61,62,54,63,64,65};
			return std::vector<int>(innerMouth, innerMouth + 8);
		}
		case NOSE_BRIDGE: return consecutive(27, 31);
		case NOSE_BASE: return consecutive(31, 36);
		case FACE_OUTLINE: {
			static int faceOutline[] = {17,18,19,20,21,22,23,24,25,26, 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0};
			return std::vector<int>(faceOutline, faceOutline + 27);
		}
		case ALL_FEATURES: return consecutive(0, 66);
	}
}

/////////////////////////////////////////////////////////
//
// pix_opencv_facetracker
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_facetracker :: pix_opencv_facetracker(int argc,t_atom* argv)
  :  m_fcheck(false)
  , m_scale(1)
  , m_fpd(-1)
  , m_show(true)
  , m_nIter(5)
  , m_clamp(3)
  , m_fTol(0.01)
  , m_failed(true)
  , m_fps(-1)
  , m_t1(0)
  , m_t0(0)
  , m_taboutput(0)
  , m_autoresize(0)
{ 
	m_dataout = outlet_new(this->x_obj, 0);

  //set other tracking parameters
  m_wSize1.push_back(7);
  m_wSize2.push_back(11); m_wSize2.push_back(9); m_wSize2.push_back(7);
  
  for ( int i = 0; i<13 ; i++) m_arraysname[i]=NULL;

  if(argc==3)
  {
    if(argv->a_type == A_SYMBOL
       && (argv+1)->a_type == A_SYMBOL
       && (argv+2)->a_type == A_SYMBOL)
    {
      try
      {
        t_canvas* canvas=canvas_getcurrent();
        std::string basename(canvas_getdir(canvas)->s_name);
        basename+="/";

        std::string tracker_file = basename + std::string(argv->a_w.w_symbol->s_name);
        std::string tri_file = basename + std::string((argv+1)->a_w.w_symbol->s_name);
        std::string con_file = basename + std::string((argv+2)->a_w.w_symbol->s_name);

        std::cout << "load: " << tracker_file << std::endl;
        m_tracker.Load(tracker_file.c_str());
        std::cout << "load: " << tri_file << std::endl;
        m_tri = IO::LoadTri(tri_file.c_str());
        std::cout << "load: " << con_file << std::endl;
        m_con = IO::LoadCon(con_file.c_str());
      }
      catch (const std::exception& e) {
        std::cerr << "fail to load model: " << e.what() << std::endl;
      }
    }
  }
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_facetracker :: ~pix_opencv_facetracker()
{ 
  
}

void pix_opencv_facetracker :: OutputMesh(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
  int n = shape.rows/2;
  int vecsize(0);
  t_garray *array;
  t_word *vec;
  
  for ( int i = 0; i<13 ; i++ ){
    
    std::vector<int> indices = getFeatureIndices(i);
    cv::Point pt;
    
    if ( m_arraysname[i] == NULL ){
      continue;
    }
    if (!(array = (t_garray *)pd_findbyclass(m_arraysname[i], garray_class))){
      error("%s: no such array", m_arraysname[i]->s_name);
      continue;
    }
  
    if (!garray_getfloatwords(array, &vecsize, &vec)){
      error("%s: bad template for tabwrite", m_arraysname[i]->s_name);
      continue;
    }
    //~if ( indices.size()*3 != vecsize ){
      //~garray_resize_long(array,indices.size()*3);
      //~if (!garray_getfloatwords(array, &vecsize, &vec)){
        //~error("%s: can't resize correctly", m_arraysname[i]->s_name);
        //~continue;
      //~} 
    //~}
    
    int idx(0);
    
    for( size_t j = 0 ; j < indices.size(); j++ )
    {
      idx=3*j;
      vec[idx].w_float = (float) shape.at<double>(indices[j],0)/image.cols;
      vec[idx+1].w_float = (float) shape.at<double>(indices[j]+n,0)/image.rows;
      vec[idx+2].w_float = 0.;
      //~printf("update %s, indice %d : %.2f,%.2f,%.2f\n",m_arraysname[i]->s_name, idx, vec[idx].w_float, vec[idx+1].w_float, vec[idx+2].w_float);
    }

    garray_redraw(array);
  }    
}

void pix_opencv_facetracker :: Draw(cv::Mat &image,cv::Mat &shape,cv::Mat &con,cv::Mat &tri,cv::Mat &visi)
{
  int i,n = shape.rows/2; cv::Point p1,p2; cv::Scalar c;

  //draw triangulation
  c = CV_RGB(0,0,0);
  for(i = 0; i < tri.rows; i++){
    if(visi.at<int>(tri.at<int>(i,0),0) == 0 ||
       visi.at<int>(tri.at<int>(i,1),0) == 0 ||
       visi.at<int>(tri.at<int>(i,2),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,0),0),
		   shape.at<double>(tri.at<int>(i,0)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    cv::line(image,p1,p2,c);
    p1 = cv::Point(shape.at<double>(tri.at<int>(i,2),0),
		   shape.at<double>(tri.at<int>(i,2)+n,0));
    p2 = cv::Point(shape.at<double>(tri.at<int>(i,1),0),
		   shape.at<double>(tri.at<int>(i,1)+n,0));
    cv::line(image,p1,p2,c);
  }
  //draw connections
  c = CV_RGB(0,0,255);
  for(i = 0; i < con.cols; i++){
    if(visi.at<int>(con.at<int>(0,i),0) == 0 ||
       visi.at<int>(con.at<int>(1,i),0) == 0)continue;
    p1 = cv::Point(shape.at<double>(con.at<int>(0,i),0),
		   shape.at<double>(con.at<int>(0,i)+n,0));
    p2 = cv::Point(shape.at<double>(con.at<int>(1,i),0),
		   shape.at<double>(con.at<int>(1,i)+n,0));
    cv::line(image,p1,p2,c,1);
  }
  //draw points
  for(i = 0; i < n; i++){    
    if(visi.at<int>(i,0) == 0)continue;
    p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i+n,0));
    c = CV_RGB(255,0,0); cv::circle(image,p1,2,c);
  }return;
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
    	
void pix_opencv_facetracker :: processImage(imageStruct &image)
{ 
  cv::Mat gray = image2mat_gray(image);
  cv::Mat im = image2mat(image);

  std::vector<int> wSize; 
  if(m_failed)wSize = m_wSize2; 
  else wSize = m_wSize1; 
  
  if(m_tracker.Track(gray,wSize,m_fpd,m_nIter,m_clamp,m_fTol,m_fcheck) == 0){
    int idx = m_tracker._clm.GetViewIdx(); m_failed = false;
    OutputMesh(im,m_tracker._shape,m_con,m_tri,m_tracker._clm._visi[idx]);
    if (m_show) Draw(im,m_tracker._shape,m_con,m_tri,m_tracker._clm._visi[idx]); 
  }else{
    m_tracker.FrameReset(); m_failed = true;
  }     
  //draw framerate on display image 
  if(m_fnum >= 9){      
    m_t1 = cv::getTickCount();
    m_fps = 10.0/((double(m_t1-m_t0)/cv::getTickFrequency())/1e+6);
    m_t0 = m_t1; m_fnum = 0;
  }else m_fnum += 1;
  if(m_show){
    char text[256];
    sprintf(text,"%.2f frames/sec",(float)m_fps);
    cv::putText(im,text,cv::Point(10,20), cv::FONT_HERSHEY_SIMPLEX,0.5,CV_RGB(255,255,255));
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_facetracker :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG0(classPtr, "reset", resetMess);
  CPPEXTERN_MSG1(classPtr, "show", showMess, int);
  CPPEXTERN_MSG2(classPtr, "settab",  tableMess, t_symbol*, t_symbol*);                  
  CPPEXTERN_MSG1(classPtr, "tolerance", toleranceMess, float);
  CPPEXTERN_MSG1(classPtr, "clamp", clampMess, float);
  CPPEXTERN_MSG1(classPtr, "filter", filterMess, float);
}

void pix_opencv_facetracker :: resetMess(void){
  m_tracker.FrameReset();
}

void pix_opencv_facetracker :: showMess(int flag){
  m_show = flag>0;
}

void pix_opencv_facetracker :: tableMess(t_symbol*featurename, t_symbol*arrayname)
{
  std::string name(featurename->s_name);

  int i=-1;
  if ( name == "LEFT_EYEBROW" ) i = 0;
  else if ( name == "RIGHT_EYEBROW" ) i = 1;
  else if ( name == "LEFT_EYE" ) i = 2;
  else if ( name == "RIGHT_EYE" ) i = 3;
  else if ( name == "LEFT_JAW" ) i = 4;
  else if ( name == "RIGHT_JAW" ) i = 5;
  else if ( name == "JAW" ) i = 6;
  else if ( name == "OUTER_MOUTH" ) i = 7;
  else if ( name == "INNER_MOUTH" ) i = 8;
  else if ( name == "NOSE_BRIDGE" ) i = 9;
  else if ( name == "NOSE_BASE" ) i = 10;
  else if ( name == "FACE_OUTLINE" ) i = 11;
  else if ( name == "ALL_FEATURES" ) i = 12;
  if ( i < 0 ) error("can't find feature %s",name.c_str());
  else { m_arraysname[i] = arrayname; m_taboutput = 1; }
}

void pix_opencv_facetracker :: toleranceMess(float arg){
  m_fTol=arg;
}

void pix_opencv_facetracker :: clampMess(float arg){
  m_clamp=arg;
}

void pix_opencv_facetracker :: filterMess(float arg){
  m_nIter=int(arg);
}
