#pragma once

#include <opencv2/core.hpp>
#include <Gem/Image.h>

cv::Mat image2mat(imageStruct& image);
cv::Mat image2mat_gray(imageStruct& image);
cv::Mat image2mat_hsv(imageStruct& image);
void mat2image(const cv::Mat& mat, imageStruct& image);