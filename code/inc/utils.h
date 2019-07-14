#pragma once
#include <opencv2/opencv.hpp>

enum Alignments{
    MANUAL_ALIGNMENT,
    DEFAULT_ALIGNMENT
};

double pointNorm(const cv::Point2d& p);
double pointDotProduct(const cv::Point2d &u,const cv::Point2d &v);
double twoPointDist(const cv::Point2d &u,const cv::Point2d &v);
double twoPointDist(const cv::Point& p,const cv::Point& q);
double innerRad(const cv::Vec3f& circle);
double innerRad(const std::vector<cv::Point2d>& polygon, const cv::Point2d& centroid);
double outerRad(const cv::Vec3f& circle);
double outerRad(const std::vector<cv::Point2d>& polygon, const cv::Point2d& centroid);

//SYSTEM

void writeConfigFile(bool TIKZ_ENV_FLAG , bool TEX_DOC_FLAG , bool LABEL_FLAG , Alignments ALIGNMENT_MODE , std::pair<float,float> GRID_SIZE , double CORNER_MERGE_THRESHOLD , int LINE_SUPPORT_THRESHOLD);

bool processCLArguments(int argc, char** argv, cv::Mat& img, bool& TIKZ_ENV_FLAG , bool& TEX_DOC_FLAG , bool& LABEL_FLAG , bool& SET_DEFAULT_PARAMS , Alignments& ALIGNMENT_MODE , std::pair<float,float>& GRID_SIZE , double& CORNER_MERGE_THRESHOLD , int& LINE_SUPPORT_THRESHOLD);

void readConfigFile(bool& TIKZ_ENV_FLAG , bool& TEX_DOC_FLAG , bool& LABEL_FLAG , Alignments& ALIGNMENT_MODE , std::pair<float,float>& GRID_SIZE , double& CORNER_MERGE_THRESHOLD , int& LINE_SUPPORT_THRESHOLD);
