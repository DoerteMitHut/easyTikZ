#pragma once
#include <opencv2/opencv.hpp>

void processImg(cv::Mat img ,cv::Mat& dstGray, cv::Mat& dstGrayInv, cv::Mat& dstBin, cv::Mat& dstBinInv);