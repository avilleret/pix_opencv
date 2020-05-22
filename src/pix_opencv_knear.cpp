#include "pix_opencv_knear.hpp"
#include "pix_opencv_utils.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

CPPEXTERN_NEW_WITH_TWO_ARGS(pix_opencv_knear, t_symbol *, A_DEFSYM, t_floatarg, A_DEFFLOAT )

/////////////////////////////////////////////////////////
//
// pix_opencv_knear
//
/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
//
// Find the min box. The min box respect original aspect ratio image
// The image is a binary data and background is white.
//
/////////////////////////////////////////////////////////


void pix_opencv_knear :: findX(const cv::Mat& imgSrc,int& min, int& max)
{
  int i;
  int minFound=0;
  cv::Scalar maxVal(imgSrc.cols * 255);
  cv::Scalar val(0);

  // for each col sum, if sum < width*255 then we find the min
  // then continue to end to search the max, if sum< width*255 then is new max
  for (i=0; i< imgSrc.cols; i++)
  {
    auto col = imgSrc.col(i);
    val= cv::sum(col);
    if(val.val[0] < maxVal.val[0])
    {
      max= i;
      if(!minFound)
      {
         min= i;
         minFound= 1;
      }
    }

  }
}

void pix_opencv_knear :: findY(const cv::Mat& imgSrc,int& min, int& max)
{
  int i;
  int minFound=0;
  cv::Scalar maxVal(imgSrc.rows * 255);
  cv::Scalar val(0);

  // for each row sum, if sum < height*255 then we find the min
  // then continue to end to search the max, if sum< width*255 then is new max
  for (i=0; i< imgSrc.rows; i++)
  {
    auto row = imgSrc.row(i);
    val= cv::sum(row);
    if(val.val[0] < maxVal.val[0])
    {
      max=i;
      if(!minFound)
      {
        min= i;
        minFound= 1;
      }
    }
  }
}

/////////////////////////////////////////////////////////
//
// Find the bounding box.
//
//
/////////////////////////////////////////////////////////

cv::Rect pix_opencv_knear :: findBB(const cv::Mat& imgSrc)
{
  int xmin, xmax, ymin, ymax;
  xmin=xmax=ymin=ymax=0;

  this->findX(imgSrc, xmin, xmax);
  this->findY(imgSrc, ymin, ymax);

  cv::Rect bb = cv::Rect(xmin, ymin, xmax-xmin, ymax-ymin);

  if ( ( bb.width == 0 ) || ( bb.height == 0 ) )
  {
     bb.x = 0;
     bb.y = 0;
     bb.width = imgSrc.cols;
     bb.height = imgSrc.rows;
  }

  return bb;
}

cv::Mat pix_opencv_knear :: preprocessing(const cv::Mat& imgSrc,int new_width, int new_height)
{
  cv::Rect bb = findBB(imgSrc);

  // get bounding box data and no with aspect ratio, the x and y can be corrupted
  cv::Mat data = imgSrc(bb);
  // create image with this data with width and height with aspect ratio 1
  // then we get highest size betwen width and height of our bounding box
  int size=(bb.width>bb.height)?bb.width:bb.height;
  cv::Mat result = cv::Mat(size,size,CV_8UC1)*255;
  // copy de data in center of image
  int x=(int)floor((float)(size-bb.width)/2.0f);
  int y=(int)floor((float)(size-bb.height)/2.0f);

  cv::Mat dataA = result(cv::Rect(x,y,bb.width, bb.height));
  data.copyTo(dataA);
  // scale result
  cv::Mat scaledResult;
  cv::resize(result, scaledResult, cv::Size(new_width, new_height), cv::INTER_NEAREST);

  // return processed data
  return scaledResult;
}

