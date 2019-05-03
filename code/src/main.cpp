#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "easytikz-modelling.h"
#include "tikzGenerator.h"
#include <string>

void displayImg(std::string window_name, cv::Mat img){
    cv::namedWindow(window_name);
    cv::imshow(window_name,img);
    cv::waitKey(0); //wait for a key press
    cv::destroyWindow(window_name);
}

int main (int argc, char** argv)
{
    //check whether there is a command line argument given
    if (argc !=2){

		printf("usage: main <Image_Path>\n");
		return -1;
	}
    //if there is a command line argument, use it to set the path for the source image
	cv::Mat img =  cv::imread(argv[1]);
	if(img.empty())
	{
		printf("No image data!\n");
		return -1;
	}

    //converting the original image into grayscale
    cv::Mat imgGrayScale;
    cvtColor(img, imgGrayScale,CV_BGR2GRAY);

    displayImg("Grayscale Image",imgGrayScale);


    //thresholding the grayscale image to get better results
    cv::Mat imgBinary;  
    //adaptiveThreshold(imgGrayScale,imgBinary,255,CV_ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,3,0);
    //apply Otsu's binarization
    cv::threshold(imgGrayScale,imgBinary,128,255,cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    
    displayImg("Binary Image",imgBinary);


    //depth 1 morphological closing
    for(int i = 0; i< 1;i++){
        cv::dilate(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
        displayImg("Dilation #" + std::to_string(i),imgBinary);
    }
    for(int i = 0; i< 1;i++){
        cv::erode(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
        displayImg("Erosion #" + std::to_string(i),imgBinary);
    }

    // vector<Vec4i> lines;
    // HoughLinesP(imgBinary,lines, 10,CV_PI/180,50,30);
    // for (Vec4i line : lines){
    //     cv::line(img,Point(line[0],line[1]),Point(line[2],line[3]),cvScalar(0,0,255),4);
    // }

    // cv::namedWindow("Tracked");
    // cv::imshow("lines",imgBinary);

    // cv::waitKey(0); //wait for a key press

    std::vector<std::vector<cv::Point> > contours; // vector of point vectors to hold the detected contours.

    cv::findContours(imgBinary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE,cv::Point(0,0));
    std::cout<<"#contours = "<<contours.size()<<std::endl;
    int count =0;
    while(contours.size()>100){
        cv::dilate(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
        cv::findContours(imgBinary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE,cv::Point(0,0));
        std::cout<<"#contours2 = "<<contours.size()<<std::endl;
        displayImg("reducing #shapes",imgBinary);
    }
    for(unsigned int i = 0; i<count;i++){
        cv::erode(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(3,3),cv::Point(-1,-1)));
    }
    TikzGenerator gen;
    std::string outString;

    //iterating through each contour
    for(std::vector<cv::Point> contour : contours)
    {
        //obtain a sequence of points of contour, pointed by the variable 'contour'
        std::vector<cv::Point> result;
        cv::approxPolyDP(contour, result , cv::arcLength(contour,true)*0.02, true);
        if(result.size() <= 6){
            //initialize array of CvPoints
            std::vector<cv::Point> pt;
            //iterate over CvSeq-Array results and cast all elements into CvPoints to be inserted into the array
            for(cv::Point p : result){
                pt.insert(pt.end(),p);
            }
            //iterate over the point array and draw lines between consecutive points
            for(int i = 0; i < result.size();i++){
                //second point of line modulo #points to loop back to pt[0]
                cv::line(img, pt[i],pt[ (i+1) % result.size()],cv::Scalar(0,255,0),4);
            }
        }
    }

    cv::namedWindow("Tracked");
    cv::imshow("toll",img);
    cv::waitKey(0); 


    //cleaning up

    return 0;
}