#include "preprocessing.h"
#include <string>

void processImg(cv::Mat img, cv::Mat& dstGray, cv::Mat& dstGrayInv, cv::Mat& dstBin, cv::Mat& dstBinInv)
{
    //preliminary unscaled grayscale image
    cv::Mat imgGray;
    //preliminary unscaled inverted grayscale image
    cv::Mat imgGrayInv;
    //preliminary unscaled binary image
    cv::Mat imgBin;
    //preliminary unscaled inverted binary image
    cv::Mat imgBinInv;

    //convert input image to grayscale
    cv::cvtColor( img    ,imgGray    ,cv::COLOR_BGR2GRAY );
    //equalize grayscale histogram
    cv::equalizeHist( imgGray    ,imgGray    );
    //inverting image if value of edge is above average
    if(imgGray.at<uchar>(0,0)<127)
    {
        cv::subtract( cv::Scalar::all(255)   ,imgGray    ,imgGray );
    }
    //invert grayscale image
    cv::subtract( cv::Scalar::all(255)   ,imgGray    ,imgGrayInv );
    

    ////////// THRESHOLDING ////////////////////////////
    ////////////////////////////////////////////////////
    //TODO: IMPLEMENT OUR THRESHOLDING METHOD

    // suspended work on enabling adaptive thresholding due to high number of artifacts
    // cv::adaptiveThreshold(imgGray,imgBinary,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,9,2);

    //apply Otsu's binarization method to regular and inverted grayscale image
    //TODO: Check, whether thresholding one and and inverting is faster. It probably is.
    cv::threshold( imgGray  ,imgBin    ,128    ,255    ,cv::THRESH_BINARY      | cv::THRESH_OTSU );
    cv::threshold( imgGray  ,imgBinInv ,128    ,255    ,cv::THRESH_BINARY_INV  | cv::THRESH_OTSU ); 

    ////////// CROPPING ////////////////////////////////
    ////////////////////////////////////////////////////

    //store bounding rectangle of all foreground pixels
    //vector of all foreground pixels
    std::vector<cv::Point> foregroundPixels;
    {
        cv::Mat_<uchar>::iterator it    = imgBin.begin<uchar>();
        cv::Mat_<uchar>::iterator end   =   imgBin.end<uchar>();
        for (;it !=end ;it++)
        {
            if(*it)
            {
                foregroundPixels.push_back(it.pos());
            }
        }
    }

    //the bounding rectangle of all foreground pixels
    cv::Rect box = cv::boundingRect(cv::Mat(foregroundPixels));

    //grayscale image to be output
    dstGray      = cv::Mat(imgGray,box);
    //inverted grayscale image to be output
    dstGrayInv   = cv::Mat(imgGrayInv,box);

    ////////// RESIZEING ///////////////////////////////
    ////////////////////////////////////////////////////

    if(box.width >= box.height)
    {
        cv::resize(dstGray    ,dstGray    ,cv::Size(800.,(int)(800.*((double)box.height/(double)box.width))));
        cv::resize(dstGrayInv ,dstGrayInv ,cv::Size(800.,(int)(800.*((double)box.height/(double)box.width))));
    }
    else
    {
        cv::resize( dstGray    ,dstGray      ,cv::Size((int)(800.*((double)box.width/(double)box.height)), 800.));
        cv::resize( dstGrayInv ,dstGrayInv   ,cv::Size((int)(800.*((double)box.width/(double)box.height)), 800.));
    }

    //threshold resized grayscale images
    cv::threshold( dstGray  ,dstBin    ,128    ,255    ,cv::THRESH_BINARY      | cv::THRESH_OTSU );
    cv::threshold( dstGray  ,dstBinInv ,128    ,255    ,cv::THRESH_BINARY_INV  | cv::THRESH_OTSU ); 

    ////////// SQUARING ////////////////////////////////
    ////////////////////////////////////////////////////

    copyMakeBorder( dstGray     ,dstGray    ,(int)std::floor((double)(1000-dstGray.rows)/2)     ,(int)std::ceil((double)(1000-dstGray.rows)/2)      ,(int)std::floor((double)(1000-dstGray.cols)/2)     ,(int)std::ceil((double)(1000-dstGray.cols)/2)      ,cv::BORDER_CONSTANT, cv::Scalar(255,255,255));
    copyMakeBorder( dstGrayInv  ,dstGrayInv ,(int)std::floor((double)(1000-dstGrayInv.rows)/2)  ,(int)std::ceil((double)(1000-dstGrayInv.rows)/2)   ,(int)std::floor((double)(1000-dstGrayInv.cols)/2)  ,(int)std::ceil((double)(1000-dstGrayInv.cols)/2)   ,cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
    copyMakeBorder( dstBin      ,dstBin     ,(int)std::floor((double)(1000-dstBin.rows)/2)      ,(int)std::ceil((double)(1000-dstBin.rows)/2)       ,(int)std::floor((double)(1000-dstBin.cols)/2)      ,(int)std::ceil((double)(1000-dstBin.cols)/2)       ,cv::BORDER_CONSTANT, cv::Scalar(255,255,255));
    copyMakeBorder( dstBinInv   ,dstBinInv  ,(int)std::floor((double)(1000-dstBinInv.rows)/2)   ,(int)std::ceil((double)(1000-dstBinInv.rows)/2)    ,(int)std::floor((double)(1000-dstBinInv.cols)/2)   ,(int)std::ceil((double)(1000-dstBinInv.cols)/2)    ,cv::BORDER_CONSTANT, cv::Scalar(0,0,0));
    
    return;
}