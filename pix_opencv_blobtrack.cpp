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
													m_FGTrainFrames(0), \
													m_monitoring_stage(0)
{ 
	m_dataout = outlet_new(this->x_obj, 0);
	setupModules();
	createModules();
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_blobtrack :: ~pix_opencv_blobtrack()
{ 	
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: processRGBAImage(imageStruct &image)
{ 
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC4, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
	IplImage img = imgMat;
	printf("imageStruct csize : %d\tIplImage.nbChannels : %d\n",image.csize, img.nChannels);

	RunBlobTrackingAuto( &img );
}

void pix_opencv_blobtrack :: processRGBImage(imageStruct &image) {
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC3, image.data, image.csize*image.xsize); // just transform imageStruct to IplImage without copying data
	IplImage img = imgMat;
	RunBlobTrackingAuto( &img );
}

void pix_opencv_blobtrack :: processYUVImage(imageStruct &image) {
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC2, image.data, image.csize*image.xsize); // convert imageStruct to cv::Mat without copying data
	IplImage img = imgMat; // convert cv::Mat to IplImage
	printf("imageStruct csize : %d\tIplImage.nbChannels : %d\n",image.csize, img.nChannels);

	RunBlobTrackingAuto( &img );}
    	
void pix_opencv_blobtrack :: processGrayImage(imageStruct &image)
{ 
	cv::Mat imgMat( image.ysize, image.xsize, CV_8UC1, image.data, image.csize*image.xsize); // convert imageStruct to cv::Mat without copying data
	IplImage img = imgMat; // convert cv::Mat to IplImage
	RunBlobTrackingAuto( &img );
}

void pix_opencv_blobtrack :: RunBlobTrackingAuto( IplImage* img )
{
        IplImage*   pMask = NULL;
        static IplImage*    imgRGB = NULL;
        
        printf("image depth : %d\n", img->depth);

        imgRGB = cvCreateImage(cvSize(img->width,img->height),img->depth,3);
        switch (img->nChannels){
			case 1:
			cvCvtColor(img, imgRGB, CV_GRAY2RGB);
			break;
			case 2:
			cvCvtColor(img, imgRGB, CV_YUV2RGB);
			break;
			case 3:
			break;
			case 4:
			cvCvtColor(img, imgRGB, CV_RGBA2RGB);
			break;
		}
        /* Process: */
        m_tracker->Process(imgRGB, pMask);
        
        /* Draw debug info: */
        if ( m_monitoring_stage == 3 ) {
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

                cvEllipse( img,
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
                cvPutText( img, str, p, &font, CV_RGB(0,255,255));
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
                            cvPutText( img, pStr, p, &font, CV_RGB(0,255,255));
                            pStr = str_next;
                        }
                        free(pStrFree);
                    }
                }

            }   /* Next blob. */;

          }/* Draw all information about test sequence. */

}   /* RunBlobTrackingAuto */

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: obj_setupCallback(t_class *classPtr)
{
	CPPEXTERN_MSG1(classPtr, "monitorStage",	monitorStageMess, 		t_float);
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

    for(m_BDModule=BlobDetector_Modules; m_BDModule->nickname; ++m_BDModule)
        if( bd_name && MY_STRICMP(bd_name,m_BDModule->nickname)==0 ) break;

    for(m_BTModule=BlobTracker_Modules; m_BTModule->nickname; ++m_BTModule)
        if( bt_name && MY_STRICMP(bt_name,m_BTModule->nickname)==0 ) break;

    for(m_BTPostProcModule=BlobTrackPostProc_Modules; m_BTPostProcModule->nickname; ++m_BTPostProcModule)
        if( btpp_name && MY_STRICMP(btpp_name,m_BTPostProcModule->nickname)==0 ) break;

    for(m_BTAnalysisModule=BlobTrackAnalysis_Modules; m_BTAnalysisModule->nickname; ++m_BTAnalysisModule)
        if( bta_name && MY_STRICMP(bta_name,m_BTAnalysisModule->nickname)==0 ) break;
}
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
}
/////////////////////////////////////////////////////////
// messages handling
//
/////////////////////////////////////////////////////////
void pix_opencv_blobtrack :: monitorStageMess(t_float arg)
{
	m_monitoring_stage = int(arg);
	t_atom data_out;
	SETFLOAT(&data_out, m_monitoring_stage);
	outlet_anything( m_dataout, gensym("monitorStage"), 1, &data_out);
}
