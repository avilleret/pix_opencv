////////////////////////////////////////////////////////
//
// GEM - Graphics Environment for Multimedia
//
// zmoelnig@iem.kug.ac.at
//
// Implementation file
//
//    Copyright (c) 1997-2000 Mark Danks.
//    Copyright (c) G�nther Geiger.
//    Copyright (c) 2001-2002 IOhannes m zmoelnig. forum::f�r::uml�ute. IEM
//    Copyright (c) 2002 James Tittle & Chris Clepper
//    For information on usage and redistribution, and for a DISCLAIMER OF ALL
//    WARRANTIES, see the file, "GEM.LICENSE.TERMS" in this distribution.
//
/////////////////////////////////////////////////////////
//
// pix_opencv_warpperspective by Antoine Villeret - 2011/13



#include "pix_opencv_warpperspective.h"
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
  gray = NULL;
  tmp = NULL;
  rgb = NULL;
  mapMatrix = cvCreateMat(3,3,CV_32FC1);
  srcMatrix = cvCreateMat(4,2,CV_32FC1);
  dstMatrix = cvCreateMat(4,2,CV_32FC1);
  
  cvSet(mapMatrix, cvScalar(0)); // set all to 0.
  CV_MAT_ELEM( *mapMatrix, float, 0, 0 ) = 1.; // then make identity
  CV_MAT_ELEM( *mapMatrix, float, 1, 1 ) = 1.;
  
  // initialize srcMatrix & dstMatrix to the same...
  CV_MAT_ELEM( *srcMatrix, float, 0, 0 ) = 0.;
  CV_MAT_ELEM( *srcMatrix, float, 0, 1 ) = 0.;
  CV_MAT_ELEM( *srcMatrix, float, 1, 0 ) = 1.;
  CV_MAT_ELEM( *srcMatrix, float, 1, 1 ) = 0.;
  CV_MAT_ELEM( *srcMatrix, float, 2, 0 ) = 1.;
  CV_MAT_ELEM( *srcMatrix, float, 2, 1 ) = 1.;
  CV_MAT_ELEM( *srcMatrix, float, 3, 0 ) = 0.;
  CV_MAT_ELEM( *srcMatrix, float, 3, 1 ) = 1.;
  
  CV_MAT_ELEM( *dstMatrix, float, 0, 0 ) = 0.;
  CV_MAT_ELEM( *dstMatrix, float, 0, 1 ) = 0.;
  CV_MAT_ELEM( *dstMatrix, float, 1, 0 ) = 1.;
  CV_MAT_ELEM( *dstMatrix, float, 1, 1 ) = 0.;
  CV_MAT_ELEM( *dstMatrix, float, 2, 0 ) = 1.;
  CV_MAT_ELEM( *dstMatrix, float, 2, 1 ) = 1.;
  CV_MAT_ELEM( *dstMatrix, float, 3, 0 ) = 0.;
  CV_MAT_ELEM( *dstMatrix, float, 3, 1 ) = 1.;
  
  mapMatrixList=new t_atom[9];
    
  flags =  CV_WARP_FILL_OUTLIERS; // TODO add a set method
  findmethod = 0; // TODO add a set method
  
  post("pix_opencv_warpperspective by Antoine Villeret");
  post("build on %s at %s", __DATE__, __TIME__);

}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_warpperspective :: ~pix_opencv_warpperspective()
{ 
    	//Destroy cv_images to clean memory
    	//if (gray) cvReleaseImage(&gray);  // TODO free image header but not data because it points to Gem image...
    	if (rgb) cvReleaseImage(&rgb);
    	//if (tmp) cvReleaseImage(&tmp); // the same
    	if (mapMatrix) cvReleaseMat(&mapMatrix);
     	if (srcMatrix) cvReleaseMat(&srcMatrix);
    	if (dstMatrix) cvReleaseMat(&dstMatrix);
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_warpperspective :: processRGBAImage(imageStruct &image)
{
  	if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!rgb)) 
	{
		this->comp_xsize = image.xsize;
		this->comp_ysize = image.ysize;

		//Destroy cv_images to clean memory
		if(gray) cvReleaseImage(&gray);
		if(rgb) cvReleaseImage(&rgb);
		if(tmp) cvReleaseImage(&tmp);

		// Create images with new sizes
		rgb = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 4);
		tmp = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 4);
	}
	
    // no need to copy a lot of memory, just point to it...
    rgb->imageData = (char*) image.data;
   	cvWarpPerspective(rgb, tmp, mapMatrix, flags, cvScalar(0));
	memcpy(image.data, tmp->imageData, image.xsize*image.ysize*image.csize);	
}

void pix_opencv_warpperspective :: processRGBImage(imageStruct &image)
{
	// TODO
    error("cant't support RGB image for now");
}

