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
    turningCertainShapesToAsh.generateEasyTikZ(littleD, &defaultAlign);
}

int main (int argc, char** argv)
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// PARAMETER PROCESSING //////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //check whether there is a command line argument given
    if (argc <2 || argc >4){
		std::cout<<"u   sage: main <Image_Path> [--tikz [--tex]]\n";
		return -1;
	}
    //if there is a command line argument, use it to set the path for the source image
	cv::Mat img =  cv::imread(argv[1]);
	if(img.empty())
	{
		std::cout<<"No image data!\n";
		return -1;
    }
    ///////////// flags to include LaTeX document frame and tikz environment in the output file
    //Flag to envelop output in tikzpicture environment
    bool TIKZ_ENV_FLAG = false;
    //Flag to envelop tikzpicture in Latex Document
    bool TEX_DOC_FLAG = false;
    if(argc >= 3)
    {
        if(std::strcmp(argv[2],"--tikz")==0)
        {
            TIKZ_ENV_FLAG = true;
            
            if(argc==4 && std::strcmp(argv[3],"--tex")==0)
            {
                TEX_DOC_FLAG = true;
            }
            else
            {
                TEX_DOC_FLAG = false;
            }      
        }
        else
        {
            TIKZ_ENV_FLAG = false;
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// IMAGE PREPROCESSING ///////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    //declaring all needed images
    cv::Mat imgGrayScale, imgGrayscaleInv, imgBinary, imgBinaryInv;

    //converting the original image into grayscale
    processImg(img,imgGrayScale,imgGrayscaleInv,imgBinary,imgBinaryInv);

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// SHAPE EXTRACTION //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    /////// POLYGONS //////////////////////////////////////
    ///////////////////////////////////////////////////////
    cv::Mat imgFilled;
    std::vector<std::vector<cv::Point2d>> shapes;

    displayImg("befor filling",imgBinary);
    fillShapes(imgBinary,imgFilled);
    displayImg("filled",imgFilled);
    findShapes(imgFilled,shapes);

    /////// CIRCLES ///////////////////////////////////////
    ///////////////////////////////////////////////////////

    // apply hough circles to find circles and draw a cirlce around it
    std::vector<cv::Vec3f> circles;

    {
        cv::Mat gaussian;
        GaussianBlur( imgFilled, gaussian, cv::Size(9, 9), 2, 2 );
        HoughCircles(imgFilled, circles, CV_HOUGH_GRADIENT, 1, gaussian.rows/8, 200,15);
        std::cout <<" | FOUND " << circles.size() << "circles"<<std::endl;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    /////// EDGE DETECTION ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////

    double pointDistThresh = 20;

    /////// ISOLATE LINES /////////////////////////////////
    ///////////////////////////////////////////////////////

    cv::Mat imgFilledInv,imgBinaryLines;

    //dilate filled image to cover edges
    unsigned int dilationDepth = 4;
    for(unsigned int i = 0; i< dilationDepth;i++){
        cv::dilate(imgFilled,imgFilled,cv::getStructuringElement(cv::MORPH_RECT,cv::Size(9,9),cv::Point(-1,-1)));
        displayImg("Dilation #" + std::to_string(i),imgFilled);
    }

    //compute !imgFilled & imgBinary to delete shapes and their outlines and leave only connecting lines
    cv::bitwise_not( imgFilled    ,imgFilledInv );
    cv::bitwise_and( imgFilledInv ,imgBinaryInv ,imgBinaryLines );

    displayImg("lines only",imgBinaryLines);

    /////// DETECT INTERSECTS AND CORNERS ///////////////
    ///////////////////////////////////////////////////////

    std::vector<cv::Point2d> mc;
    cv::Mat imgHarris;

    {
        //find corners (endpoints) of edges via Harris corner detection
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
    std::vector<cv::Vec4d> lines;
    std::vector<double> support(edges.size());
    std::fill(support.begin(), support.end(), 0.);

    cv::HoughLinesP(imgBinaryLines,lines, 1,CV_PI/180,15,50,10);
    std::cout<<"found the following hough-lines:\n===========================\n";
    for(const cv::Vec4d& line : lines)
    {
        std::cout<<"("<<line[0]<<"|"<<line[1]<<")--("<<line[2]<<"|"<<line[3]<<")\n"<<std::endl;
    }
    std::cout<<"detected "<<lines.size()<<" lines"<<std::endl;
    for (const cv::Vec4d& line : lines){
        double sum = cv::abs(line[2]-line[0])+cv::abs(line[3]-line[1]);
        //double nacken = nackenDist(line,motherLine,false);
        //std::cout << nacken<<std::endl;
        //cv::line(fin,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(255*cv::abs(line[2]-line[0])/sum,0,255*cv::abs(line[3]-line[1])/sum),4);
        //cv::line(houghImg,cv::Point(line[0],line[1]),cv::Point(line[2],line[3]),cv::Scalar(255*nacken,0,255*1.-nacken),4);
        unsigned int bestIndex = edges.size();
        double bestNacken = -1;

        for(unsigned int i = 0; i < edges.size(); i++)
        {
            //std::cout<<"pre nacken"<<std::endl;
            std::cout.setstate(std::ios_base::failbit);
            double len_line = std::sqrt(std::pow(line[0]-line[2],2)+std::pow(line[1]-line[3],2));
            double len_edge = std::sqrt(std::pow(edges[i][0]-edges[i][2],2)+std::pow(edges[i][1]-edges[i][3],2));
            double sigma = 0.1*len_edge;
            double lengthDiff = cv::exp(-0.5*cv::pow((len_line-
                                                len_edge)/sigma,2));

            double nacken = nackenDist(line,edges[i],true,15,50,20);
            std::cout.clear();
            std::cout<<"nacken: "<<nacken;
            //nacken *= lengthDiff;
            std::cout << "\t| with length factor "<< lengthDiff <<": " << nacken << std::endl;
            
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

//////////////////////////////////////////////////
//////////////////////////////////////////////////
double maxi = -1;
double sum = 0;
for(double s : support)
{
    if(s>maxi)
    {
        maxi = s;
    }
    sum += s;
}
// if(maxi!=0)
// {
//     for(unsigned int i = 0; i<support.size();i++)
//     {
//         support[i]=std::round(support[i]*(255./maxi));
//     }
// }



// std::vector<double> hist(256);
// std::fill(hist.begin(), hist.end(), 0);
// for(unsigned int i = 0; i<support.size();i++)
// {
//     hist[(int)support[i]]++;
// }

// for(unsigned int i = 0; i < hist.size(); i++)
// {
//     std::cout<< i<<": " <<hist[i]<<std::endl;
// }
//////////////////////////////////////////////////
//////////////////////////////////////////////////

std::vector<double> candidates;
double nSum = 0;
for(unsigned int i = 0; i < edges.size(); i++)
{
    if(support[i]>0)
    {
        candidates.push_back(support[i]);
    }
}


    double thresh = otsu(support);

    //double thresh = sum/candidates.size();
    std::cout<<"threshold is: "<<thresh<<std::endl;
    for (unsigned int i=0; i < edges.size();i++)
    {
        if(support[i]>=thresh)
        {
        //cv::line(fin2,cv::Point(edges[i][0],edges[i][1]),cv::Point(edges[i][2],edges[i][3]),cv::Scalar(0,255,0),4);
        }
    }

    // for(const cv::Point2d& p : shape_centroids)
    // {
    //     //cv::circle(fin2,p,4,cv::Scalar(0,0,255),-1,8,0);
    // }

    //displayImg("finished",fin2);

    Diagram littleD;
    DefaultAlign defaultAlign;

    // for (const std::vector<cv::Point>&  contour : contours )
    // {
    //     cv::Rect2d r = cv::boundingRect(contour);
    //     cv::Moments mom = cv::moments(contour,false);
    //     auto gutesRect = std::make_shared<Rectangle>(r.width/100,r.height/100,"firstBoi",(mom.m10/mom.m00)/100, -(mom.m01/mom.m00)/100);
    //     littleD.insertRectangle(gutesRect);
    // }
    //TODO: revert change so that insertRectangle is insertNode again


    TikzGenerator turningCertainShapesToAsh;
    turningCertainShapesToAsh.generateEasyTikZ(littleD, &defaultAlign);



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
