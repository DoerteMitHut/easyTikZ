#include "preprocessing.h"
#include <string>

void processImg(cv::Mat img, cv::Mat& dst)
{
    cv::threshold(img,img,128,255,cv::THRESH_BINARY_INV ); 

    if(img.at<int>(0,0))
    {
        cv::subtract(cv::Scalar::all(255),img,img);
    }
    
    std::vector<cv::Point> points;
    cv::Mat_<uchar>::iterator it = dst.begin<uchar>();
    cv::Mat_<uchar>::iterator end = dst.end<uchar>();

    for (;it !=end ;it++)
    {
        if(*it)
        {
            points.push_back(it.pos());
        }
    }

    cv::Rect box = cv::boundingRect(cv::Mat(points));
    std::cout<<box.x<<"|"<<box.y<<" "<<box.width<<"|" <<box.height<<std::endl;

    // box.x = box.x-box.width/8.<0?0:box.x-box.width/8.;
    // box.y = box.y-box.height/8.<0?0:box.y-box.height/8.;
    // box.height = box.y + box.height + box.height/4. >  dst.rows  ?  box.height + box.height/4.  :  dst.rows - box.y - 1; 
    // box.width  = box.x + box.width  + box.width/4.  >  dst.cols  ?  box.width  + box.width/4.   :  dst.cols - box.x - 1;
    
    std::cout<<box.x<<"|"<<box.y<<" "<<box.width<<"|" <<box.height<<std::endl;

    //cv::rectangle(img, box, cv::Scalar(255,0,0));

    dst = cv::Mat(img,box);

    if(box.width >= box.height)
    {
        cv::resize(dst,dst,cv::Size(1000,(int)(1000/box.width)*box.height));
    }
    else
    {
        cv::resize(dst,dst,cv::Size((int)(1000/box.height)*box.width,1000));
    }

    copyMakeBorder( dst, dst, (int)(1000-dst.rows/2), (int)(1000-dst.rows/2), (int)(1000-dst.cols/2), (int)(1000-dst.cols/2), cv::BORDER_CONSTANT,cv::Scalar(0,0,0));
    return;
}