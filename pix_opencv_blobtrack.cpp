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
// mainly copy and paste from blobtracker_sample.cpp provided by OpenCV 2.3 SVN rev7875

#include "pix_opencv_blobtrack.h"
#include <stdio.h>
#include <RTE/MessageCallbacks.h>


CPPEXTERN_NEW(pix_opencv_blobtrack)

/////////////////////////////////////////////////////////
//
// pix_opencv_blobtrack
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_blobtrack :: pix_opencv_blobtrack() : 	m_fg_name(FGDetector_Modules[0].nickname), \
													m_bd_name(BlobDetector_Modules[0].nickname), \
													m_bt_name(BlobTracker_Modules[0].nickname), \
													m_btpp_name(BlobTrackPostProc_Modules[0].nickname), \
													m_bta_name(BlobTrackAnalysis_Modules[0].nickname), \
													m_bt_corr("none"), \
													m_FGTrainFrames(6), \
													m_monitoring_stage(0), \
													m_tracker(NULL)
{ 
	m_param.pBT=NULL;
	m_param.pBD=NULL;
	m_param.pBTGen=NULL;
	m_param.pBTA=NULL;
	m_param.pFG=NULL;
	m_dataout = outlet_new(this->x_obj, 0);
	setupModules();
	createModules();
	printParamsMess();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_blobtrack :: ~pix_opencv_blobtrack()
{ 	
	releaseModules();
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: processRGBAImage(imageStruct &image)
{ 
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
	IplImage img = imgMat; // convert cv::Mat to IplImage
	RunBlobTrackingAuto( &img );
}

void pix_opencv_blobtrack :: processRGBImage(imageStruct &image) {
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC3, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
	IplImage img = imgMat;
	RunBlobTrackingAuto( &img );
}

void pix_opencv_blobtrack :: processYUVImage(imageStruct &image) {
	
	error("YVU format not supported");
}
    	
void pix_opencv_blobtrack :: processGrayImage(imageStruct &image)
{ 
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // convert imageStruct to cv::Mat without copying data
	IplImage img = imgMat; // convert cv::Mat to IplImage
	RunBlobTrackingAuto( &img );
}

void pix_opencv_blobtrack :: RunBlobTrackingAuto( IplImage* img )
{
        IplImage*    pMask = NULL;
        IplImage*    imgRGB = NULL;
        
        if ( x_size != img->width || y_size != img->height ){
			releaseModules();
			createModules();
			x_size = img->width;
			y_size = img->height;
		}
        
        imgRGB = cvCreateImage(cvSize(img->width,img->height),img->depth,3);
        switch (img->nChannels){
			case 1:
			cvCvtColor(img, imgRGB, CV_GRAY2RGB);
			break;
			case 4:
			cvCvtColor(img, imgRGB, CV_RGBA2RGB);
			break;
		}
		
        /* Process: */
        m_tracker->Process(imgRGB, pMask);
		
		//~ t_atom blob_num;
		//~ SETFLOAT(&blob_num, m_tracker->GetBlobNum());
		//~ outlet_anything(m_dataout, gensym("blobnum"), 1, &blob_num);
		
		int blob_atom_size = 2+m_tracker->GetBlobNum()*6;
		
		t_atom* blob_atom = new t_atom[blob_atom_size];
		SETFLOAT(&blob_atom[0], m_tracker->GetBlobNum());
		SETFLOAT(&blob_atom[1], 6);
		
        for(int i=0; i<m_tracker->GetBlobNum(); i++){
			CvBlob* blob = m_tracker->GetBlob(i);
			SETFLOAT(&blob_atom[2+i*6], blob->ID);
			SETFLOAT(&blob_atom[3+i*6], blob->x/img->width);
			SETFLOAT(&blob_atom[4+i*6], blob->y/img->height);
			SETFLOAT(&blob_atom[5+i*6], blob->w/img->width);
			SETFLOAT(&blob_atom[6+i*6], blob->h/img->height);
			SETFLOAT(&blob_atom[7+i*6], m_tracker->GetState(blob->ID));
			//~ t_atom *ap=blob_atom+2+6*i;
			//~ SETFLOAT(ap++, blob->ID);
			//~ SETFLOAT(ap++, blob->x/img->width);
			//~ SETFLOAT(ap++, blob->y/img->height);
			//~ SETFLOAT(ap++, blob->w/img->width);
			//~ SETFLOAT(ap++, blob->h/img->height);
			//~ SETFLOAT(ap++, m_tracker->GetState(blob->ID));
		}
		
		outlet_anything(m_dataout, gensym("blob"), blob_atom_size, blob_atom);
		
		if(blob_atom) delete blob_atom;
		blob_atom = NULL;
		
		if ( m_monitoring_stage == 1 ) { // show foreground
			IplImage*           fg = m_tracker->GetFGMask();
			switch (img->nChannels){
				case 1:
				cvCopy(fg, img);
				break;
				case 4:
				cvCvtColor(fg, img, CV_GRAY2RGBA);
				break;
			}
		}
		
        if ( m_monitoring_stage == 2 ) {
           /* Draw all information about test sequence: */
            char        str[1024];
            int         line_type = CV_AA;   // Change it to 8 to see non-antialiased graphics.
            CvFont      font;
            int         i;

            cvInitFont( &font, CV_FONT_HERSHEY_PLAIN, 0.7, 0.7, 0, 1, line_type );

            for(i=m_tracker->GetBlobNum(); i>0; i--)
            {
                CvSize  TextSize;
                CvBlob* pB = m_tracker->GetBlob(i-1);
                CvPoint p = cvPoint(cvRound(pB->x*256),cvRound(pB->y*256));
                CvSize  s = cvSize(MAX(1,cvRound(CV_BLOB_RX(pB)*256)), MAX(1,cvRound(CV_BLOB_RY(pB)*256)));
                int c = cvRound(255*m_tracker->GetState(CV_BLOB_ID(pB)));

                cvEllipse( imgRGB,
                    p,
                    s,
                    0, 0, 360,
                    CV_RGB(c,255-c,0), cvRound(1+(3*0)/255), CV_AA, 8 );

                p.x >>= 8;
                p.y >>= 8;
                s.width >>= 8;
                s.height >>= 8;
                sprintf(str,"%03d",CV_BLOB_ID(pB));
                cvGetTextSize( str, &font, &TextSize, NULL );
                p.y -= s.height;
                cvPutText( imgRGB, str, p, &font, CV_RGB(0,255,255));
                {
                    const char* pS = m_tracker->GetStateDesc(CV_BLOB_ID(pB));

                    if(pS)
                    {
                        char* pStr = strdup(pS);
                        char* pStrFree = pStr;

                        while (pStr && strlen(pStr) > 0)
                        {
                            char* str_next = strchr(pStr,'\n');

                            if(str_next)
                            {
                                str_next[0] = 0;
                                str_next++;
                            }

                            p.y += TextSize.height+1;
                            cvPutText( imgRGB, pStr, p, &font, CV_RGB(0,255,255));
                            pStr = str_next;
                        }
                        free(pStrFree);
                    }
                }

            }   /* Next blob. */;
			switch (img->nChannels){
				case 1:
				cvCvtColor(imgRGB, img, CV_RGB2GRAY);
				break;
				case 4:
				cvCvtColor(imgRGB, img, CV_RGB2RGBA);
				break;
			}

          }/* Draw all information about test sequence. */
          cvReleaseImage(&imgRGB);
		  imgRGB = NULL;

}   /* RunBlobTrackingAuto */

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: obj_setupCallback(t_class *classPtr)
{
	CPPEXTERN_MSG1(classPtr, "monitorStage",	monitorStageMess, 		int);
	CPPEXTERN_MSG1(classPtr, "fgTrainFrames",	fgTrainFramesMess, 		int);
	CPPEXTERN_MSG (classPtr, "getParam", 		getParamMess);
	CPPEXTERN_MSG (classPtr, "setParam", 		setParamMess);
	CPPEXTERN_MSG (classPtr, "getModule", 		getModuleMess);
	CPPEXTERN_MSG (classPtr, "setModule", 		setModuleMess);
}

/////////////////////////////////////////////////////////
// private function
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: setupModules()
{
	const char* fg_name = m_fg_name.c_str();
	const char* bd_name = m_bd_name.c_str();
	const char* bt_name = m_bt_name.c_str();
	const char* btpp_name = m_btpp_name.c_str();
	const char* bta_name = m_bta_name.c_str();
	
    for(m_FGModule=FGDetector_Modules; m_FGModule->nickname; ++m_FGModule)
        if( fg_name && MY_STRICMP(fg_name,m_FGModule->nickname)==0 ) break;
    if ( m_FGModule->nickname == NULL ){
		error("FG Module %s doesn't exist, swith to default one", fg_name);
		m_FGModule=FGDetector_Modules;
	}

    for(m_BDModule=BlobDetector_Modules; m_BDModule->nickname; ++m_BDModule)
        if( bd_name && MY_STRICMP(bd_name,m_BDModule->nickname)==0 ) break;
    if ( m_BDModule->nickname == NULL ){
		error("BD Module %s doesn't exist, swith to default one", bd_name);
		m_BDModule=BlobDetector_Modules;
	}

    for(m_BTModule=BlobTracker_Modules; m_BTModule->nickname; ++m_BTModule)
        if( bt_name && MY_STRICMP(bt_name,m_BTModule->nickname)==0 ) break;
    if ( m_BTModule->nickname == NULL ){
		error("BT Module %s doesn't exist, swith to default one", bt_name);
		m_BTModule=BlobTracker_Modules;
	}

    for(m_BTPostProcModule=BlobTrackPostProc_Modules; m_BTPostProcModule->nickname; ++m_BTPostProcModule)
        if( btpp_name && MY_STRICMP(btpp_name,m_BTPostProcModule->nickname)==0 ) break;
    if ( m_BTPostProcModule->nickname == NULL ){
		error("BTPP Module %s doesn't exist, swith to default one", btpp_name);
		m_BTPostProcModule=BlobTrackPostProc_Modules;
	}

    for(m_BTAnalysisModule=BlobTrackAnalysis_Modules; m_BTAnalysisModule->nickname; ++m_BTAnalysisModule)
        if( bta_name && MY_STRICMP(bta_name,m_BTAnalysisModule->nickname)==0 ) break;
    if ( m_BTAnalysisModule->nickname == NULL ){
		error("BTA Module %s doesn't exist, swith to default one", bta_name);
		m_BTAnalysisModule=BlobTrackAnalysis_Modules;
	}
} /* setupModules */

void pix_opencv_blobtrack :: createModules()
{   /* Create autotracker module and its components: */

        m_param.FGTrainFrames = m_FGTrainFrames;
        /* Create FG Detection module: */
        m_param.pFG = m_FGModule->create();
        if(!m_param.pFG){
            error("Can not create FGDetector module");
            return;
		}
        m_param.pFG->SetNickName(m_FGModule->nickname);
        //~ set_params(argc, argv, m_param.pFG, "fg", m_FGModule->nickname);
        /* Create Blob Entrance Detection module: */
        m_param.pBD = m_BDModule->create();
        if(!m_param.pBD){
            error("Can not create BlobDetector module");
            return;
		}
        m_param.pBD->SetNickName(m_BDModule->nickname);
        //~ set_params(argc, argv, m_param.pBD, "bd", m_BDModule->nickname);

        /* Create blob tracker module: */
        m_param.pBT = m_BTModule->create();
        if(!m_param.pBT){
            error("Can not create BlobTracker module");
		}
        m_param.pBT->SetNickName(m_BTModule->nickname);
        //~ set_params(argc, argv, m_param.pBT, "bt", m_BTModule->nickname);

        /* Create blob trajectory generation module: */
        /*
        m_param.pBTGen = NULL;
        if(m_BTGenModule && track_name && m_BTGenModule->create)
        {
            m_param.pBTGen = m_BTGenModule->create();
            m_param.pBTGen->SetFileName(track_name);
        }
        if(m_param.pBTGen)
        {
            m_param.pBTGen->SetNickName(m_BTGenModule->nickname);
            //~ set_params(argc, argv, m_param.pBTGen, "btgen", m_BTGenModule->nickname);
        }
        */

        /* Create blob trajectory post processing module: */
        m_param.pBTPP = NULL;
        if(m_BTPostProcModule && m_BTPostProcModule->create)
        {
            m_param.pBTPP = m_BTPostProcModule->create();
        }
        if(m_param.pBTPP)
        {
            m_param.pBTPP->SetNickName(m_BTPostProcModule->nickname);
            //~ set_params(argc, argv, m_param.pBTPP, "btpp", pBTPostProcModule->nickname);
        }
        
        const char * bt_corr = m_bt_corr.c_str();

        m_param.UsePPData = (bt_corr && MY_STRICMP(bt_corr,"PostProcRes")==0);

        /* Create blob trajectory analysis module: */
        m_param.pBTA = NULL;
        if(m_BTAnalysisModule && m_BTAnalysisModule->create)
        {
            m_param.pBTA = m_BTAnalysisModule->create();
            //~ m_param.pBTA->SetFileName(bta_data_name);
        }
        if(m_param.pBTA)
        {
            m_param.pBTA->SetNickName(m_BTAnalysisModule->nickname);
            //~ set_params(argc, argv, m_param.pBTA, "bta", m_BTAnalysisModule->nickname);
        }

        /* Create whole pipline: */
        m_tracker = cvCreateBlobTrackerAuto1(&m_param);
        if(!m_tracker)
            error("Can not create BlobTrackerAuto");
} /* createModules */

void pix_opencv_blobtrack :: releaseModules(void)
{
	if(m_param.pBT)cvReleaseBlobTracker(&m_param.pBT);
	if(m_param.pBD)cvReleaseBlobDetector(&m_param.pBD);
	if(m_param.pBTGen)cvReleaseBlobTrackGen(&m_param.pBTGen);
	if(m_param.pBTA)cvReleaseBlobTrackAnalysis(&m_param.pBTA);
	if(m_param.pFG)cvReleaseFGDetector(&m_param.pFG);
	if(m_tracker)cvReleaseBlobTrackerAuto(&m_tracker);
} /* releaseModules */

void pix_opencv_blobtrack :: print_params(CvVSModule* pM, const char* module)
{
    int i;
    if(pM->GetParamName(0) == NULL ) return;


    post("%s(%s) module parameters:",module,pM->GetNickName());

    for (i=0; ; ++i)
    {
        const char*   param = pM->GetParamName(i);
        const char*   str = param?pM->GetParamStr(param):NULL;
        if(param == NULL)break;
        if(str)
        {
            post("  %s: %s",param,str);
        }
        else
        {
            post("  %s: %g",param,pM->GetParam(param));
        }
    }
}   /* print_params */

/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: monitorStageMess(int arg)
{
	m_monitoring_stage = int(arg);
	t_atom data_out;
	SETFLOAT(&data_out, m_monitoring_stage);
	outlet_anything( m_dataout, gensym("monitorStage"), 1, &data_out);
}

void pix_opencv_blobtrack :: fgTrainFramesMess(int arg)
{
	//~ m_FGTrainFrames = int(arg);
	t_atom data_out;
	SETFLOAT(&data_out, m_FGTrainFrames);
	outlet_anything( m_dataout, gensym("FGTrainFrames"), 1, &data_out);
}

void pix_opencv_blobtrack :: printParamsMess(void)
{   /* Print module parameters: */
        struct DefMMM
        {
            CvVSModule* pM;
            const char* name;
        } Modules[] = {
            {(CvVSModule*)m_param.pFG,"FGdetector"},
            {(CvVSModule*)m_param.pBD,"BlobDetector"},
            {(CvVSModule*)m_param.pBT,"BlobTracker"},
            {(CvVSModule*)m_param.pBTGen,"TrackGen"},
            {(CvVSModule*)m_param.pBTPP,"PostProcessing"},
            {(CvVSModule*)m_param.pBTA,"TrackAnalysis"},
            {NULL,NULL}
        };
        int     i;
        for(i=0; Modules[i].name; ++i)
        {
            if(Modules[i].pM)
                print_params(Modules[i].pM,Modules[i].name);
        }
}   /* Print module parameters. */

void pix_opencv_blobtrack :: getParamMess(t_symbol*s, int argc, t_atom*argv)
{
	// TODO
}

void pix_opencv_blobtrack :: setParamMess(t_symbol*s, int argc, t_atom*argv)
{
	// TODO
}

void pix_opencv_blobtrack :: getModuleMess(t_symbol*s, int argc, t_atom*argv)
{
	if ( argc == 0 ){ // no args : print all modules in pipeline
		post("avaible modules in pipeline :\n \
		fg : ForeGround detector\n \
		bd : Blob Detector\n \
		bt : Blob Tracker\n \
		btpp : Blob Tracker Post Processing\n \
		bta : Blob Tracker Analysis");
		
		t_atom module_list[5];
		SETSYMBOL(&module_list[0], gensym("fg"));
		SETSYMBOL(&module_list[1], gensym("bd"));
		SETSYMBOL(&module_list[2], gensym("bt"));
		SETSYMBOL(&module_list[3], gensym("btpp"));
		SETSYMBOL(&module_list[4], gensym("bta"));
		outlet_anything(m_dataout, gensym("modulelist"), 5, module_list);
		
	} else if ( argc == 1 ) { // one arg : print available algo for specified module
		if ( argv[0].a_type == A_SYMBOL ) {
			t_atom algo_list[512];
			int i;
			if ( std::string(argv[0].a_w.w_symbol->s_name) == "fg" ){
				post("available foreground detector algo :");
				for (i = 0 ; FGDetector_Modules[i].nickname!=NULL ; i++){
					post("\t%s : %s", FGDetector_Modules[i].nickname, FGDetector_Modules[i].description);
					SETSYMBOL(&algo_list[i],gensym(FGDetector_Modules[i].nickname));
				}
				outlet_anything(m_dataout, gensym("fg_algo"), i, algo_list);
			} else if ( std::string(argv[0].a_w.w_symbol->s_name) == "bd" ){
				post("available blob detector algo :");
				for (i = 0 ; BlobDetector_Modules[i].nickname!=NULL ; i++){
					post("\t%s : %s", BlobDetector_Modules[i].nickname, BlobDetector_Modules[i].description);
					SETSYMBOL(&algo_list[i],gensym(BlobDetector_Modules[i].nickname));
				}
				outlet_anything(m_dataout, gensym("bd_algo"), i, algo_list);
			} else if ( std::string(argv[0].a_w.w_symbol->s_name) == "bt" ){
				post("available blob tracker algo :");
				for (i = 0 ; BlobTracker_Modules[i].nickname!=NULL ; i++){
					post("\t%s : %s", BlobTracker_Modules[i].nickname, BlobTracker_Modules[i].description);
					SETSYMBOL(&algo_list[i],gensym(BlobTracker_Modules[i].nickname));
				}
				outlet_anything(m_dataout, gensym("bt_algo"), i, algo_list);
			} else if ( std::string(argv[0].a_w.w_symbol->s_name) == "btpp" ){
				post("available blob tracker post processing algo :");
				for (i = 0 ; BlobTrackPostProc_Modules[i].nickname!=NULL ; i++){
					post("\t%s : %s", BlobTrackPostProc_Modules[i].nickname, BlobTrackPostProc_Modules[i].description);
					SETSYMBOL(&algo_list[i],gensym(BlobTrackPostProc_Modules[i].nickname));
				}
				outlet_anything(m_dataout, gensym("btpp_algo"), i, algo_list);
			} else if ( std::string(argv[0].a_w.w_symbol->s_name) == "bta" ){
				post("available blob tracker analysis algo :");
				for (i = 0 ; BlobTrackAnalysis_Modules[i].nickname!=NULL ; i++){
					post("\t%s : %s", BlobTrackAnalysis_Modules[i].nickname, BlobTrackAnalysis_Modules[i].description);
					SETSYMBOL(&algo_list[i],gensym(BlobTrackAnalysis_Modules[i].nickname));
				}
				outlet_anything(m_dataout, gensym("bd_algo"), i, algo_list);
			}		
		} else error("getModules <symbol> [<symbol>]");
	} else error("getModules <symbol> [<symbol>] : need 1 or 2 args");
}

void pix_opencv_blobtrack :: setModuleMess(t_symbol*s, int argc, t_atom*argv)
{
	if ( argc != 2 ){
		error("use : setModule <module> <module_name>");
		error("try getModule to get available module list");
		return;
	}
	
	if ( argv[0].a_type == A_SYMBOL ) {
		if (std::string(argv[0].a_w.w_symbol->s_name) == "fg") {
			m_fg_name = argv[1].a_w.w_symbol->s_name;
		}
		else if (std::string(argv[0].a_w.w_symbol->s_name) == "bd") {
			m_bd_name = argv[1].a_w.w_symbol->s_name;
		}
		else if (std::string(argv[0].a_w.w_symbol->s_name) == "bt") {
			m_bt_name = argv[1].a_w.w_symbol->s_name;
		}
		else if (std::string(argv[0].a_w.w_symbol->s_name) == "btpp") {
			m_btpp_name = argv[1].a_w.w_symbol->s_name;
		}
		else if (std::string(argv[0].a_w.w_symbol->s_name) == "bta") {
			m_bta_name = argv[1].a_w.w_symbol->s_name;
		}
		releaseModules();
		setupModules();
		createModules();
		printParamsMess();
	}
}
