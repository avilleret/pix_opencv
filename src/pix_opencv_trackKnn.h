/*-----------------------------------------------------------------
LOG
  GEM - Graphics Environment for Multimedia

  Copyright (c) 1997-1999 Mark Danks. mark@danks.org
  Copyright (c) Günther Geiger. geiger@epy.co.at
  Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::für::umläute. IEM. zmoelnig@iem.kug.ac.at
  Copyright (c) 2002 James Tittle & Chris Clepper
  For information on usage and redistribution, and for a DISCLAIMER OF ALL
  WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.

-----------------------------------------------------------------*/

#ifndef INCLUDE_PIX_OPENCV_TRACKKNN_H_
#define INCLUDE_PIX_OPENCV_TRACKKNN_H_

#ifndef _EiC
// #include <opencv2/bgsegm.hpp>
#include "opencv2/video.hpp"
#endif

#include "Base/GemPixObj.h"
#include <RTE/MessageCallbacks.h>

#include <stdio.h>
#include <list>

#include "Blob.hpp"

/*-----------------------------------------------------------------
-------------------------------------------------------------------
CLASS
  pix_opencv_trackKnn
  
detects contours and send them out

KEYWORDS
  pix
  
DESCRIPTION
 
-----------------------------------------------------------------*/
class GEM_EXPORT pix_opencv_trackKnn : public GemPixObj
{
  CPPEXTERN_HEADER(pix_opencv_trackKnn, GemPixObj)

public:

    //////////
    // Constructor
  pix_opencv_trackKnn();
    
protected:
    
  //////////
  // Destructor
  virtual ~pix_opencv_trackKnn();

  //////////
  // Do the processing
  virtual void 	processImage(imageStruct &image);

  // Messages handling
  void drawBlobMess(double arg);
  void cvblobMess(t_symbol *s, int argc, t_atom* argv);
  void tabinputMess(float arg);
  void taboutputMess(float arg);
  void tableMess(t_symbol *s, int argc, t_atom* argv);

private:
  
  void doTracking(void);
  int trackKnn(unsigned int blob_index, unsigned int k, double thresh);
  void outputBlob();
  void resetMess();
  
  t_outlet *m_dataout_right; // cvblob outlet
  
  unsigned int m_repeat_point;
  double m_area_threshold;	// min area for contour
  double m_epsilon;
  int m_drawBlob;
  
  std::vector<Blob>  m_trackedBlobs;
  std::vector<Blob>  m_inputBlobs;

  int m_mode, m_taboutput, m_tabinput;
  
  int IdCounter;
  
  t_symbol *m_x_arrayname, *m_y_arrayname, *m_z_arrayname, *m_id_arrayname;
    
};
#endif	// for header file
