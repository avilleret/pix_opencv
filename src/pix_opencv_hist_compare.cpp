#include "pix_opencv_hist_compare.hpp"
#include "pix_opencv_utils.hpp"

CPPEXTERN_NEW(pix_opencv_hist_compare)

/////////////////////////////////////////////////////////
//
// pix_opencv_hist_compare
//
/////////////////////////////////////////////////////////
// Constructor
//
/////////////////////////////////////////////////////////

pix_opencv_hist_compare :: pix_opencv_hist_compare()
{ 
  inlet_new(this->x_obj, &this->x_obj->ob_pd, gensym("float"), gensym("save"));
  m_dataout = outlet_new(this->x_obj, &s_anything);
  m_measureout = outlet_new(this->x_obj, &s_anything);

  save_now = 0;
}

/////////////////////////////////////////////////////////
// Destructor
//
/////////////////////////////////////////////////////////
pix_opencv_hist_compare :: ~pix_opencv_hist_compare()
{
}

/////////////////////////////////////////////////////////
// processImage
//
/////////////////////////////////////////////////////////

void pix_opencv_hist_compare :: processImage(imageStruct &image)
{ 
  auto mat = image2mat(image);

  std::vector<cv::Mat> planes;
  cv::split(mat, planes);

  std::vector<cv::Mat> hist;
  std::vector<int> hist_size;
  cv::calcHist(planes, {1}, 0, hist, {hist_size}, {0});
  cv::equalizeHist(hist, hist);

  // Build the histogram and compute its contents.
  if (save_now>=0) {
    post("saving histogram %d\n",save_now);
    save_now=-1;
    saved_hists.push_back(hist);
  }

  if (!saved_hists.empty()) {
    std::vector<t_atom> datalist;
    datalist.resize(saved_hists.size());

    int nearest = -1;
    double max  =  0;
    int n=0;
    for ( const auto& saved_hist : saved_hists) {
      double val = cv::compareHist(hist, saved_hist, cv::HISTCMP_INTERSECT);
      SETFLOAT(&datalist[n], val);
      if (val>max) {
        max = val;
        nearest = n;
      }
      ++n;
    }

    outlet_float(m_dataout, (float)nearest);
    outlet_list( m_measureout, 0, saved_hists.size() , datalist.data());

  } else {
    outlet_float(m_dataout, -1.0);
  }

  // Create an image to use to visualize our histogram.
  int scale = 10;
  // populate our visualization with little gray squares.
  double max_value = 0;
  cv::minMaxLoc(hist, nullptr, &max_value, nullptr, nullptr);

  int h = 0;
  int s = 0;

  int h_bins = (int)(image.xsize/10), s_bins = (int)(image.ysize/10);
  for( h = 0; h < h_bins; h++ ) {
    for( s = 0; s < s_bins; s++ ) {
      float bin_val = hist[0].at<float>(h, s);
      int intensity = cvRound( bin_val * 255 / max_value );
      cv::rectangle(
            mat,
            cv::Point( h*scale, s*scale ),
            cv::Point( (h+1)*scale - 1, (s+1)*scale - 1),
            CV_RGB(intensity,intensity,intensity), cv::FILLED, 8 , 0 );
    }
  }
  mat2image(mat, image);
}

/////////////////////////////////////////////////////////
// static member function
//
/////////////////////////////////////////////////////////

void pix_opencv_hist_compare :: obj_setupCallback(t_class *classPtr)
{
  class_addmethod(classPtr, (t_method)&pix_opencv_hist_compare::saveMessCallback,
                  gensym("save"), A_FLOAT, A_NULL);
}

void pix_opencv_hist_compare :: saveMess(float index)
{
  if (((int)index>=0)&&((int)index<MAX_HISTOGRAMS_TO_COMPARE)) save_now = (int)index;
}

void pix_opencv_hist_compare :: saveMessCallback(void *data, t_floatarg index)
{
  GetMyClass(data)->saveMess(index);
}
