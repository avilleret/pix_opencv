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
// and on some code from CCV : http://ccv.nuigroup.com/ (the tracker it self)
// pix_opencv_trackKnn extract and simplify contours of incomming image
// by Antoine Villeret - 2012

#include "pix_opencv_trackKnn.h"

using namespace cv;
using namespace std;

CPPEXTERN_NEW(pix_opencv_trackKnn)

/////////////////////////////////////////////////////////
//
// pix_opencv_trackKnn
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_trackKnn :: pix_opencv_trackKnn() :   \
                        m_taboutput(0), \
                        IdCounter(0), \
                        m_x_arrayname(NULL), \
                        m_y_arrayname(NULL), \
                        m_z_arrayname(NULL)
{ 
  m_dataout_right = outlet_new(this->x_obj, 0);

  //~ post("build on %s at %s", __DATE__, __TIME__);
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_trackKnn :: ~pix_opencv_trackKnn()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_trackKnn :: processRGBAImage(imageStruct &image)
{ 
  error( "pix_opencv_trackKnn : rgba format not supported" );
}

void pix_opencv_trackKnn :: processRGBImage(imageStruct &image) {
  error( "pix_opencv_trackKnn : rgb format not supported");
}

void pix_opencv_trackKnn :: processYUVImage(imageStruct &image) {
  error( "pix_opencv_trackKnn : yuv format not supported" );
}
      
void pix_opencv_trackKnn :: processGrayImage(imageStruct &image)
{ 
  // nothing to do for now...
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_trackKnn :: obj_setupCallback(t_class *classPtr)
{
  CPPEXTERN_MSG(classPtr, "cvblob",  cvblobMess);                  
  CPPEXTERN_MSG1(classPtr, "drawBlob",  drawBlobMess,       int);                  
  CPPEXTERN_MSG1(classPtr, "taboutput",  taboutputMess,       float);                  
  CPPEXTERN_MSG1(classPtr, "tabinput",  tabinputMess,       float);                  
  CPPEXTERN_MSG(classPtr, "settab",  tableMess);                  
  CPPEXTERN_MSG0(classPtr, "reset",  resetMess);                  
}

/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_trackKnn :: resetMess(){
  m_trackedBlobs.clear();
  IdCounter=0;
  
}

void pix_opencv_trackKnn :: drawBlobMess(double arg)
{
  m_drawBlob = arg > 0 ? arg : 3.;
}

void pix_opencv_trackKnn :: cvblobMess(t_symbol *s, int argc, t_atom* argv){
  // supposed to receive a cvblob message formatted like an iem matrix : the 2 first elements are the matrix cols and rows
  
  // first check is matrix is valid...
  if ( argc < 2 ){
    error("this doesn't seems to be a valid cvblob matrix (too few elements !)");
    return;
  }
  
  for ( int i = 0 ; i < argc ;  i++){
    if ( argv[i].a_type != A_FLOAT ){
      error("cvblob takes only float arg");
      return;
    }
  }
  
  int blob_number = (int) atom_getfloatarg(0,argc,argv);
  int blob_atom_size = (int) atom_getfloatarg(1, argc, argv);
  
  if ( blob_number * blob_atom_size != argc - 2 ){
    error("cvblob matrix have a wrong size %d != %d x %d", argc -2, blob_number, blob_atom_size);
    return;
  }
  
  m_inputBlobs.clear();
  for ( int i = 0; i < blob_number ; i++){
    Blob blob;
    blob.id=-1;
    int j = i*blob_atom_size+2;
    if ( blob_atom_size < 3 ) j--; // if there is more than 2 element per blob, assume, the first is Id
    blob.centroid.x=atom_getfloatarg(j+1,argc,argv);
    blob.centroid.y=atom_getfloatarg(j+2,argc,argv);
    
    if ( blob_atom_size > 2 ){ // if we pass more than just x and y coordinates, assume the same data order than pix_opencv_contours outputs
        blob.angleBoundingRect.size.width=atom_getfloatarg(j+3,argc,argv);
        blob.angleBoundingRect.size.height=atom_getfloatarg(j+4,argc,argv);
        blob.angle=atom_getfloatarg(j+5,argc,argv);
        blob.area=atom_getfloatarg(j+6,argc,argv);
        
        blob.nPts=atom_getfloatarg(j+15,argc,argv);
        blob.length=atom_getfloatarg(j+16,argc,argv);
    }
    
    m_inputBlobs.push_back(blob);
  }
  
  doTracking();
}

void pix_opencv_trackKnn :: doTracking()
{
    // find an Id
  for(unsigned int i=0; i<m_trackedBlobs.size(); i++)
  {
    
    /******************************************************************
    * *****************TRACKING FUNCTION TO BE USED*******************
    * Replace 'trackKnn(...)' with any function that will take the
    * current track and find the corresponding track in the newBlobs
    * 'winner' should contain the index of the found blob or '-1' if
    * there was no corresponding blob
    *****************************************************************/
    int winner = trackKnn(i, 3, 0.);

    if(winner==-1) //track has died, mark it for deletion
    {
      //erase calibrated blob from map
      //calibratedBlobs.erase(m_trackedBlobs[i].id);

      //mark the blob for deletion
      m_trackedBlobs[i].id = -1;
    }
    else //still alive, have to update
    {
      //if winning new blob was labeled winner by another track
      //then compare with this track to see which is closer
      if(m_inputBlobs[winner].id!=-1)
      {
        //find the currently assigned blob
        unsigned int j; //j will be the index of it
        for(j=0; j<m_trackedBlobs.size(); j++)
        {
          if(m_trackedBlobs[j].id==m_inputBlobs[winner].id)
            break;
        }

        if(j==m_trackedBlobs.size())//got to end without finding it
        {
          m_inputBlobs[winner].id = m_trackedBlobs[i].id;
          m_inputBlobs[winner].age = m_trackedBlobs[i].age;
          m_inputBlobs[winner].sitting = m_trackedBlobs[i].sitting;
          m_inputBlobs[winner].downTime = m_trackedBlobs[i].downTime;
          m_inputBlobs[winner].color = m_trackedBlobs[i].color;
          m_inputBlobs[winner].lastTimeTimeWasChecked = m_trackedBlobs[i].lastTimeTimeWasChecked;

          m_trackedBlobs[i] = m_inputBlobs[winner];
        }
        else //found it, compare with current blob
        {
          double _x = m_inputBlobs[winner].centroid.x;
          double y = m_inputBlobs[winner].centroid.y;
          double xOld = m_trackedBlobs[j].centroid.x;
          double yOld = m_trackedBlobs[j].centroid.y;
          double xNew = m_trackedBlobs[i].centroid.x;
          double yNew = m_trackedBlobs[i].centroid.y;
          double distOld = (_x-xOld)*(_x-xOld)+(y-yOld)*(y-yOld);
          double distNew = (_x-xNew)*(_x-xNew)+(y-yNew)*(y-yNew);

          //if this track is closer, update the ID of the blob
          //otherwise delete this track.. it's dead
          if(distNew<distOld) //update
          {
            m_inputBlobs[winner].id = m_trackedBlobs[i].id;
            m_inputBlobs[winner].age = m_trackedBlobs[i].age;
            m_inputBlobs[winner].sitting = m_trackedBlobs[i].sitting;
            m_inputBlobs[winner].downTime = m_trackedBlobs[i].downTime;
            m_inputBlobs[winner].color = m_trackedBlobs[i].color;
            m_inputBlobs[winner].lastTimeTimeWasChecked = m_trackedBlobs[i].lastTimeTimeWasChecked;

            //TODO--------------------------------------------------------------------------
            //now the old winning blob has lost the win.
            //I should also probably go through all the newBlobs
            //at the end of this loop and if there are ones without
            //any winning matches, check if they are close to this
            //one. Right now I'm not doing that to prevent a
            //recursive mess. It'll just be a new track.

            //erase calibrated blob from map
            //calibratedBlobs.erase(m_trackedBlobs[j].id);
            //mark the blob for deletion
            m_trackedBlobs[j].id = -1;
            //------------------------------------------------------------------------------
          }
          else //delete
          {
            //erase calibrated blob from map
            //calibratedBlobs.erase(m_trackedBlobs[i].id);
            //mark the blob for deletion
            m_trackedBlobs[i].id = -1;
          }
        }
      }
      else //no conflicts, so simply update
      {
        m_inputBlobs[winner].id = m_trackedBlobs[i].id;
        m_inputBlobs[winner].age = m_trackedBlobs[i].age;
        m_inputBlobs[winner].sitting = m_trackedBlobs[i].sitting;
        m_inputBlobs[winner].downTime = m_trackedBlobs[i].downTime;
        m_inputBlobs[winner].color = m_trackedBlobs[i].color;
        m_inputBlobs[winner].lastTimeTimeWasChecked = m_trackedBlobs[i].lastTimeTimeWasChecked;
      }
    }
  }

  for(unsigned int i=0; i<m_trackedBlobs.size(); i++)
  {
    if(m_trackedBlobs[i].id==-1) //dead
    {
      //erase track
      m_trackedBlobs.erase(m_trackedBlobs.begin()+i,
        m_trackedBlobs.begin()+i+1);
      //m_trackedBlobs.erase(m_trackedBlobs.begin()+i);

      i--; //decrement one since we removed an element
    }
    else //living, so update it's data
    {
      for(unsigned int j=0; j<m_inputBlobs.size(); j++)
      {
        if(m_trackedBlobs[i].id==m_inputBlobs[j].id)
        {
          //update track
          cv::Point tempLastCentroid = m_trackedBlobs[i].centroid; // assign the new centroid to the old
          m_trackedBlobs[i]=m_inputBlobs[j];
          m_trackedBlobs[i].lastCentroid = tempLastCentroid;

          //get the Differences in position
          //~m_trackedBlobs[i].D.set((m_trackedBlobs[i].centroid.x - m_trackedBlobs[i].lastCentroid.x) / (ofGetElapsedTimeMillis() - m_trackedBlobs[i].lastTimeTimeWasChecked), 
            //~(m_trackedBlobs[i].centroid.y - m_trackedBlobs[i].lastCentroid.y) / (ofGetElapsedTimeMillis() - m_trackedBlobs[i].lastTimeTimeWasChecked));

          //printf("D(%f, %f)\n", m_trackedBlobs[i].D.x, m_trackedBlobs[i].D.y);

          //if( abs((int)m_trackedBlobs[i].D.x) > 1 || abs((int)m_trackedBlobs[i].D.y) > 1) {
          //            printf("\nUNUSUAL BLOB @ %f\n-----------------------\nm_trackedBlobs[%i]\nD = (%f, %f)\nXY= (%f, %f)\nlastTimeTimeWasChecked = %f\nsitting = %f\n",
          //                 ofGetElapsedTimeMillis(),
          //                 i,
          //                 m_trackedBlobs[i].D.x,  m_trackedBlobs[i].D.y,
          //                 m_trackedBlobs[i].centroid.x, m_trackedBlobs[i].centroid.y,
          //                 m_trackedBlobs[i].lastTimeTimeWasChecked,
          //                 m_trackedBlobs[i].downTime,
          //                 m_trackedBlobs[i].sitting
          //            );
          //          }


          //calculate the accelleration
          //~ofPoint tD = m_trackedBlobs[i].D;
          //~m_trackedBlobs[i].maccel = sqrtf((tD.x* tD.x)+(tD.y*tD.y)/(ofGetElapsedTimeMillis() - m_trackedBlobs[i].lastTimeTimeWasChecked));

          //~m_trackedBlobs[i].lastTimeTimeWasChecked = ofGetElapsedTimeMillis();

          //calculate the age
          //~m_trackedBlobs[i].age = ofGetElapsedTimef() - m_trackedBlobs[i].downTime;

          //if not moving more than min_movement_threshold then set to same position as last frame
  //           if(m_trackedBlobs[i].maccel < MIN_MOVEMENT_THRESHOLD)
  //           {  //this helps avoid jittery blobs
  //             m_trackedBlobs[i].centroid.x = m_trackedBlobs[i].lastCentroid.x;
  //             m_trackedBlobs[i].centroid.y = m_trackedBlobs[i].lastCentroid.y;
  //           }

          //set sitting (held length)
          //~if(m_trackedBlobs[i].maccel < 7)
          {  //1 more frame of sitting
            //~if(m_trackedBlobs[i].sitting != -1)
              //~m_trackedBlobs[i].sitting = ofGetElapsedTimef() - m_trackedBlobs[i].downTime;           
          //~}
          //~else {
            //~m_trackedBlobs[i].sitting = -1;
          //~}

          //printf("time: %f\n", ofGetElapsedTimeMillis());
          //printf("%i age: %f, downTimed at: %f\n", i, m_trackedBlobs[i].age, m_trackedBlobs[i].downTime);

          //if blob has been 'holding/sitting' for 1 second send a held event
          //~if(m_trackedBlobs[i].sitting > 1.0f){
            //held event only happens once so set to -1
            //~m_trackedBlobs[i].sitting = -1;
          } 
        }
      }
    }
  }
  /*==================*/
  //--Add New Living Tracks
  //now every new blob should be either labeled with a tracked ID or
  //have ID of -1... if the ID is -1... we need to make a new track
  for(unsigned int i=0; i<m_inputBlobs.size(); i++)
  {
    if(m_inputBlobs[i].id==-1)
    {

      //add new track
      m_inputBlobs[i].id=IdCounter;
      IdCounter++;
      //~IdCounter=IDcounter%6; // limit the id between 0 and 6...
      
      //~m_inputBlobs[i].downTime = ofGetElapsedTimef();
      //m_inputBlobs[i].lastTimeTimeWasChecked = ofGetElapsedTimeMillis();

      //random color for blob. Could be useful?
      Mat color(1,1,CV_8UC3);
      color=Scalar(rand()%180,255,255);
      cvtColor(color,color,CV_HSV2RGB);
      
      const unsigned char* ptr=color.ptr<unsigned char>(0);
      Scalar scolor(ptr[0],ptr[1],ptr[2]);
      
      m_inputBlobs[i].color = scolor;
      m_trackedBlobs.push_back(m_inputBlobs[i]);

      //~numEnter++;
      //~if (numEnter > 20)
      //~{
        //~printf("something wrong!\n");
      //~}
      
    }
  }
  
  outputBlob();
}

void pix_opencv_trackKnn :: outputBlob(){
  if ( m_taboutput )
  {
    //~ put contours in 3 tables.
    //~ contours are separated by 0 values
    
    if ( m_x_arrayname == NULL || m_y_arrayname == NULL || m_id_arrayname == NULL){
      error("please settab before trying to write into...");
      return;
    }
          
    int size(m_trackedBlobs.size()), vecxsize(0), vecysize(0), vecidsize(0);
    t_garray  *ax, *ay, *aid;
    t_word *vecx, *vecy, *vecid;
    
    //~  check if array exist
    if (!(ax = (t_garray *)pd_findbyclass(m_x_arrayname, garray_class))){
      error("%s: no such array", m_x_arrayname->s_name);
      return;
    }
    if (!(ay = (t_garray *)pd_findbyclass(m_y_arrayname, garray_class))){
      error("%s: no such array", m_y_arrayname->s_name);
      return;
    }		
    if (!(aid = (t_garray *)pd_findbyclass(m_id_arrayname, garray_class))){
      error("%s: no such array", m_id_arrayname->s_name);
      return;
    }
    
    if (!garray_getfloatwords(ax, &vecxsize, &vecx)){
      error("%s: bad template for tabwrite", m_x_arrayname->s_name);
      return;
    } else if ( vecxsize != size ){
      garray_resize_long(ax,size);
      if (!garray_getfloatwords(ax, &vecxsize, &vecx)){
        error("%s: can't resize correctly", m_x_arrayname->s_name);
      return;
      } 
    }
    
    if (!garray_getfloatwords(ay, &vecysize, &vecy)){
      error("%s: bad template for tabwrite", m_y_arrayname->s_name);
      return;
    } else if ( vecysize != size ){
      garray_resize_long(ay,size);
      if (!garray_getfloatwords(ay, &vecysize, &vecy)){
        error("%s: can't resize correctly", m_y_arrayname->s_name);
      return;
      } 
    }
    
    if (!garray_getfloatwords(aid, &vecidsize, &vecid)){
      error("%s: bad template for tabwrite", m_id_arrayname->s_name);
      return;
    } else if ( vecidsize != size ){
      garray_resize_long(aid,size);
      if (!garray_getfloatwords(aid, &vecidsize, &vecid)){
        error("%s: can't resize correctly", m_id_arrayname->s_name);
      return;
      } 
    }
    
    for( size_t i = 0 ; i < m_trackedBlobs.size(); i++ )
    {
      vecx[i].w_float = m_trackedBlobs[i].centroid.x;
      vecy[i].w_float = m_trackedBlobs[i].centroid.y;
      vecid[i].w_float = m_trackedBlobs[i].id;
    }
    //~ comment the redraw fnt if not needed
    garray_redraw(ax);
    garray_redraw(ay);
    garray_redraw(aid);
    
  } else {
    unsigned int blob_num=m_trackedBlobs.size();
    unsigned int blobMatrixWidth=17;
    unsigned int blob_atom_size = 2+blob_num*blobMatrixWidth;
    
    t_atom* blob_atom = new t_atom[blob_atom_size];
    SETFLOAT(&blob_atom[0], blob_num);
    SETFLOAT(&blob_atom[1], blobMatrixWidth);
    
    for(unsigned int i=0; i<blob_num; i++){
      t_atom *apt=blob_atom+2+i*blobMatrixWidth;
      SETFLOAT(&apt[0], m_trackedBlobs[i].id);
      SETFLOAT(&apt[1], m_trackedBlobs[i].centroid.x);
      SETFLOAT(&apt[2], m_trackedBlobs[i].centroid.y);
      SETFLOAT(&apt[3], m_trackedBlobs[i].angleBoundingRect.size.width);
      SETFLOAT(&apt[4], m_trackedBlobs[i].angleBoundingRect.size.height);
      SETFLOAT(&apt[5], 1); // state
      SETFLOAT(&apt[6], m_trackedBlobs[i].area); // area in pixels
      
      t_atom* apt2 = apt+7;
      cv::Point2f corners[4];
      m_trackedBlobs[i].angleBoundingRect.points(corners);
            
      // blob rot rect 4 corners
      for (int j=0;j<4;j++) {
        SETFLOAT(apt2, corners[j].x);
        SETFLOAT(apt2+1, corners[j].y);
        apt2+=2;
      }

      SETFLOAT(apt+15,  m_trackedBlobs[i].nPts); // number of points in segment
      SETFLOAT(apt+16, (float) m_trackedBlobs[i].length); 
    }
    
    outlet_anything(m_dataout_right, gensym("cvblob"), blob_atom_size, blob_atom);
    if (blob_atom){
      delete blob_atom;
      blob_atom=NULL;
    }
  }
}

int pix_opencv_trackKnn :: trackKnn(unsigned int blob_index, unsigned int k, double thresh = 0)
{
  vector<Blob> newBlobs=m_inputBlobs;
  Blob *track = &m_trackedBlobs[blob_index];

  int winner = -1; //initially label track as '-1'=dead
  if((k%2)==0) k++; //if k is not an odd number, add 1 to it

  //if it exists, square the threshold to use as square distance
  if(thresh>0)
    thresh *= thresh;

  //list of neighbor point index and respective distances
  std::list<std::pair<int,double> > nbors;
  std::list<std::pair<int,double> >::iterator iter;

  //find 'k' closest neighbors of testpoint
  double x, y, xT, yT, dist;
  for(unsigned int i=0; i<newBlobs.size(); i++)
  {
    x = newBlobs[i].centroid.x;
    y = newBlobs[i].centroid.y;

    xT = track->centroid.x;
    yT = track->centroid.y;
    dist = (x-xT)*(x-xT)+(y-yT)*(y-yT);

    if(dist<=thresh)//it's good, apply label if no label yet and return
    {
      winner = i;
      return winner;
    }

    /****************************************************************
    * check if this blob is closer to the point than what we've seen
    *so far and add it to the index/distance list if positive
    ****************************************************************/

    //search the list for the first point with a longer distance
    for(iter=nbors.begin(); iter!=nbors.end()
      && dist>=iter->second; iter++);

      if((iter!=nbors.end())||(nbors.size()<k)) //it's valid, insert it
      {
        nbors.insert(iter, 1, std::pair<int, double>(i, dist));
        //too many items in list, get rid of farthest neighbor
        if(nbors.size()>k)
          nbors.pop_back();
      }
  }

  /********************************************************************
  * we now have k nearest neighbors who cast a vote, and the majority
  * wins. we use each class average distance to the target to break any
  * possible ties.
  *********************************************************************/

  // a mapping from labels (IDs) to count/distance
  std::map<int, std::pair<int, double> > votes;

  //remember:
  //iter->first = index of newBlob
  //iter->second = distance of newBlob to current tracked blob
  for(iter=nbors.begin(); iter!=nbors.end(); iter++)
  {
    //add up how many counts each neighbor got
    int count = ++(votes[iter->first].first);
    double dist = (votes[iter->first].second+=iter->second);

    /* check for a possible tie and break with distance */
    if(count>votes[winner].first || ( count==votes[winner].first && dist<votes[winner].second ))
    {
      winner = iter->first;
    }
  }

  return winner;
}

void pix_opencv_trackKnn :: taboutputMess(float arg)
{
  m_taboutput = arg > 0;
}

void pix_opencv_trackKnn :: tabinputMess(float arg)
{
  m_tabinput = arg > 0;
}

void pix_opencv_trackKnn :: tableMess(t_symbol *s, int argc, t_atom* argv)
{
  if ( argc < 3 ){
    error("table message need at least 3 argumments");
    return;
  }
  
  for ( int i = 0 ; i < argc ; i++ ){
    if ( argv[i].a_type != A_SYMBOL ){
      error("table message takes only symbol arguments, at most 4 at least 3");
      return;
    }
  }
  
  if ( argc == 3 && argv[0].a_type == A_SYMBOL && argv[1].a_type == A_SYMBOL && argv[2].a_type == A_SYMBOL ){
      
    // check if arrays exist
    m_x_arrayname = argv[0].a_w.w_symbol;
    m_y_arrayname = argv[1].a_w.w_symbol;
    m_z_arrayname = argv[2].a_w.w_symbol;
  }
  
  if ( argc > 3 ){
    m_id_arrayname= argv[0].a_w.w_symbol;
    m_x_arrayname = argv[1].a_w.w_symbol;
    m_y_arrayname = argv[2].a_w.w_symbol;
    m_z_arrayname = argv[3].a_w.w_symbol;
  }
  m_tabinput = 1;
}
