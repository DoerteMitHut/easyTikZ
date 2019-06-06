#include <iostream>
#include <opencv2/opencv.hpp>
#include "Diagram.h"
#include "TikzGenerator.h"
#include "ShapeRecognition.h"
#include "preprocessing.h"

void displayImg(std::string window_name, cv::Mat img){
    cv::namedWindow(window_name,cv::WINDOW_NORMAL);
    cv::resizeWindow(window_name, 800,800);
    cv::imshow(window_name,img);
    cv::waitKey(0); //wait for a key press
    cv::destroyWindow(window_name);
}

double otsu(std::vector<double> hist)
{
    double total = 0;
    for(const double d : hist)
    {
            total += d;
    }
    unsigned int top = hist.size()-1;
    double sumB = 0;
    double wB = 0;
    double maximum = 0.0;
    double sum1 = 0;
    double toRet;
    for (unsigned int i = 0; i < hist.size(); i++)
    {
        sum1 += i*hist[i];
    }
    for (unsigned int ii = 0; ii < hist.size();ii++)
    {
        double wF = total - wB;
        if (wB > 0 && wF > 0)
        {
            double mF = (sum1 - sumB) / wF;
            double val = wB * wF * ((sumB / wB) - mF) * ((sumB / wB) - mF);
            if (val >= maximum)
            {
                toRet = ii;
                maximum = val;
            }
        }
        wB = wB + hist[ii];
        sumB = sumB + (ii-1) * hist[ii];
    }
    return toRet;
}


double maxxxxxxxx(std::vector<double> v)
{
    double max_val = 0;
    for (double x : v)
    {
        if (x > max_val)
        {
            max_val=x;
        }
    }
    return max_val;
}

void testMethod()
{
    auto firstBoi_ptr = std::make_shared<Rectangle>(2,1,"firstBoi",0,0);
    auto wingman_ptr = std::make_shared<Rectangle>(2,1,"wingman",0,0);

    Diagram littleD;
    littleD.insertRectangle(firstBoi_ptr);
    littleD.insertRectangle(wingman_ptr);
    //TODO: revert change so that insertRectangle is insertNode again

	DefaultAlign defaultAlign;

    TikzGenerator turningCertainShapesToAsh;
    turningCertainShapesToAsh.generateEasyTikZ(littleD, defaultAlign);
}

