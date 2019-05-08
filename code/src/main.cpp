#include <iostream>
#include <opencv2/opencv.hpp>
#include "Diagram.h"
#include "tikzGenerator.h"

void displayImg(std::string window_name, cv::Mat img){
    cv::namedWindow(window_name);
    cv::imshow(window_name,img);
    cv::waitKey(0); //wait for a key press
    cv::destroyWindow(window_name);
}

//computes Nacken's metric for line segments
double nackenDist(cv::Vec4i s1,cv::Vec4i s2, bool consider_translation)
{
    cv::Point p_1_1,p_1_2,p_2_1,p_2_2;
    // making sure both lines consider their (x-component wise) endpoint to be the first one
    if(s1[0]>s1[2])
    {
        p_1_1 = cv::Point(s1[2],s1[3]);
        p_1_2 = cv::Point(s1[0],s1[1]);
    }
    else
    {
        p_1_2 = cv::Point(s1[2],s1[3]);
        p_1_1 = cv::Point(s1[0],s1[1]);
    }
    if(s2[0]>s2[2])
    {
        p_2_1 = cv::Point(s2[2],s2[3]);
        p_2_2 = cv::Point(s2[0],s2[1]);
    }
    else
    {
        p_2_2 = cv::Point(s2[2],s2[3]);
        p_2_1 = cv::Point(s2[0],s2[1]);
    }

    //calculating vectors
    cv::Point v_1 = p_1_2-p_1_1;
    cv::Point v_2 = p_2_2-p_2_1;
    
    //calculating line lengths
    double l_1,l_2;
    l_1 = cv::sqrt(v_1.x*v_1.x+v_1.y*v_1.y);
    l_2 = cv::sqrt(v_2.x*v_2.x+v_2.y*v_2.y); 

    std::cout << "l1: " << l_1 << std::endl << "l2: " << l_2 << std::endl;


    
    //calculating angles in rad
    double phi_1, phi_2, delta_phi;
    phi_1 = (double)cv::fastAtan2(v_1.y,v_1.x);
    phi_2 = (double)cv::fastAtan2(v_2.y,v_2.x);
    delta_phi = phi_2-phi_1;
    std::cout << "phi_1: " << phi_1 << std::endl << "phi_2: " << phi_2 << std::endl;


    //calculating standard deviations
    //sigma_l is not needed for the method proposed by other people
    double sigma_d, sigma_w, sigma_l;
    sigma_d = (3.*CV_PI/2.)/cv::sqrt(30*l_2);
    sigma_w = 5.;
    sigma_l = 2.*cv::sqrt(30.*l_2);
    // computing Nacken's metric for line segments
    // rotational component
    double rot_comp = (1./(cv::pow(CV_PI*cv::pow(sigma_d,2),1./4.))*cv::exp(-((phi_2-phi_1)/cv::pow(sigma_d,2.))));
    // translational component
    // calculating the centers of the lines
    cv::Point c_1,c_2,c_2_1,c_2_1_r;
    c_1 = p_1_1+v_1/2.;
    c_2 = p_2_1+v_2/2.;
    //difference between centers
    c_2_1 = c_1-c_2;
    std::cout<<"center distance: "<<cv::sqrt(cv::pow(c_2_1.x,2.)+cv::pow(c_2_1.y,2.)) << std::endl;
    // rotating everything to align s_2 with the x-axis
    c_2_1_r = cv::Point(c_1.x*cos(-phi_2)-c_1.y*sin(-phi_2),c_1.x*sin(-phi_2)+c_1.y*cos(-phi_2));
    double disp_comp = consider_translation ?
    (1/(cv::sqrt(CV_PI*cv::pow(sigma_l,2.)*cv::pow(sigma_w,2.))))*cv::exp(-cv::pow((c_2_1_r.x)/sigma_l,2.) - cv::pow((c_2_1_r.y)/sigma_w,2.))
    :(1/(cv::sqrt(CV_PI*cv::pow(sigma_w,2.))))*cv::exp(-((c_2_1_r.y)/cv::pow(sigma_w,2.)));
    
    
    std::cout << "r: " << rot_comp << std::endl << "t: " << disp_comp << std::endl;
    return rot_comp*disp_comp;
}

