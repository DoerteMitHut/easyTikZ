#pragma once
#include <opencv2/opencv.hpp>

double pointNorm(const cv::Point2d& p);
double pointDotProduct(const cv::Point2d &u,const cv::Point2d &v);
double twoPointDist(const cv::Point2d &u,const cv::Point2d &v);
double twoPointDist(const cv::Point& p,const cv::Point& q);
double innerRad(const cv::Vec3f& circle);
double innerRad(const std::vector<cv::Point2d>& polygon, const cv::Point2d& centroid);
double outerRad(const cv::Vec3f& circle);
double outerRad(const std::vector<cv::Point2d>& polygon, const cv::Point2d& centroid);