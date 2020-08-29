#include "pix_opencv_utils.hpp"
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
    case 2:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC2, image.data);
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
  cv::Mat gray;
  image2mat_gray(image, gray);
  return gray;
}

void image2mat_gray(imageStruct& image, cv::Mat& gray)
{
  cv::Mat mat;
  switch(image.csize)
  {
    case 1:
      gray = cv::Mat(image.ysize, image.xsize, CV_8UC1, image.data);
      break;
    case 2:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC2, image.data);
      cv::cvtColor(mat, gray, cv::COLOR_YUV2GRAY_420);
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
}

cv::Mat image2mat_bgr(imageStruct& image)
{
  cv::Mat bgr;
  image2mat_bgr(image, bgr);
  return bgr;
}

void image2mat_bgr(imageStruct& image, cv::Mat& bgr)
{
  cv::Mat mat;
  switch(image.csize)
  {
    case 1:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC1, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_GRAY2BGR);
      break;
    case 2:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC2, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_YUV2BGR);
      break;
    case 3:
      bgr = cv::Mat(image.ysize, image.xsize, CV_8UC3, image.data);
      break;
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_BGRA2BGR);
      break;
    default:
      error("image format not supported");
  }
}

cv::Mat image2mat_hsv(imageStruct& image)
{
  cv::Mat hsv;
  image2mat_hsv(image, hsv);
  return hsv;
}

void image2mat_hsv(imageStruct& image, cv::Mat& hsv)
{
  cv::Mat mat, bgr;
  switch(image.csize)
  {
    case 1:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC1, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_GRAY2BGR);
      cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
      break;
    case 3:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC3, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_BGR2HSV);
      break;
    case 4:
      mat = cv::Mat(image.ysize, image.xsize, CV_8UC4, image.data);
      cv::cvtColor(mat, bgr, cv::COLOR_BGRA2BGR);
      cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
      break;
    default:
      error("image csize %d not supported", image.csize);
  }
}

void mat2image(const cv::Mat& mat, imageStruct& image)
{
  if(mat.cols != image.xsize || mat.rows != image.ysize)
  {
    error("image should be of the same size");
    return;
  }

  switch(image.csize)
  {
    case 1:
      switch(mat.channels())
      {
        case 1:
        {
          image.data = mat.data;
          break;
        }
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
      break;
    case 4:
      switch(mat.channels())
      {
        case 1:
        {
          cv::Mat rgba = image2mat(image);
          cv::cvtColor(mat, rgba, cv::COLOR_GRAY2BGRA);
          break;
        }
        case 3:
        {
          cv::Mat rgb = image2mat(image);
          cv::cvtColor(mat, rgb, cv::COLOR_RGB2RGBA);
          break;
        }
        case 4:
        {
          image.data = mat.data;
          break;
        }
        default:
          error("mat plane count not supported: %d", mat.channels());
      }
      break;
    default:
      error("can't convert cv::Mat to image with csize %d", image.csize);
      break;
  }
}