void pix_opencv_knear :: load_patterns(void)
{
  cv::Mat prs_image;
  cv::Mat data;
  char file[255];
  int i=0;

  this->x_rsamples = 0;

  for(int j = 0; j< this->x_nsamples; j++)
  {

     // load fileath
     
    if ( x_filepath[0] == '/' ){ // absolute path
      sprintf(file,"%s/%03d.png",this->x_filepath, j);
    } else { // relative path 
      std::string absolutePath = localPath + x_filepath;
      sprintf(file,"%s/%03d.png",absolutePath.c_str(), j);
    }
     cv::Mat src_image = cv::imread(file);
     if(src_image.empty())
     {
       post("pix_opencv_knear : error: couldn't load image %s\n", file);
       continue;
     }
     if ( ( this->x_pwidth == -1 ) || ( this->x_pheight == -1 ) )
     {
        this->x_pwidth = src_image.cols;
        this->x_pheight = src_image.rows;
        // post( "pix_opencv_knear : loaded : %s (%dx%d)", file, src_image->width, src_image->height);
        this->x_rsamples++;
     }
     else if ( ( src_image.cols != this->x_pwidth ) || ( src_image.rows != this->x_pheight ) )
     {
        post( "pix_opencv_knear : error : %s (%dx%d) : wrong size ( should be %dx%d )", file, src_image.cols, src_image.rows, this->x_pwidth, this->x_pheight);
        continue;
     }
     else
     {
        // post( "pix_opencv_knear : loaded : %s (%dx%d)", file, src_image->width, src_image->height);
        this->x_rsamples++;
     }

     // process file
     prs_image = this->preprocessing(src_image, this->x_pwidth, this->x_pheight);
     // post( "pix_opencv_knear : preprocessed : %s (%dx%d)", file, this->x_pwidth, this->x_pheight);

     trainData = cv::Mat(x_nsamples, x_pwidth*x_pheight, CV_32FC1);
     trainClasses = cv::Mat(x_nsamples, 1, CV_32FC1);

     // set class label
     cv::Mat row = trainClasses.row(j);
     row.at<float>(0) = i;

     row = trainData.row(j);

     cv::Mat img(cv::Size( this->x_pwidth, this->x_pheight ), CV_32FC1);
     // convert 8 bits image to 32 float image
     prs_image.convertTo(img, CV_32FC1, 0.0039215, 0);

     data = img(cv::Rect( 0, 0, this->x_pwidth, this->x_pheight));

     // convert data matrix sizexsize to vecor
     cv::Mat row1 = data.row(1);
     row1.copyTo(row);
  }

  // create the classifier
  post( "pix_opencv_knear : loaded : %d samples from %s", this->x_rsamples, this->x_filepath);
  if ( this->x_rsamples == this->x_nsamples )
  {
    x_knn = cv::ml::KNearest::create();
    //x_knn->train(trainData, trainClasses, ...);
    this->knn=new CvKNearest( this->trainData, this->trainClasses, 0, false, this->x_nsamples );
    this->x_nearest=cvCreateMat(1,this->x_nsamples,CV_32FC1);
    this->x_dist=cvCreateMat(1,this->x_nsamples,CV_32FC1);
  }
}


/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////
pix_opencv_knear :: pix_opencv_knear(t_symbol *path, t_floatarg nsamples)
{ 
  m_dataout = outlet_new(this->x_obj, &s_anything);

  comp_xsize  = 320;
  comp_ysize = 240;

  if ( path && path != gensym("") && nsamples > 0. ){
    x_filepath = ( char * ) getbytes( 1024 );
    sprintf( x_filepath, "%s", path->s_name );
    x_nsamples = (int)nsamples;

    x_classify = 0;
    x_pwidth = -1;
    x_pheight = -1;

    trainData = NULL;
    trainClasses = NULL;
    
    t_canvas* canvas =  canvas_getcurrent();
    localPath = std::string(canvas_getdir(canvas)->s_name) + "/";
    
    try {
      this->load_patterns();
    } catch(...) {
        error( "can't load patterns from : %s", path->s_name );
        return;
    }
  }
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_knear :: ~pix_opencv_knear()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////
void pix_opencv_knear :: processRGBAImage(imageStruct &image)
{
 int i;

  if ((this->comp_xsize!=image.xsize)&&(this->comp_ysize!=image.ysize)) 
  {

    this->comp_xsize = image.xsize;
    this->comp_ysize = image.ysize;

    // destroy cv_images to clean memory
    cvReleaseImage( &rgba );
    cvReleaseImage( &rgb );
    cvReleaseImage( &grey );

    // create cv_images 
    this->rgba = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 4 );
    this->rgb = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 3 );
    this->grey = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 1 );

  }

  memcpy( rgba->imageData, image.data, image.xsize*image.ysize*4 );
  cvCvtColor(rgba, grey, CV_BGRA2GRAY);

  if ( this->x_classify )
  {
     IplImage prs_image;
     CvMat row_header, *row1, odata;

       // post( "pix_opencv_knear : size : (%dx%d)", this->x_pwidth, this->x_pheight);

       // process file
       prs_image = this->preprocessing(this->grey, this->x_pwidth, this->x_pheight);

       //Set data
       IplImage* img32 = cvCreateImage( cvSize( this->x_pwidth, this->x_pheight ), IPL_DEPTH_32F, 1 );
       cvConvertScale(&prs_image, img32, 0.0039215, 0);
       cvGetSubRect(img32, &odata, cvRect(0,0, this->x_pwidth, this->x_pheight));
       row1 = cvReshape( &odata, &row_header, 0, 1 );

       this->knn->find_nearest(row1,this->x_nsamples,0,0,this->x_nearest,this->x_dist);
       for ( i=0; i<this->x_nsamples; i++ )
       {
         // post( "pix_opencv_knear : distance : %f", this->x_dist->data.fl[i] );
       }
       outlet_float(this->m_dataout, this->x_dist->data.fl[0]);

       cvReleaseImage( &img32 );
       this->x_classify = 0;
  }
    
  memcpy( image.data, rgba->imageData, image.xsize*image.ysize*4 );
}

