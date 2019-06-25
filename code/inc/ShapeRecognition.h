#pragma once
#include "Edge.h"
#include <utility>
#include <memory>
#include <unordered_map>
#include "Connection.h"
#include <string>
#include <optional>
#include "NodePoint.h"
#include "NodeShape.h"


void sortLineVector(std::vector<cv::Vec4i>&);
double pointDotProduct(const cv::Point&, const cv::Point&);
double clusterFunction(std::vector<cv::Vec4i>);
double nackenDist(cv::Vec4d, cv::Vec4d,bool,double,double,double);
double otsu(std::vector<double> hist);
void fillShapes(cv::Mat src, cv::Mat& dst);
void findShapes(cv::Mat src, std::vector<std::vector<cv::Point2d>>& shapes, int maxVertCount = 6);
void findShapeCentroids(std::vector<std::vector<cv::Point>> shapes, std::vector<cv::Point2d>& centroids);
void connectorImage(cv::Mat srcShapes,cv::Mat srcBin, cv::Mat& dst,int dilationDepth = 4, int dilationShape = cv::MORPH_RECT, cv::Size kernelSize = cv::Size(9,9),cv::Point anchor = cv::Point(-1,-1));
void findCorners(cv::Mat src ,std::vector<cv::Point2d>& mc, double minDist = 20);
void generateEdges( const std::vector<cv::Point2d>& corners, std::vector<cv::Vec4d>& edges);
void computeEdgeSupport(std::vector<cv::Vec4d> lines, std::vector<cv::Vec4d> edgeCandidates, std::vector<double>& dstSupport);
void findIncidentEdges(const std::vector<cv::Point2d>& shape, const std::vector<std::shared_ptr<Edge>>& edges, std::vector<std::pair<Position,std::shared_ptr<Edge>>>& dstEdges);
void linkShapes(const std::vector<std::shared_ptr<NodeShape>>& nodes, const std::vector<std::shared_ptr<Edge>>& edges, std::vector<Connection>& dstConnections);


double pointNorm(const cv::Point2d& p);
double pointDotProduct(const cv::Point2d &u,const cv::Point2d &v);
double twoPointDist(const cv::Point2d &u,const cv::Point2d &v);
double twoPointDist(const cv::Point& p,const cv::Point& q);