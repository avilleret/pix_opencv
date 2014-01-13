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

#include "pix_opencv_matchshape.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>

using namespace std;


CPPEXTERN_NEW(pix_opencv_matchshape)

/////////////////////////////////////////////////////////
//
// pix_opencv_matchshape
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_matchshape :: pix_opencv_matchshape() : m_threshold(2), m_method(CV_CONTOURS_MATCH_I1)
{ 
	m_dataout = outlet_new(this->x_obj, 0);
	m_template_vec_vec.clear();
	m_template_vec_mat.clear();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_matchshape :: ~pix_opencv_matchshape()
{ 
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_matchshape :: processRGBAImage(imageStruct &image)
{ 
	error( "pix_opencv_matchshape : rgba format not supported" );
}

void pix_opencv_matchshape :: processRGBImage(imageStruct &image) {
	error( "pix_opencv_matchshape : rgb format not supported");
}

void pix_opencv_matchshape :: processYUVImage(imageStruct &image) 
{
	error( "pix_opencv_matchshape : yuv format not supported" );
}
    	
void pix_opencv_matchshape :: processGrayImage(imageStruct &image)
{ 
	error( "pix_opencv_matchshape : gray format not supported" );
	error( "pix_opencv_matchshape only works on pd lists" );
	
	// TODO add support to compare grayscale images
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_matchshape :: obj_setupCallback(t_class *classPtr)
{	
	CPPEXTERN_MSG0(classPtr, 	"clear",		clearMess);
	CPPEXTERN_MSG(classPtr, 	"template",		templateMess);
	CPPEXTERN_MSG(classPtr, 	"contour",		contourMess);
	CPPEXTERN_MSG1(classPtr, 	"threshold",	thresholdMess,	float);
	CPPEXTERN_MSG1(classPtr, 	"method",		methodMess,		int);
}

void pix_opencv_matchshape :: clearMess(void)
{
	for ( vector<vector<cv::Point2f> >::iterator it=m_template_vec_vec.end(); it!=m_template_vec_vec.begin(); it--)
	{
		(*it).clear();
	}
	m_template_vec_vec.clear();
	m_template_vec_mat.clear();
	
	t_atom data;
	SETFLOAT(&data, (float)m_template_vec_vec.size());
	outlet_anything(m_dataout, gensym("template_vec"), 1, &data);

	SETFLOAT(&data, (float)m_template_vec_mat.size());
	outlet_anything(m_dataout, gensym("template_mat"), 1, &data);
}
	

void pix_opencv_matchshape :: contourMess(t_symbol*s, int argc, t_atom*argv)
{
	if ( m_template_vec_vec.empty() ){
		error("no template to match, please load a template before matching");
		return;
	}
	
	if ( (argc%2)!=0 ) {
		error("template message argument should be multiple of 2");
		return;
	}
	
	for (int i=0;i<argc;i++){
		if(argv[i].a_type!=A_FLOAT){
			error("template message should have only float arguments");
			return;
		}
	}
	
	vector<cv::Point2f> tmp_contour;
	for (int i=2;i<argc;i+=2){ // start at index 2, first two elements are reserved (for compatibility)
		cv::Point2f pt;
		pt.x = atom_getfloat(argv+i);
		pt.y = atom_getfloat(argv+i+1);
		tmp_contour.push_back(pt);
	}
	
	double comp, min=INFINITY;
	int id(0),i(0);
	for ( vector<vector<cv::Point2f> >::iterator it=m_template_vec_vec.begin(); it!=m_template_vec_vec.end(); ++it){
		comp = cv::matchShapes(tmp_contour, *it, m_method, 0.);
		if ( comp < min ){
			min=comp;
			id=i;
		}
		i++;
	}
	
	if ( min < m_threshold )
	{
		t_atom data[2];
		SETFLOAT(data,id);
		SETFLOAT(data+1, min);
		outlet_anything(m_dataout, gensym("match_vec"), 2, data);
	}
}

void pix_opencv_matchshape :: templateMess(t_symbol*s, int argc, t_atom*argv)
{
	if ( (argc%2)!=0 ) {
		error("template message argument should be multiple of 2");
		return;
	}
	for (int i=0;i<argc;i++){
		if(argv[i].a_type!=A_FLOAT){
			error("template message should have only float arguments");
			return;
		}
	}
	
	vector<cv::Point2f> tmp_contour;
	tmp_contour.clear();
	for (int i=2;i<argc;i+=2){ // start at index 2, first two elements are reserved (for compatibility)
		cv::Point2f pt;
		pt.x = atom_getfloat(argv+i);
		pt.y = atom_getfloat(argv+i+1);
		tmp_contour.push_back(pt);
	}
	m_template_vec_vec.push_back(tmp_contour);
	
	t_atom data;
	SETFLOAT(&data, (int)m_template_vec_vec.size());
	outlet_anything(m_dataout, gensym("template_vec"), 1, &data);
}

void pix_opencv_matchshape :: thresholdMess(float arg)
{
	m_threshold=(double)(arg>0.?arg:0.);
	t_atom data;
	SETFLOAT(&data, m_threshold);
	outlet_anything(m_dataout, gensym("threshold"), 1, &data);
}

void pix_opencv_matchshape :: methodMess(int arg)
{
	switch (arg) {
		case 1:
			m_method=CV_CONTOURS_MATCH_I1;
			break;
		case 2:
			m_method=CV_CONTOURS_MATCH_I2;
			break;
		case 3:
			m_method=CV_CONTOURS_MATCH_I3;
			break;
		default:
			error("method should be 1, 2 or 3");
			m_method=CV_CONTOURS_MATCH_I1;
		}
	t_atom data;
	switch (m_method) {
		case CV_CONTOURS_MATCH_I1:
			SETFLOAT(&data, 1);
			break;
		case CV_CONTOURS_MATCH_I2:
			SETFLOAT(&data, 2);
			break;
		case CV_CONTOURS_MATCH_I3:
			SETFLOAT(&data, 3);
			break;
		}
	outlet_anything(m_dataout, gensym("method"), 1, &data);
}
