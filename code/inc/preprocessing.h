#pragma once
#include <opencv2/opencv.hpp>

enum BinarizationMode{
    HALF_RANGE,
    OTSU,
    ADAPTIVE,
    GRADIENT_MAGNITUDE
};

void processImg(cv::Mat img ,cv::Mat& dstGray, cv::Mat& dstGrayInv, cv::Mat& dstBin, cv::Mat& dstBinInv);