int main (int argc, char** argv)
{
    //check whether there is a command line argument given
    if (argc !=2){
		std::cout<<"u   sage: main <Image_Path>\n";
		return -1;
	}
    //if there is a command line argument, use it to set the path for the source image
	cv::Mat img =  cv::imread(argv[1]);
	if(img.empty())
	{
		std::cout<<"No image data!\n";
		return -1;
	}

    //converting the original image into grayscale
    cv::Mat imgGrayScale;
    cv::cvtColor(img, imgGrayScale,cv::COLOR_BGR2GRAY);

    displayImg("Grayscale Image",imgGrayScale);


    //thresholding the grayscale image to get better results
    // cv::Mat imgBinary;  
    // cv::equalizeHist(imgGrayScale,imgGrayScale);
    // displayImg("normalized Histogram", imgGrayScale);
    // cv::subtract(cv::Scalar::all(255),imgGrayScale,imgGrayScale);
    // displayImg("inverted grayscale",imgGrayScale);
    // cv::adaptiveThreshold(imgGrayScale,imgBinary,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,9,2);
    // //apply Otsu's binarization
    // //cv::threshold(imgGrayScale,imgBinary,128,255,cv::THRESH_BINARY_INV | cv::THRESH_OTSU); 
    // displayImg("Binary Image",imgBinary);


    // //depth 1 morphological closing
    // unsigned int closingDepth = 1;
    // for(int i = 0; i< closingDepth;i++){
    //     cv::dilate(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    //     displayImg("Dilation #" + std::to_string(i),imgBinary);
    // }
    // for(int i = 0; i< closingDepth;i++){
    //     cv::erode(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    //     displayImg("Erosion #" + std::to_string(i),imgBinary);
    // }

    // cv:: Mat houghBinary = cv::Mat(imgBinary);
    // std::vector<cv::Vec4i> lines;
    // cv::HoughLinesP(houghBinary,lines, 10,CV_PI/180,30,30);
    // for (cv::Vec4i line : lines){
    //     cv::line(houghBinary,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(0,0,255),4);
    // }
    // displayImg("Probabilistic Hough on Binary",houghBinary);

    // houghBinary = cv::Mat(imgBinary);
    // lines.clear();
    // cv::HoughLines(houghBinary,lines, 10,CV_PI/180,30,30);
    // for (cv::Vec4i line : lines){
    //     cv::line(houghBinary,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(0,0,255),4);
    // }
    // displayImg("Non-Probabilistic Hough on Binary",houghBinary);

    // cv:: Mat houghImg = cv::Mat(imgGrayScale);
    // cv::dilate(houghImg,houghImg,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    // displayImg("dilated grayscale",houghImg);
    // //cv::Canny(houghImg,houghImg,50,200,3);
    // lines.clear();
    // cv::HoughLinesP(houghImg,lines, .001,CV_PI/360,3);
    // for (cv::Vec4i line : lines){
    //     cv::line(houghImg,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(0,0,255),4);
    // }
    // displayImg("Hough on img",houghImg);

    // houghImg = cv::Mat(imgGrayScale);
    // cv::dilate(houghImg,houghImg,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    // displayImg("dilated grayscale",houghImg);
    // //cv::Canny(houghImg,houghImg,50,200,3);
    // lines.clear();
    // cv::HoughLinesP(houghImg,lines, .001,CV_PI/360,3);
    // for (cv::Vec4i line : lines){
    //     cv::line(houghImg,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(0,0,255),4);
    // }
    // displayImg("Hough on img",houghImg);



    // std::vector<std::vector<cv::Point> > contours; // vector of point vectors to hold the detected contours.

    // cv::findContours(imgBinary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE,cv::Point(0,0));
    // std::cout<<"#contours = "<<contours.size()<<std::endl;
    // unsigned int count =0;
    // while(contours.size()>100){
    //     cv::dilate(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    //     cv::findContours(imgBinary, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE,cv::Point(0,0));
    //     std::cout<<"#contours2 = "<<contours.size()<<std::endl;
    //     displayImg("reducing #shapes",imgBinary);
    // }
    // for(unsigned int i = 0; i<count;i++){
    //     cv::erode(imgBinary,imgBinary,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(3,3),cv::Point(-1,-1)));
    // }
    // TikzGenerator gen;
    // std::string outString;

    // //iterating through each contour
    // for(std::vector<cv::Point> contour : contours)
    // {
    //     //obtain a sequence of points of contour, pointed by the variable 'contour'
    //     std::vector<cv::Point> result;
    //     cv::approxPolyDP(contour, result , cv::arcLength(contour,true)*0.02, true);
    //     if(result.size() <= 6){
    //         //initialize array of CvPoints
    //         std::vector<cv::Point> pt;
    //         //iterate over CvSeq-Array results and cast all elements into CvPoints to be inserted into the array
    //         for(cv::Point p : result){
    //             pt.insert(pt.end(),p);
    //         }
    //         //iterate over the point array and draw lines between consecutive points
    //         for(unsigned int i = 0; i < result.size();i++){
    //             //second point of line modulo #points to loop back to pt[0]
    //             cv::line(img, pt[i],pt[ (i+1) % result.size()],cv::Scalar(0,255,0),4);
    //         }
    //     }
    // }

    // displayImg("finished",img);
    



    std::vector<cv::Vec4i> lines;
    cv::Mat houghImg;
    houghImg = cv::Mat(img);
    cv::cvtColor(houghImg,houghImg,cv::COLOR_BGR2GRAY);
    cv::subtract(cv::Scalar::all(255),houghImg,houghImg);
    //cv::dilate(houghImg,houghImg,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    //cv::Canny(houghImg,houghImg,50,200,3);
    displayImg("dilated grayscale",houghImg);
    lines.clear();
    cv::HoughLinesP(houghImg,lines, 1,CV_PI/180,10,60,10);
    cv::cvtColor(houghImg,houghImg,cv::COLOR_GRAY2BGR);
    std::cout<<"detected "<<lines.size()<<" lines"<<std::endl;
    cv::Vec4i motherLine = lines[0];
    for (cv::Vec4i line : lines){
        float sum = cv::abs(line[2]-line[0])+cv::abs(line[3]-line[1]);
        //double nacken = nackenDist(line,motherLine,false);
        //std::cout << nacken<<std::endl;
        cv::line(houghImg,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(255*cv::abs(line[2]-line[0])/sum,0,255*cv::abs(line[3]-line[1])/sum),4);
        //cv::line(houghImg,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(255*nacken,0,255*1.-nacken),4);
    }
    //cv::line(houghImg,cv::Point(motherLine[0],motherLine[1]),cv::Point(motherLine[2],motherLine[3]),cv::Scalar(0,255,0),4);
    displayImg("Hough on img",houghImg);

    nackenDist(cv::Vec4i(0,0,1,0),cv::Vec4i(0,0,1,0),true);
    // displayImg("",img);
    // bool running = true;
    // std::string sequence = "";
    // std::string input;
    // cv::Mat gray;
    // while(running)
    // {   input = "";
    //     std::cin >> input;
    //     if(input == "gray"){
    //         cv::cvtColor(img,img,cv::COLOR_BGR2GRAY);
    //     }
    //     else if(input == "color"){
    //         cv::cvtColor(img,img,cv::COLOR_GRAY2BGR);
    //     }
    //     else if(input == "otsu")
    //     {
    //         cv::threshold(img,img,128,255,cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
    //     }
    //     else if(input == "adaptive")
    //     {
    //         cv::adaptiveThreshold(img,img,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,9,2);
    //     }
    //     else if(input == "dilate")
    //     {
    //         cv::dilate(img,img,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));
    //     }
    //     else if (input == "erode")
    //     {
    //         cv::erode(img,img,cv::getStructuringElement(cv::MORPH_CROSS,cv::Size(7,7),cv::Point(-1,-1)));

    //     }
    //     else if (input == "hough")
    //     {
    //         cv:: Mat houghBinary = cv::Mat(img);
    //         std::vector<cv::Vec4i> lines;
    //         cv::HoughLines(houghBinary,lines, 10,CV_PI/180,30,30);
    //         for (cv::Vec4i line : lines){
    //             cv::line(houghBinary,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(0,0,255),4);
    //         }
    //         displayImg("Non-Probabilistic Hough on Binary",houghBinary);
    //     }
    //     else if (input == "houghp")
    //     {
    //         cv:: Mat houghBinary = cv::Mat(img);
    //         std::vector<cv::Vec4i> lines;
    //         cv::HoughLinesP(houghBinary,lines, 10,CV_PI/180,30,30);
    //         for (cv::Vec4i line : lines){
    //             cv::line(houghBinary,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(0,0,255),4);
    //         }
    //     }
    //     else if(input == "exit")
    //     {
    //         running = false;
    //     }
    //     else
    //     {

    //     }
    //     displayImg("",img);
    // }


    return 0;
}