int main (int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// PARAMETER PROCESSING //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////
   
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

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// IMAGE PREPROCESSING ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //declaring all needed images
    cv::Mat imgGrayScale, imgBinary;
    
    //converting the original image into grayscale
    cv::cvtColor(img, imgGrayScale,cv::COLOR_BGR2GRAY);    
    displayImg("Grayscale Image",imgGrayScale);
    
    //equalize grayscale histogram
    cv::equalizeHist(imgGrayScale,imgGrayScale);

    //inverting image if value of edge is above average 
    if(imgGrayScale.at<double>(0,0) >128)
    {
     cv::subtract(cv::Scalar::all(255),imgGrayScale,imgGrayScale);
    }
    displayImg("inverted grayscale",imgGrayScale);

    //rescale image to measure 100px at longest side
    processImg(imgGrayScale,imgGrayScale);

    //apply thresholding to binarize image
    
    // cv::adaptiveThreshold(imgGrayScale,imgBinary,255,cv::ADAPTIVE_THRESH_GAUSSIAN_C,cv::THRESH_BINARY_INV,9,2);
    
    //apply Otsu's binarization method
    cv::threshold(imgGrayScale,imgBinary,128,255,cv::THRESH_BINARY | cv::THRESH_OTSU); 
    
    displayImg("Binary Image",imgBinary);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// SHAPE EXTRACTION //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    cv::Rect* bound = new cv::Rect();
    cv::Mat imgFilled;
    imgBinary.copyTo(imgFilled);
    std::cout <<"floddfill returned "<< cv::floodFill(imgFilled, cv::Point(1,1), cv::Scalar(255,255,255), bound, cv::Scalar(1), cv::Scalar(1), 4|(0<<8))<<std::endl;
    std::cout << "floodfill returned Rect at (" << bound->x <<"|"<<bound->y<<") with width " << bound->width << " and height" << bound->height<<"."<<std::endl; 
    std::cout << imgFilled.rows <<"|"<<imgFilled.cols<<std::endl;

    if(bound->height!=imgBinary.rows || bound->width != imgBinary.rows)
    {
        return -1;
    }
    
    delete bound;

    displayImg("filled",imgFilled);

    //invert filled image to suit shape recognition
    cv::subtract(cv::Scalar::all(255),imgFilled,imgFilled);

    displayImg("filled",imgFilled);

    //finding contours to approximate shapes
    std::vector<std::vector<cv::Point> > contours;

    cv::findContours(imgFilled, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE,cv::Point(0,0));
    
    std::cout<<"#contours = "<<contours.size()<<std::endl;

    //iterating through each contour
    cv::Mat fin,fin2;
    cv::cvtColor(imgGrayScale,fin,cv::COLOR_GRAY2BGR);
    cv::cvtColor(imgGrayScale,fin2,cv::COLOR_GRAY2BGR);
    std::vector<cv::Point2d> shape_centroids;
    for(std::vector<cv::Point> contour : contours)
    {
        //obtain a sequence of points of contour, pointed by the variable 'contour'
        std::vector<cv::Point> result;
        cv::approxPolyDP(contour, result , cv::arcLength(contour,true)*0.02, true);
        if(result.size() <= 6){
            cv::Moments mom = moments( contour, false );
            shape_centroids.push_back(cv::Point2d( mom.m10/mom.m00 , mom.m01/mom.m00));
            //initialize array of CvPoints
            std::vector<cv::Point> pt;
            //iterate over CvSeq-Array results and cast all elements into CvPoints to be inserted into the array
            for(cv::Point p : result){
                pt.insert(pt.end(),p);
            }
            //iterate over the point array and draw lines between consecutive points
            for(unsigned int i = 0; i < result.size();i++){
                //second point of line modulo #points to loop back to pt[0]
                std::cout<<"draw line"<<std::endl;
                cv::line(fin, pt[i],pt[ (i+1) % result.size()],cv::Scalar(0,255,0),4);
            }
        }
    }

    displayImg("finished",fin);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// EDGE DETECTION ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    double pointDistThresh = 20;

    //dilate filled image to cover edges
    unsigned int closingDepth = 4;
    for(int i = 0; i< closingDepth;i++){
        cv::dilate(imgFilled,imgFilled,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(11,11),cv::Point(-1,-1)));
        displayImg("Dilation #" + std::to_string(i),imgFilled);
    }

    //compute !imgFilled & imgBinary to delete shapes and their outlines and leave only connecting lines
    cv::Mat imgFilledInv,imgBinaryLines;
    cv::bitwise_not(imgFilled,imgFilledInv);
    cv::bitwise_and(imgFilledInv,imgBinary,imgBinaryLines);

    displayImg("lines only",imgBinaryLines);

    //find corners (endpoints) of edges via Harris corner detection
    cv::Mat imgHarris;
    cv::cornerHarris(imgBinaryLines,imgHarris,5,5,0.1);

    cv::dilate(imgHarris,imgHarris,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(7,7),cv::Point(-1,-1)));
    cv::threshold(imgHarris,imgHarris,0.5,1,cv::THRESH_BINARY);
    
    double min, max;
    cv::minMaxLoc(imgHarris, &min, &max);
    imgHarris-=min;
    imgHarris.convertTo(imgHarris,CV_8U,255.0/(max-min));
    
    std::vector<std::vector<cv::Point>> contours2;
    std::vector<cv::Vec4i> hierarchy;
    cv::Canny(imgHarris,imgHarris,50,150,3);
    displayImg("Harris",imgHarris);
    
    cv::findContours(imgHarris,contours2,hierarchy,cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0) );
    // get the moments
    std::vector<cv::Moments> mu(contours2.size());
    for(unsigned int i = 0; i<contours2.size(); i++ )
    {
        mu[i] = moments( contours2[i], false );
    }
    // get the centroid of figures.
    std::vector<cv::Point2d> mc;
    for(const cv::Moments m : mu)
    {
        std::cout << "("<<m.m10/m.m00<<"|"<< m.m01/m.m00<<")"<<std::endl;
        cv::Point2d mi = cv::Point2d( m.m10/m.m00 , m.m01/m.m00);
        bool valid = true;
        for (const cv::Point2d& p : mc)
        {
            if (cv::sqrt(cv::pow(mi.x - p.x,2.) + cv::pow(mi.y - p.y,2.)) < pointDistThresh)
            {
                std::cout<<"INVALIDATED"<<std::endl;
                valid = false;
            }
        }
        if(valid)
        {
            mc.push_back(mi); 
            cv::circle(imgHarris,cv::Point(mi),4,cv::Scalar(255),-1,8,0);
        }
    }


    std::vector<cv::Vec4i> edges;
    std::cout<<"Points found: "<<mc.size()<<std::endl;

    for(const cv::Point& p : mc)
    {
        for (const cv::Point& q : mc)
        {
            if(p!=q)
            {
                if(q.x <= p.x)
                {
                    edges.push_back(cv::Vec4i(q.x,q.y,p.x,p.y));
                }
                else
                {
                    edges.push_back(cv::Vec4i(p.x,p.y,q.x,q.y));
                }
                
            }
        }
    }
    displayImg("Harris",imgHarris);



    //find hough lines in line image
    std::vector<cv::Vec4i> lines;
    std::vector<double> support(edges.size());
    std::fill(support.begin(), support.end(), 0.);

    cv::HoughLinesP(imgBinaryLines,lines, 1,CV_PI/180,15,50,10);
    std::cout<<"found the following hough-lines:\n===========================\n";
    for(cv::Vec4i line : lines)
    {
        std::cout<<"("<<line[0]<<"|"<<line[1]<<")--("<<line[2]<<"|"<<line[3]<<")\n"<<std::endl;    
    }
    std::cout<<"detected "<<lines.size()<<" lines"<<std::endl;
    for (cv::Vec4i line : lines){
        double sum = cv::abs(line[2]-line[0])+cv::abs(line[3]-line[1]);
        //double nacken = nackenDist(line,motherLine,false);
        //std::cout << nacken<<std::endl;
        cv::line(fin,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(255*cv::abs(line[2]-line[0])/sum,0,255*cv::abs(line[3]-line[1])/sum),4);
        //cv::line(houghImg,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(255*nacken,0,255*1.-nacken),4);
        unsigned int bestIndex = edges.size();
        double bestNacken = -1;

        for(unsigned int i = 0; i < edges.size(); i++)
        {
            //std::cout<<"pre nacken"<<std::endl;
            std::cout.setstate(std::ios_base::failbit);
            double nacken = nackenDist(line,edges[i],true,15,50,20);
            std::cout.clear();
            //std::cout<<"nacken was "<<nacken<<std::endl;
            if(nacken > bestNacken)
            {
                //std::cout<<"\t...which is greater than "<<bestNacken<<std::endl;
                bestIndex = i;
                bestNacken = nacken;
            }
        }
        support[bestIndex]++;
    }
    for(unsigned int i = 0; i < edges.size(); i++)
    {
        if(support[i]>0)
        {
            std::cout<<"("<<edges[i][0]<<"|"<<edges[i][1]<<")--("<<edges[i][2]<<"|"<<edges[i][3]<<")"<<"\t: "<<support[i]<<std::endl;
        }
    }

    double thresh = otsu(support);
    std::cout<<"threshold is: "<<thresh<<std::endl;
    for (unsigned int i=0; i < edges.size();i++)
    {
        if(support[i]>=thresh)
        {
        cv::line(fin2,cv::Point(edges[i][0],edges[i][1]),cv::Point(edges[i][2],edges[i][3]),cv::Scalar(0,255,0),4);
        }
    }

    for(const cv::Point2d& p : shape_centroids)
    {
        cv::circle(fin2,p,4,cv::Scalar(0,0,255),-1,8,0);
    }

    displayImg("finished",fin2);

    Diagram littleD;
    DefaultAlign defaultAlign;

    for (const std::vector<cv::Point>&  contour : contours )
    {
        cv::Rect2d r = cv::boundingRect(contour);
        cv::Moments mom = cv::moments(contour,false);
        auto gutesRect = std::make_shared<Rectangle>(r.width/100,r.height/100,"firstBoi",(mom.m10/mom.m00)/100, -(mom.m01/mom.m00)/100);
        littleD.insertRectangle(gutesRect);
    }
    //TODO: revert change so that insertRectangle is insertNode again

	
    TikzGenerator turningCertainShapesToAsh;
    turningCertainShapesToAsh.generateEasyTikZ(littleD, defaultAlign);


    
    // cv::Vec4d mother(-1,0,1,0);
    // double oneDeg = 2*3.1456/360.;
    // cv::Vec4d cur;
    // std::cout << "oneDeg: "<<oneDeg<<std::endl;
    // for(unsigned int i = 0; i < 360; i++)
    // {
    //     cur = cv::Vec4d(-cos(i*oneDeg),-sin(i*oneDeg),cos(i*oneDeg),sin(i*oneDeg));
    //     std::cout << "trying "<< cur[0] << "|"<< cur[1] << "|"<<cur[2] << "|"<<cur[3] << "|"<<std::endl;
    //     std::cout << "Nacken distance for "<<i << " degree is: "<<nackenDist(cur,mother,true,1,1,1)<<std::endl;
    // }

    return 0;
}