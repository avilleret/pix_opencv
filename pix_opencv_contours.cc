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
// pix_opencv_contours extract and simplify contours of incomming image
// by Antoine Villeret - 2012

#include "pix_opencv_contours.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>

using namespace cv;

CPPEXTERN_NEW(pix_opencv_contours)

/////////////////////////////////////////////////////////
//
// pix_opencv_contours
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_contours :: pix_opencv_contours() :   \
                        m_repeat_point(1), \
                        m_epsilon(2), \
                        m_enable_contours(1), \
                        m_enable_hulls(1), \
                        m_enable_defects(1), \
                        m_hierarchy_level(-1), \
                        m_taboutput(0), \
                        m_enable_cvblob(0), \
                        m_areaThreshold(30), \
                        m_totalPointsCount(0), \
                        m_autoresize(0), \
                        m_x_arrayname(NULL), \
                        m_y_arrayname(NULL), \
                        m_z_arrayname(NULL)
{ 
  m_dataout_middle = outlet_new(this->x_obj, 0);
  m_dataout_right = outlet_new(this->x_obj, 0);

  //~ post("build on %s at %s", __DATE__, __TIME__);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_contours :: ~pix_opencv_contours()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_contours :: processRGBAImage(imageStruct &image)
{ 
  error( "pix_opencv_contours : rgba format not supported" );
}

void pix_opencv_contours :: processRGBImage(imageStruct &image) {
  error( "pix_opencv_contours : rgb format not supported");
}

void pix_opencv_contours :: processYUVImage(imageStruct &image) {
  error( "pix_opencv_contours : yuv format not supported" );
}
      
void pix_opencv_contours :: processGrayImage(imageStruct &image)
{ 
  if ( image.xsize < 0 || image.ysize < 0 ) return;

  cv::Mat imgMat2( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
  
  cv::Mat imgMat = imgMat2.clone(); // copy data because findContours need it...

  m_contours.clear();
  m_convexhulls.clear();
  m_area.clear();
  
  /*****************/
  /* Find Contours */
  /*****************/

  std::vector<std::vector<cv::Point> > contours;
  std::vector<cv::Vec4i> hierarchy;

  cv::findContours(imgMat, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE);
  
  /*
  std::cout << "hierarchy : \n" << std::endl;
  std::cout << "id\tnext\tprev\tchild\tparent" << std::endl;
  for ( size_t i = 0; i < contours.size(); i++ )
  {
    std::cout << i << "\t" << hierarchy[i][0] << "\t" << hierarchy[i][1] << "\t" << hierarchy[i][2] << "\t" << hierarchy[i][3] << std::endl;
  }
  */
  
  if ( m_hierarchy_level == -1 ) {  
    for ( size_t i = 0; i < contours.size(); i++ )
    {
      int area = cv::contourArea(contours[i], false);
      if ( area > m_areaThreshold ){
        std::vector<cv::Point> one_contour;
        if (m_epsilon > 0) { 
          cv::approxPolyDP(contours[i], one_contour, m_epsilon, true);
        } else {
          one_contour = contours[i];
        }
        m_contours.push_back(one_contour);
        m_area.push_back(area);
      }
    }
  } else if ( m_hierarchy_level==-2 ) {
    for ( size_t i = 0; i < contours.size(); i++ )
    {
      int area = cv::contourArea(contours[i], false);
      if ( area > m_areaThreshold && hierarchy[i][2] == -1 ){ // if contour area > threshold and if contour has no child (a hole)
        std::vector<cv::Point> one_contour;
        if (m_epsilon > 0) { 
          cv::approxPolyDP(contours[i], one_contour, m_epsilon, true);
        } else {
          one_contour = contours[i];
        }
        m_contours.push_back(one_contour);
        m_area.push_back(area);
      }
    }
  } else {
    int i=0;
    int hierarchy_level=0;
    
    while ( i < (int) contours.size() && i!=-1 && hierarchy_level != -1 ) 
    {
      int area = cv::contourArea(contours[i], false);
      if ( area > m_areaThreshold && hierarchy_level == m_hierarchy_level )
      {
        std::vector<cv::Point> one_contour;
        if (m_epsilon > 0) { 
          cv::approxPolyDP(contours[i], one_contour, m_epsilon, true);
        } else {
          one_contour = contours[i];
        }
        m_contours.push_back(one_contour); // push contour if it's big enough
        m_area.push_back(area);
      }
      if ( hierarchy_level < m_hierarchy_level && hierarchy[i][2] != -1 ){ // si on n'a pas atteint le niveau choisi et qu'il y a un enfant on le prend
        hierarchy_level++;
        int j = i;
        i=hierarchy[j][2]; // get the first child
      } else if ( hierarchy[i][0] != -1 ) {
        i=hierarchy[i][0]; // get the next contour at this hierarchy level if it exists
      } else {
        while ( hierarchy_level != -1 )
        {
          hierarchy_level--;
          i=hierarchy[i][3];
          if ( i < 0 ) break; // pas de parent...
          if ( hierarchy[i][0] != -1 ) {
            i=hierarchy[i][0]; // next du parent
            break;
          }
        }

      }
    }
  }
  
  outputCount();
  outputBlobs(image);
  outputContours(image);
  
  //~ cv::drawContours(imgMat2, m_contours, -1, cv::Scalar(128,255,255), 3);
  
  /**********************/
  /* Compute Convexhull */
  /**********************/
  if ( m_enable_defects || m_enable_hulls )
  {
    for ( size_t i = 0; i < m_contours.size(); i++ )
    {
      std::vector<int> convexhull;
      cv::convexHull(m_contours[i], convexhull);
      m_convexhulls.push_back(convexhull);
    }
  }
  
  if ( m_enable_hulls )
  {  
    for ( size_t i = 0 ; i < m_convexhulls.size() ; i++ )
    {
      int list_size=(int) m_convexhulls[i].size()*2+2;
      
      t_atom* data = new t_atom[list_size];
      
      SETFLOAT(data,m_convexhulls[i].size()); // nb of points for current convexhull
      SETFLOAT(data+1, 2); // each point is represented by 2 values
      
      t_atom* apt=data+2;

      for ( size_t j = 0 ; j < m_convexhulls[i].size() ; j++){
        int k = m_convexhulls[i][j];
        cv::Point pt = m_contours[i][k];
        SETFLOAT(apt,   (float) pt.x/image.xsize);
        SETFLOAT(apt+1, (float) pt.y/image.ysize);
        apt+=2;
      }
      outlet_anything(m_dataout_middle, gensym("convexhull"), list_size, data);
      
      if (data) delete data;
      data = NULL;
    }
  }
  
  /*****************************/
  /* Compute convexity defects */
  /*****************************/
  if ( m_enable_defects )
  {
    for ( size_t i = 0 ; i < m_contours.size() ; i++ )
    {
      std::vector<cv::Vec4i> defects(m_convexhulls[i].size());
      
      cv::Ptr<CvMemStorage> storage = cvCreateMemStorage();
      cv::InputArray _points = m_contours[i];
      cv::InputArray _hull = m_convexhulls[i];
      
      cv::Mat points =  _points.getMat();
      cv::Mat hull = _hull.getMat();
      
      CvMat c_points = points, c_hull = hull;
      
      CvSeq* seq = cvConvexityDefects(&c_points, &c_hull, storage);
      
      double norm = sqrtf( image.xsize*image.ysize );
    
      if ( !seq ) {
          error("seq undefined...");
          continue;
      }
      
      int list_size=(int) seq->total*7+2; 
      
      if (seq->total > 0) 
      {
        t_atom* data = new t_atom[list_size];
        
        SETFLOAT(data, seq->total); // number of defect for current contour
        SETFLOAT(data+1, 7); // a defect is represented by 7 values : start point (x,y), end point (x,y), farthest point (x,y) and defect depth
        
          cv::SeqIterator<CvConvexityDefect> it = cv::Seq<CvConvexityDefect>(seq).begin(); // TODO : crash sometimes but don't know why yet...
        t_atom* apt = data+2;
        
        for ( int j = 0 ; j < seq->total ; j++, ++it )
        {
          CvConvexityDefect& defect = *it;
          SETFLOAT(apt, (float) defect.start->x/image.xsize);
          SETFLOAT(apt+1, (float) defect.start->y/image.ysize);
          SETFLOAT(apt+2, (float) defect.end->x/image.xsize);
          SETFLOAT(apt+3, (float) defect.end->y/image.ysize);
          SETFLOAT(apt+4, (float) defect.depth_point->x/image.xsize);
          SETFLOAT(apt+5, (float) defect.depth_point->y/image.ysize);
          SETFLOAT(apt+6, (float) defect.depth/norm);
          apt+=7;
        }
        
        outlet_anything(m_dataout_middle, gensym("convexitydefects"), list_size, data);
        
        if (data) delete data;
        data = NULL;
      }
    }
  }
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_contours :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG1(classPtr, "epsilon",  epsilonMess,     double);                  
  CPPEXTERN_MSG1(classPtr, "area",  areaMess,       double);                  
  CPPEXTERN_MSG1(classPtr, "contours",  contoursMess,       double);                  
  CPPEXTERN_MSG1(classPtr, "cvblobOutput",  cvblobMess,       double);                  
  CPPEXTERN_MSG1(classPtr, "convexhulls",  convexhullsMess,       double);                  
  CPPEXTERN_MSG1(classPtr, "convexitydefects",  convexitydefectsMess,       double);                  
  CPPEXTERN_MSG1(classPtr, "hierarchy_level",  hierarchyMess,       double);                  
  CPPEXTERN_MSG1(classPtr, "taboutput",  taboutputMess,       float);                  
  CPPEXTERN_MSG3(classPtr, "settab",  tableMess, t_symbol*, t_symbol*, t_symbol*);                  
  CPPEXTERN_MSG1(classPtr, "repeat_point",  repeat_pointMess, float);                  
}

void pix_opencv_contours :: outputCount(){
  m_totalPointsCount=0;
  for( size_t i = 0 ; i < m_contours.size(); i++ ){
      m_totalPointsCount+=m_contours[i].size();
  }
  m_totalPointsCount+=m_contours.size()*m_repeat_point*2; // add 2 points for each contour (on start and end)
  
  t_atom count_atom[2];
  SETFLOAT(count_atom, m_contours.size());
  SETFLOAT(count_atom+1, m_totalPointsCount);
  outlet_anything(m_dataout_right, gensym("count"), 2, count_atom);
}

void pix_opencv_contours :: outputBlobs(imageStruct &image){
  
  if ( m_enable_cvblob )
  {          
    int blob_num=m_contours.size();
    int blobMatrixWidth=17;
    int blob_atom_size = 2+blob_num*blobMatrixWidth;
    
    t_atom* blob_atom = new t_atom[blob_atom_size];
    for ( int i = 0; i < blob_atom_size; i++){
      SETFLOAT(blob_atom+i,0);
    }
    
    SETFLOAT(blob_atom+1, blobMatrixWidth);
    
    int count(0);
    int imageArea = image.xsize * image.ysize;
    for( size_t i = 0 ; i < m_contours.size(); i++ )
    {
      if (!m_contours[i].empty() && m_contours[i].size() > 2) {
        
        /* compute centroid */
        Moments mu = moments(m_contours[i]);
        Point2f centroid;
        centroid.x=mu.m10/mu.m00;
        centroid.y=mu.m01/mu.m00;
        cv::RotatedRect rot_rect = cv::minAreaRect(m_contours[i]);
        cv::Point2f corners[4];
        rot_rect.points(corners);
        double length = cv::arcLength(m_contours[i],true);
        float area = m_area[i];
        
        t_atom* apt = blob_atom+2+i*blobMatrixWidth;
        
        SETFLOAT(apt, count); // set Id
        count++;
        SETFLOAT(apt+1, rot_rect.center.x/image.xsize); // rotrect center
        SETFLOAT(apt+2, rot_rect.center.y/image.ysize);
        SETFLOAT(apt+3, rot_rect.size.width/image.xsize); // blob size
        SETFLOAT(apt+4, rot_rect.size.height/image.ysize);
        SETFLOAT(apt+5, rot_rect.angle); // rotrect angle
        SETFLOAT(apt+6, area/imageArea); // blob area in % of image sizes
        
        t_atom* apt2 = apt+7;
              
        // blob rot rect 4 corners
        for (int j=0;j<4;j++) {
          SETFLOAT(apt2, corners[j].x/image.xsize);
          SETFLOAT(apt2+1, corners[j].y/image.ysize);
          apt2+=2;
        }

        SETFLOAT(apt+15, m_contours[i].size()+m_repeat_point*2); // number of points in segment
        SETFLOAT(apt+16, (float) length);        
      }
    }
        
    SETFLOAT(blob_atom, (float) count);
    if (count) outlet_anything(m_dataout_right, gensym("cvblob"), count*blobMatrixWidth+2, blob_atom);
    else outlet_float(m_dataout_right, 0);

    if (blob_atom) delete blob_atom;
    blob_atom = NULL;
  }
}

void pix_opencv_contours :: outputContours(imageStruct &image){
  if ( m_enable_contours ){
    if ( !m_taboutput ){
      for( size_t i = 0 ; i < m_contours.size() ; i++ )
      {
                      
        if (!m_contours[i].empty() && m_contours[i].size() > 2) {
          int size = 2+(m_repeat_point*2+m_contours[i].size())*2;
          t_atom*acontours = new t_atom[size];
          t_atom* apt=acontours;
          SETFLOAT(apt, static_cast<t_float>(m_repeat_point*2+m_contours[i].size()));
          SETFLOAT(apt+1, 2.0);
          
          apt+=2;
          
          for ( size_t j = 0 ; j < m_repeat_point ; j++){
            cv::Point pt = m_contours[i][0];
            SETFLOAT(apt, (float) pt.x/image.xsize);
            SETFLOAT(apt+1,(float) pt.y/image.ysize);
            apt+=2;
          }
                      
          for ( size_t j = 1 ; j < m_contours[i].size() ; j++){
            cv::Point pt = m_contours[i][j];
            SETFLOAT(apt,(float) pt.x/image.xsize);
            SETFLOAT(apt+1,(float) pt.y/image.ysize);
            apt+=2;
          }
                      
          for ( size_t j = 0 ; j < m_repeat_point ; j++){
            cv::Point pt = m_contours[i][0]; // repeat the first point to close the contour
            SETFLOAT(apt, (float) pt.x/image.xsize);
            SETFLOAT(apt+1,(float) pt.y/image.ysize);
            apt+=2;
          }
          
          outlet_anything(m_dataout_middle, gensym("contour"), size, acontours);
          if(acontours) {
            delete acontours;
            acontours=NULL;  
          }
        }
      }    
    } else {
  
      //~ put contours in 3 tables.
      //~ contours are separated by 0 values
      
      if ( m_x_arrayname == NULL || m_y_arrayname == NULL || m_z_arrayname == NULL){
        error("please settab before trying to write into...");
        return;
      }
            
      int vecxsize(0), vecysize(0), veczsize(0);
      t_garray  *ax, *ay, *az;
      t_word *vecx, *vecy, *vecz;
      
      //~  check if array exist
      if (!(ax = (t_garray *)pd_findbyclass(m_x_arrayname, garray_class))){
        error("%s: no such array", m_x_arrayname->s_name);
        return;
      }
      if (!(ay = (t_garray *)pd_findbyclass(m_y_arrayname, garray_class))){
        error("%s: no such array", m_y_arrayname->s_name);
        return;
      }    
      if (!(az = (t_garray *)pd_findbyclass(m_z_arrayname, garray_class))){
        error("%s: no such array", m_z_arrayname->s_name);
        return;
      }
      
      if (!garray_getfloatwords(ax, &vecxsize, &vecx)){
        error("%s: bad template for tabwrite", m_x_arrayname->s_name);
        return;
      } else if ( vecxsize != m_totalPointsCount && m_autoresize ){
        garray_resize_long(ax,m_totalPointsCount);
        if (!garray_getfloatwords(ax, &vecxsize, &vecx)){
          error("%s: can't resize correctly", m_x_arrayname->s_name);
        return;
        } 
      }
      
      if (!garray_getfloatwords(ay, &vecysize, &vecy)){
        error("%s: bad template for tabwrite", m_y_arrayname->s_name);
        return;
      } else if ( vecysize != m_totalPointsCount && m_autoresize ){
        garray_resize_long(ay,m_totalPointsCount);
        if (!garray_getfloatwords(ay, &vecysize, &vecy)){
          error("%s: can't resize correctly", m_y_arrayname->s_name);
        return;
        } 
      }
      
      if (!garray_getfloatwords(az, &veczsize, &vecz)){
        error("%s: bad template for tabwrite", m_z_arrayname->s_name);
        return;
      } else if ( veczsize != m_totalPointsCount && m_autoresize){
        garray_resize_long(az,m_totalPointsCount);
        if (!garray_getfloatwords(az, &veczsize, &vecz)){
          error("%s: can't resize correctly", m_z_arrayname->s_name);
        return;
        } 
      }
      
      int n=0;      
      
      for( size_t i = 0 ; i < m_contours.size(); i++ )
      {
        if (n >= vecxsize || n>=vecysize || n>=veczsize)
        {
          error("array are not wide enough");
          break;
        }
        
        unsigned int j;
        cv::Point pt;
        pt = m_contours[i][0];
        //~ start with blank point
        for (j=0; j<m_repeat_point; j++){
            vecx[n].w_float = (float) pt.x/image.xsize;
            vecy[n].w_float = (float) pt.y/image.ysize;
            vecz[n].w_float = 0.;
            n++;
        }

        for ( j = 0 ; j < m_contours[i].size() ; j++) {
        
          pt = m_contours[i][j];
          
          vecx[n].w_float = (float) pt.x/image.xsize;
          vecy[n].w_float = (float) pt.y/image.ysize;
          vecz[n].w_float = 1.;
          n++;
        }
        // close contour
        pt = m_contours[i][0];
        for (j=0; j<m_repeat_point; j++){ // TODO what is this loop ??? m_repeat_point is either 0 or 1...
            vecx[n].w_float = (float) pt.x/image.xsize;
            vecy[n].w_float = (float) pt.y/image.ysize;
            vecz[n].w_float = 0.;
            n++;
        }


      }
      //~ comment the redraw fnt if not needed
      garray_redraw(ax);
      garray_redraw(ay);
      garray_redraw(az);
    }
  }
}

/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_contours :: epsilonMess(double arg)
{
  m_epsilon = arg > 0 ? arg : 0.;
}
void pix_opencv_contours :: areaMess(double arg)
{
  m_areaThreshold = arg > 0 ? arg : 30.;
}
void pix_opencv_contours :: contoursMess(double arg)
{
  m_enable_contours = arg > 0;
}
void pix_opencv_contours :: cvblobMess(double arg)
{
  m_enable_cvblob = arg > 0;
}
void pix_opencv_contours :: convexhullsMess(double arg)
{
  m_enable_hulls = arg > 0;
}
void pix_opencv_contours :: convexitydefectsMess(double arg)
{
  m_enable_defects = arg > 0;
}
void pix_opencv_contours :: hierarchyMess(int arg)
{
  m_hierarchy_level = arg < -2 ? -1 : arg;
  m_mode = m_hierarchy_level == -1 ? CV_RETR_LIST : CV_RETR_TREE;
}

void pix_opencv_contours :: taboutputMess(float arg)
{
  m_taboutput = arg > 0;
}

void pix_opencv_contours :: repeat_pointMess(float arg)
{
  m_repeat_point = arg > 1 ? arg : 1;
}

void pix_opencv_contours :: tableMess(t_symbol*xarray, t_symbol*yarray, t_symbol*zarray)
{  
  // check if arrays exist
  m_x_arrayname = xarray;
  m_y_arrayname = yarray;
  m_z_arrayname = zarray;
  
  m_taboutput = 1;
}
