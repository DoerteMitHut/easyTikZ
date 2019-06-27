#include "utils.h"
#include <limits>

double pointNorm(const cv::Point2d& p)
{
    return cv::sqrt(cv::pow(p.x,2)+cv::pow(p.y,2));
}

double pointDotProduct(const cv::Point2d &u,const cv::Point2d &v)
{
    double s = (u.x*v.x)+(u.y*v.y);
    return s; 
}

double twoPointDist(const cv::Point2d &u,const cv::Point2d &v)
{
    return std::sqrt(std::pow(u.x-v.x,2)+std::pow(u.y-v.y,2));
}

double twoPointDist(const cv::Point& p,const cv::Point& q)
{
    return std::sqrt(std::pow(p.x-q.x,2)+std::pow(p.y-q.y,2));
}

double innerRad(const cv::Vec3f& circle)
{
    //return 80% of the circles radius as the innerRad
    return 0.8*circle[2];
}
double innerRad(const std::vector<cv::Point2d>& polygon,const cv::Point2d& centroid)
{
    //return 80% of the center-distance of the edge center closest to the centroid
    std::vector<cv::Point2d>::const_iterator it;
    double minRad = std::numeric_limits<double>::max();
    for(it = polygon.begin(); it!=polygon.end(); ++it)
    {
        cv::Point2d middle = (*it - *((it+1)==polygon.end() ? polygon.begin() : it+1))/2.;
        if(twoPointDist(middle,centroid)<minRad){
            minRad = twoPointDist(middle,centroid);
        }
    }
    return 0.8*minRad;
}

double outerRad(const cv::Vec3f& circle)
{
    //return 120% of the circles radius as the innerRad
    return 1.2*circle[2];
}
double outerRad(const std::vector<cv::Point2d>& polygon,const cv::Point2d& centroid)
{
    //return 120% of the center distance of the vertex farthest from the centroid
    std::vector<cv::Point2d>::const_iterator it;
    double maxRad = std::numeric_limits<double>::min();
    for(it = polygon.begin(); it!=polygon.end(); ++it)
    {
        if(twoPointDist(*it,centroid)<maxRad){
            maxRad = twoPointDist(*it,centroid);
        }
    }
    return 1.2*maxRad;
}