#include <opencv2/opencv.hpp>
void sortLineVector(std::vector<cv::Vec4i>&);
double pointDotProduct(const cv::Point&, const cv::Point&);
double clusterFunction(std::vector<cv::Vec4i>);
double nackenDist(cv::Vec4d, cv::Vec4d,bool,double,double,double);
void getShapes(std::vector<cv::Point2f>,std::vector<cv::Vec4i>);
