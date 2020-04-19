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
      break;
    case 3:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC3, image.data);
      break;
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
      break;
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
      break;
    case 3:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC3, image.data);
      cv::cvtColor(mat, gray, cv::COLOR_RGB2GRAY);
      break;
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
      cv::cvtColor(mat, gray, cv::COLOR_RGBA2GRAY);
      break;
    default:
      error("image format not supported");
  }
  return gray;
}

cv::Mat image2mat_hsv(imageStruct& image)
{
  cv::Mat mat, hsv, bgr;
  switch(image.csize)
  {
    case 1:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC3, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_GRAY2BGR);
      cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    case 3:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC3, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_BGR2HSV);
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_BGRA2BGR);
      cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    default:
      error("image format not supported");
  }
  return hsv;
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
      case 3:
      {
        cv::Mat rgb = image2mat(image);
        cv::cvtColor(mat, rgb, cv::COLOR_GRAY2RGB);
        break;
      }
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