void pix_opencv_warpperspective :: processYUVImage(imageStruct &image)
{
	// TODO
	error( "pix_opencv_warpperspective : yuv format not supported" );
}
    	
void pix_opencv_warpperspective :: processGrayImage(imageStruct &image)
{ 
	if ((this->comp_xsize!=image.xsize)||(this->comp_ysize!=image.ysize)||(!gray)) 
	{
		this->comp_xsize = image.xsize;
		this->comp_ysize = image.ysize;

		//Destroy cv_images to clean memory
		if(gray) cvReleaseImage(&gray);
		if(rgb) cvReleaseImage(&rgb);
		if(tmp) cvReleaseImage(&tmp);

		// Create images with new sizes
		gray = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 1);
		tmp = cvCreateImage(cvSize(image.xsize,image.ysize), IPL_DEPTH_8U, 1);
	}

    // no need to copy a lot of memory, just point to it...
    gray->imageData = (char*) image.data;
    
   	cvWarpPerspective(gray, tmp, mapMatrix, flags, cvScalar(0));
	memcpy(image.data, tmp->imageData, image.xsize*image.ysize);
	
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
		
	// fillin the mapMatrix
	CV_MAT_ELEM( *mapMatrix, float, 0, 0 ) = argv[0].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 1, 0 ) = argv[1].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 2, 0 ) = argv[2].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 0, 1 ) = argv[3].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 1, 1 ) = argv[4].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 2, 1 ) = argv[5].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 0, 2 ) = argv[6].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 1, 2 ) = argv[7].a_w.w_float;
	CV_MAT_ELEM( *mapMatrix, float, 2, 2 ) = argv[8].a_w.w_float;
	
	/*
    int j;
	printf("---mapMatrix---\n");
	for ( j = 0 ; j < 3 ; j++ ){
		for( i=0 ; i<3 ; i++){
			printf("%.2f,\t", CV_MAT_ELEM( *mapMatrix, float, i, j));
		}
		printf("\n");
	}
	*/
}

void pix_opencv_warpperspective :: srcMatrixMess (int argc, t_atom *argv)
{
	int i;
	if ( argc % 2 ) { 
		error("src is should be a list of couple x/y values"); 
		return; 
	}
	if ( argc != dstMatrix->rows * dstMatrix->cols )
	{ 
		error("src matrix should have the same size as dst matrix (which is %d x %d)", dstMatrix->cols, dstMatrix->rows);
		return;
	}
	for ( i = 0; i < argc ; i++) {		
		if (argv[i].a_type != A_FLOAT) { 
				error("src matrix should be float"); 
				return; 
			}
	}

	// fillin the srcMatrix
	for ( i = 0 ; i < dstMatrix->rows ; i++ )
	{
		CV_MAT_ELEM( *srcMatrix, float, i, 0 ) = argv[i*2].a_w.w_float;
		CV_MAT_ELEM( *srcMatrix, float, i, 1 ) = argv[i*2+1].a_w.w_float; // does it work ?
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
	if ( dstMatrix->rows != argc/2 ) {
		// delete and recreate matrix if needed
		cvReleaseMat(&dstMatrix);
		cvReleaseMat(&srcMatrix);
		dstMatrix = cvCreateMat(argc/2,2,CV_32FC1);
		srcMatrix = cvCreateMat(argc/2,2,CV_32FC1);
		cvSet(srcMatrix, cvScalar(0)); // set all to 0.
	}
	// fillin the dstMatrix
	for ( i = 0 ; i < dstMatrix->rows ; i++ )
	{
		CV_MAT_ELEM( *dstMatrix, float, i, 0 ) = argv[i*2].a_w.w_float;
		CV_MAT_ELEM( *dstMatrix, float, i, 1 ) = argv[i*2+1].a_w.w_float; // does it work ?
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
            flags |= cv::WARP_INVERSE_MAP;
        } else {
            flags ^= cv::WARP_INVERSE_MAP;
        }
    }
}

void pix_opencv_warpperspective :: findhomography( )
{
	int i,j;
		if ( srcMatrix->cols != dstMatrix->cols || srcMatrix->rows != dstMatrix->rows ) {
			error("srcMatrix and dstMatrix should have the same size to compute homography !");
			return;
		}
			
		cvFindHomography(srcMatrix, dstMatrix, mapMatrix, findmethod, 0, NULL);
		
		for ( j = 0 ; j < 3 ; j++ ){
			for( i=0 ; i<3 ; i++){
				SETFLOAT(&mapMatrixList[i+j*3], CV_MAT_ELEM( *mapMatrix, float, i, j));
			}
		}
		
		// send out mapMatrix
		outlet_list( m_dataout, 0, 9, mapMatrixList);
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