void pix_opencv_knear :: processRGBImage(imageStruct &image)
{
 int i;

  if ((this->comp_xsize!=image.xsize)&&(this->comp_ysize!=image.ysize)) 
  {

    this->comp_xsize = image.xsize;
    this->comp_ysize = image.ysize;

    // destroy cv_images to clean memory
    cvReleaseImage( &rgba );
    cvReleaseImage( &rgb );
    cvReleaseImage( &grey );

    // create cv_images 
    this->rgba = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 4 );
    this->rgb = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 3 );
    this->grey = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 1 );

  }

  memcpy( rgb->imageData, image.data, image.xsize*image.ysize*3 );
  cvCvtColor(rgb, grey, CV_BGR2GRAY);

  if ( this->x_classify )
  {
     IplImage prs_image;
     CvMat row_header, *row1, odata;

       // post( "pix_opencv_knear : size : (%dx%d)", this->x_pwidth, this->x_pheight);

       // process file
       prs_image = this->preprocessing(this->grey, this->x_pwidth, this->x_pheight);

       //Set data
       IplImage* img32 = cvCreateImage( cvSize( this->x_pwidth, this->x_pheight ), IPL_DEPTH_32F, 1 );
       cvConvertScale(&prs_image, img32, 0.0039215, 0);
       cvGetSubRect(img32, &odata, cvRect(0,0, this->x_pwidth, this->x_pheight));
       row1 = cvReshape( &odata, &row_header, 0, 1 );

       this->knn->find_nearest(row1,this->x_nsamples,0,0,this->x_nearest,this->x_dist);
       for ( i=0; i<this->x_nsamples; i++ )
       {
         // post( "pix_opencv_knear : distance : %f", this->x_dist->data.fl[i] );
       }
       outlet_float(this->m_dataout, this->x_dist->data.fl[0]);

       cvReleaseImage( &img32 );
       this->x_classify = 0;
  }
    
  memcpy( image.data, rgb->imageData, image.xsize*image.ysize*3 );
}

void pix_opencv_knear :: processYUVImage(imageStruct &image)
{
  post( "pix_opencv_knear : yuv format not supported" );
}
    	
void pix_opencv_knear :: processGrayImage(imageStruct &image)
{ 
 int i;

  if ((this->comp_xsize!=image.xsize)&&(this->comp_ysize!=image.ysize)) 
  {

    this->comp_xsize = image.xsize;
    this->comp_ysize = image.ysize;

    // destroy cv_images to clean memory
    cvReleaseImage( &rgba );
    cvReleaseImage( &rgb );
    cvReleaseImage( &grey );

    // create cv_images 
    this->rgba = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 4 );
    this->rgb = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 3 );
    this->grey = cvCreateImage( cvSize(comp_xsize, comp_ysize), 8, 1 );

  }

  memcpy( grey->imageData, image.data, image.xsize*image.ysize*1 );

  if ( this->x_classify )
  {
     IplImage prs_image;
     CvMat row_header, *row1, odata;

       // post( "pix_opencv_knear : size : (%dx%d)", this->x_pwidth, this->x_pheight);

       // process file
       prs_image = this->preprocessing(this->grey, this->x_pwidth, this->x_pheight);

       //Set data
       IplImage* img32 = cvCreateImage( cvSize( this->x_pwidth, this->x_pheight ), IPL_DEPTH_32F, 1 );
       cvConvertScale(&prs_image, img32, 0.0039215, 0);
       cvGetSubRect(img32, &odata, cvRect(0,0, this->x_pwidth, this->x_pheight));
       row1 = cvReshape( &odata, &row_header, 0, 1 );

       this->knn->find_nearest(row1,this->x_nsamples,0,0,this->x_nearest,this->x_dist);
       for ( i=0; i<this->x_nsamples; i++ )
       {
         // post( "pix_opencv_knear : distance : %f", this->x_dist->data.fl[i] );
       }
       outlet_float(this->m_dataout, this->x_dist->data.fl[0]);

       cvReleaseImage( &img32 );
       this->x_classify = 0;
  }
    
  memcpy( image.data, grey->imageData, image.xsize*image.ysize*1 );
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////
void pix_opencv_knear :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_knear::bangMessCallback,
  		  gensym("bang"), A_NULL);
  class_addmethod(classPtr, (t_method)&pix_opencv_knear::loadMessCallback,
		  gensym("load"), A_SYMBOL, A_DEFFLOAT, A_NULL);
}

void pix_opencv_knear :: bangMessCallback(void *data)
{
   if ( GetMyClass(data)->trainData == NULL )
  {
     ::post( "pix_opencv_knear : no patterns loaded : cannot process" );
     return;
  }
  GetMyClass(data)->x_classify=1;
}

void pix_opencv_knear :: loadMessCallback(void *data, t_symbol *path, t_floatarg nsamples)
{
   if ( (int) nsamples <= 0 )
   {
      ::post( "pix_opencv_knear : wrong number of samples : %d", nsamples );
      return;
   }
   else
   {
      GetMyClass(data)->x_nsamples = (int)nsamples;
      GetMyClass(data)->x_rsamples = 0;
      cvReleaseMat( &GetMyClass(data)->trainData );
      cvReleaseMat( &GetMyClass(data)->trainClasses );
      GetMyClass(data)->trainData = NULL;
      GetMyClass(data)->trainClasses = NULL;
   }
   strcpy( GetMyClass(data)->x_filepath, path->s_name );
   GetMyClass(data)->load_patterns();
}
