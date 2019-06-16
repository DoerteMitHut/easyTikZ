#include <opencv2/opencv.hpp>
void sortLineVector(std::vector<cv::Vec4i>&);
double pointDotProduct(const cv::Point&, const cv::Point&);
double clusterFunction(std::vector<cv::Vec4i>);
double nackenDist(cv::Vec4d, cv::Vec4d,bool,double,double,double);
double otsu(std::vector<double> hist);
void fillShapes(cv::Mat src, cv::Mat& dst);
void findShapes(cv::Mat src, std::vector<std::vector<cv::Point2d>>& shapes, int maxVertCount = 6);
void findShapeCentroids(std::vector<std::vector<cv::Point>> shapes, std::vector<cv::Point2d>& centroids);