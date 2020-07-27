#include "pix_opencv_warpperspective.hpp"
#include "pix_opencv_utils.hpp"

#include <stdio.h>

CPPEXTERN_NEW(pix_opencv_warpperspective)

/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_warpperspective :: pix_opencv_warpperspective()
{ 
  //inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("minarea"));
  //inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("maxarea"));
  m_dataout = outlet_new(this->x_obj, 0);
  //m_countout = outlet_new(this->x_obj, 0);
  comp_xsize  = 320;
  comp_ysize  = 240;

  mapMatrix = cv::Mat::eye(3,3, CV_32FC1);
    
  flags =  cv::WARP_FILL_OUTLIERS; // TODO add a set method
  findmethod = 0; // TODO add a set method
  
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_warpperspective :: processImage(imageStruct &image)
{
  auto mat = image2mat(image);
  cv::warpPerspective(mat, mat, mapMatrix, mat.size(), flags);
}

/////////////////////////////////////////////////////////
// floatThreshMess
//
/////////////////////////////////////////////////////////
void pix_opencv_warpperspective :: mapMatrixMess (int argc, t_atom *argv)
{
	post("set mapMatrix");
	int i;
	if (argc != 9) { 
			error("map matrix should be 3x3"); 
			return; 
		}
	for ( i = 0; i < 9 ; i++) {
		if (argv[i].a_type != A_FLOAT) { 
			error("map matrix should be float");
			return; 
		}
	}

  float data[9] = {
    argv[0].a_w.w_float,
    argv[1].a_w.w_float,
    argv[2].a_w.w_float,
    argv[3].a_w.w_float,
    argv[4].a_w.w_float,
    argv[5].a_w.w_float,
    argv[6].a_w.w_float,
    argv[7].a_w.w_float,
    argv[8].a_w.w_float
  };

  mapMatrix = cv::Mat(3,3,CV_32FC1, data);
}

void pix_opencv_warpperspective :: srcMatrixMess (int argc, t_atom *argv)
{
	int i;
	if ( argc % 2 ) { 
		error("src is should be a list of couple x/y values"); 
		return; 
	}
  if ( argc != dstMatrix.total() )
	{ 
    error("src matrix should have the same size as dst matrix (%d x %d)", dstMatrix.cols, dstMatrix.rows);
		return;
	}
	for ( i = 0; i < argc ; i++) {		
		if (argv[i].a_type != A_FLOAT) { 
				error("src matrix should be float"); 
				return; 
			}
	}

  srcMatrix = cv::Mat();

  for ( i = 0 ; i < dstMatrix.rows ; i++ )
	{
    srcMatrix.push_back(cv::Point2f(argv[i*2].a_w.w_float, argv[i*2+1].a_w.w_float));
	}
	findhomography();
}

void pix_opencv_warpperspective :: dstMatrixMess (int argc, t_atom *argv)
{
	int i;
	if ( argc % 2 ){ 
			error("dstMatrix is should be a list of x/y pairs"); 
			return; 
		}
	for ( i = 0; i < argc ; i++) {
		if (argv[i].a_type != A_FLOAT) {
				error("dstMatrix should be float"); 
				return;
			}
	}

	// fillin the dstMatrix
  dstMatrix = cv::Mat();
  for ( i = 0 ; i < argc/2 ; i++ )
	{
    dstMatrix.push_back(cv::Point2f(argv[i*2].a_w.w_float, argv[i*2+1].a_w.w_float));
	}
	findhomography();
}

void pix_opencv_warpperspective :: invertMess( int argc, t_atom *argv ){
    if ( argc == 0 ){
        error("invert need one argument (0|1)");
    } else if ( argv[0].a_type != A_FLOAT ){
        error("invert need one float argument (0|1)");   
    } else {
        int invert = argv[0].a_w.w_float;
        if (invert > 0){
            flags = static_cast<cv::InterpolationFlags>(flags | cv::WARP_INVERSE_MAP);
        } else {
            flags = static_cast<cv::InterpolationFlags>(flags ^ cv::WARP_INVERSE_MAP);
        }
    }
}

void pix_opencv_warpperspective :: findhomography( )
{
	int i,j;
    if ( srcMatrix.cols != dstMatrix.cols || srcMatrix.rows != dstMatrix.rows ) {
			error("srcMatrix and dstMatrix should have the same size to compute homography !");
			return;
		}
    mapMatrix = cv::findHomography(srcMatrix, dstMatrix, findmethod);
		for ( j = 0 ; j < 3 ; j++ ){
			for( i=0 ; i<3 ; i++){
        SETFLOAT(&mapMatrixList[i+j*3], mapMatrix.at<float>(j,i));
			}
		}
		// send out mapMatrix
    outlet_list( m_dataout, 0, 9, mapMatrixList.data());
}
/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_warpperspective :: obj_setupCallback(t_class *classPtr)
{
	// TODO add method for message "flags"
	// TODO treat list messages in a PD way ?
	  class_addmethod(classPtr, (t_method)&pix_opencv_warpperspective::mapMatrixMessCallback,
  		  gensym("mapMatrix"), A_GIMME, A_NULL);	 
	  class_addmethod(classPtr, (t_method)&pix_opencv_warpperspective::srcMatrixMessCallback,
  		  gensym("srcMatrix"), A_GIMME, A_NULL);	 
	  class_addmethod(classPtr, (t_method)&pix_opencv_warpperspective::dstMatrixMessCallback,
  		  gensym("dstMatrix"), A_GIMME, A_NULL);
	  class_addmethod(classPtr, (t_method)&pix_opencv_warpperspective::invertMessCallback,
  		  gensym("invert"), A_GIMME, A_NULL);
}

void pix_opencv_warpperspective :: mapMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv)
{
		GetMyClass(data)->mapMatrixMess(argc, argv);
}

void pix_opencv_warpperspective :: srcMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv)
{
		GetMyClass(data)->srcMatrixMess(argc, argv);
}

void pix_opencv_warpperspective :: dstMatrixMessCallback(void *data, t_symbol *s, int argc, t_atom *argv)
{
		GetMyClass(data)->dstMatrixMess(argc, argv);
}

void pix_opencv_warpperspective :: invertMessCallback(void *data, t_symbol *s, int argc, t_atom *argv)
{
		GetMyClass(data)->invertMess(argc, argv);
}
