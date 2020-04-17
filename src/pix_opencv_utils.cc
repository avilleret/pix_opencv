#include "pix_opencv_utils.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <Gem/Image.h>
#include <Base/CPPExtern.h>

cv::Mat image2mat(imageStruct& image)
{
  cv::Mat mat;
  switch(image.csize)
  {
    case 1:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC1, image.data);
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
    default:
      error("image format not supported");
  }
  return mat;
}

cv::Mat image2mat_gray(imageStruct& image)
{
  cv::Mat mat, gray;
  switch(image.csize)
  {
    case 1:
      gray = cv::Mat(image.ysize, image.xsize, CV_8UC1, image.data);
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
      cv::cvtColor(mat, gray, cv::COLOR_RGBA2GRAY);
    default:
      error("image format not supported");
  }
  return gray;
}

void mat2image(const cv::Mat& mat, imageStruct& image)
{
  if(mat.cols != image.xsize || mat.rows != image.ysize)
  {
    error("image should be of the same size");
    return;
  }

  if(image.csize == 1)
  {
    switch(mat.channels())
    {
      case 1:
        image.data = mat.data;
        break;
      case 4:
      {
        cv::Mat rgba = image2mat(image);
        cv::cvtColor(mat, rgba, cv::COLOR_GRAY2RGBA);
        break;
      }
      default:
        error("mat plane count not supported: %d", mat.channels());
    }
  }
}
